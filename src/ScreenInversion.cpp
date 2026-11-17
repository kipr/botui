#include "ScreenInversion.h"

#include "RootController.h"
#include "ScreenOrientation.h"
#include "KanshiProcess.h"

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>


namespace {
bool run(const QString &executable, const QStringList &arguments, QByteArray *output,
         QString *errorMessage)
{
	const QString binary = QStandardPaths::findExecutable(executable);
	if (binary.isEmpty()) {
		if (errorMessage) *errorMessage = QStringLiteral("%1 executable was not found").arg(executable);
		return false;
	}
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start(binary, arguments);
	if (!process.waitForStarted(1000) || !process.waitForFinished(3000)) {
		process.kill();
		process.waitForFinished(500);
		if (errorMessage) *errorMessage = QStringLiteral("%1 could not start or timed out: %2").arg(executable, process.errorString());
		return false;
	}
	const QByteArray result = process.readAll();
	if (output) *output = result;
	if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
		if (errorMessage) *errorMessage = QStringLiteral("%1 failed: %2").arg(executable, QString::fromLocal8Bit(result).trimmed());
		return false;
	}
	return true;
}

bool outputTransform(bool *inverted, QString *errorMessage)
{
	QByteArray listing;
	if (!run(QStringLiteral("wlr-randr"), {}, &listing, errorMessage)) return false;
	const QStringList lines = QString::fromLocal8Bit(listing).split(QLatin1Char('\n'));
	bool inOutput = false;
	int found = 0;
	for (const QString &line : lines) {
		if (!line.isEmpty() && !line.at(0).isSpace()) {
			inOutput = line.startsWith(QStringLiteral("HDMI-A-1 "));
			if (inOutput) ++found;
		} else if (inOutput && line.trimmed().startsWith(QStringLiteral("Transform:"))) {
			const QString transform = line.section(QLatin1Char(':'), 1).trimmed();
			if (found != 1 || (transform != QStringLiteral("normal") && transform != QStringLiteral("180"))) {
				if (errorMessage) *errorMessage = QStringLiteral("HDMI-A-1 has an unsupported live transform");
				return false;
			}
			*inverted = transform == QStringLiteral("180");
			return true;
		}
	}
	if (errorMessage) *errorMessage = QStringLiteral("HDMI-A-1 was not found in wlr-randr output");
	return false;
}

bool apply(bool inverted, QString *errorMessage)
{
	if (!KanshiProcess::reload(errorMessage)) return false;
	if (!run(QStringLiteral("wlr-randr"),
	         {QStringLiteral("--output"), QStringLiteral("HDMI-A-1"), QStringLiteral("--transform"),
	          inverted ? QStringLiteral("180") : QStringLiteral("normal")}, nullptr, errorMessage))
		return false;
	bool actual = false;
	if (!outputTransform(&actual, errorMessage)) return false;
	if (actual != inverted) {
		if (errorMessage) *errorMessage = QStringLiteral("HDMI-A-1 did not adopt the requested transform");
		return false;
	}
	return true;
}

class InversionPage final : public QWidget
{
public:
	InversionPage()
		: m_store(), m_desired(false), m_seconds(15), m_confirming(false)
	{
		setObjectName(QStringLiteral("screenInversionPage"));
		auto *layout = new QVBoxLayout(this);
		m_label = new QLabel(tr("Preparing screen orientation..."), this);
		m_label->setAlignment(Qt::AlignCenter);
		m_label->setWordWrap(true);
		QFont font = m_label->font();
		font.setPointSize(20);
		m_label->setFont(font);
		layout->addWidget(m_label, 1);
		m_confirm = new QPushButton(tr("Keep This Orientation"), this);
		m_confirm->setObjectName(QStringLiteral("screenInversionConfirmButton"));
		m_confirm->setMinimumHeight(80);
		m_confirm->hide();
		layout->addWidget(m_confirm);
		m_revert = new QPushButton(tr("Cancel"), this);
		m_revert->setMinimumHeight(60);
		layout->addWidget(m_revert);
		connect(m_confirm, &QPushButton::clicked, this, [this]() { confirm(); });
		connect(m_revert, &QPushButton::clicked, this, [this]() {
			if (m_store.pending()) rollback(tr("Screen change reverted."));
			else leave();
		});
		m_timer.setInterval(1000);
		connect(&m_timer, &QTimer::timeout, this, [this]() {
			if (--m_seconds <= 0) rollback(tr("Screen change was not confirmed in time."));
			else updateLabel();
		});
		QTimer::singleShot(0, this, [this]() { start(); });
	}

private:
	void start()
	{
		QString error;
		bool current = false;
		bool live = false;
		if (!m_store.load(&current, &error) ||
		    KanshiProcess::findForSession().isEmpty() ||
		    !outputTransform(&live, &error)) {
			if (error.isEmpty()) error = tr("No Kanshi process owned by the session user was found.");
			showError(error);
			return;
		}
		m_desired = !current;
		if (!m_store.stage(m_desired, &error) || !apply(m_desired, &error)) {
			rollback(error);
			return;
		}
		m_confirming = true;
		m_confirm->show();
		m_revert->setText(tr("Revert Now"));
		m_timer.start();
		updateLabel();
	}

	void updateLabel()
	{
		m_label->setText(tr("Touch Keep This Orientation to save. Reverting in %1 seconds.").arg(m_seconds));
	}

	void confirm()
	{
		if (!m_confirming) return;
		m_timer.stop();
		QString error;
		if (!m_store.confirm(&error)) { showError(error); return; }
		m_confirming = false;
		m_confirm->hide();
		m_label->setText(tr("Screen orientation saved."));
		m_revert->setText(tr("Back"));
		QTimer::singleShot(500, this, [this]() { leave(); });
	}

	void rollback(const QString &reason)
	{
		m_timer.stop();
		m_confirming = false;
		m_confirm->hide();
		QString error;
		if (m_store.pending()) {
			if (!m_store.restore(&error)) { showError(reason + tr(" Restore failed: ") + error); return; }
			bool previous = false;
			if (!m_store.backupOrientation(&previous, &error)) { showError(reason + tr(" Recovery is still pending: ") + error); return; }
			if (!apply(previous, &error) || !m_store.confirm(&error)) {
				showError(reason + tr(" Recovery is still pending: ") + error);
				return;
			}
		}
		showError(reason);
	}

	void showError(const QString &message)
	{
		m_label->setText(message);
		m_confirm->hide();
		m_revert->setText(m_store.pending() ? tr("Retry Restore") : tr("Back"));
	}

	void leave()
	{
		RootController::ref().setDismissable(true);
		RootController::ref().dismissWidget();
	}

	ScreenOrientationStore m_store;
	QLabel *m_label;
	QPushButton *m_confirm;
	QPushButton *m_revert;
	QTimer m_timer;
	bool m_desired;
	int m_seconds;
	bool m_confirming;
};
}

void ScreenInversion::showPage()
{
	auto *page = new InversionPage;
	RootController::ref().presentWidget(page);
	if (RootController::ref().currentWidget() == page)
		RootController::ref().setDismissable(false);
}

bool ScreenInversion::recoverPending(QString *errorMessage)
{
	ScreenOrientationStore store;
	if (!store.pending()) return true;
	if (!store.restore(errorMessage)) return false;
	bool inverted = false;
	if (!store.backupOrientation(&inverted, errorMessage)) return false;
	if (!apply(inverted, errorMessage)) return false;
	return store.confirm(errorMessage);
}

bool ScreenInversion::synchronizeConfiguredOutput(QString *errorMessage)
{
	ScreenOrientationStore store;
	if (store.pending()) return true; // The confirmation page owns this transaction.
	bool configured = false;
	if (!store.load(&configured, errorMessage))
		return false;
	bool live = false;
	if (!outputTransform(&live, errorMessage))
		return false;
	return live == configured || apply(configured, errorMessage);
}
