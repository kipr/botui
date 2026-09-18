#include "Calibrate.h"

#include "RootController.h"
#include "TouchCalibration.h"

#include <QMouseEvent>
#include <QImage>
#include <QPainter>
#include <QProcess>
#include <QPushButton>
#include <QResizeEvent>
#include <QStandardPaths>
#include <QTimer>
#include <QWidget>

#include <cmath>

namespace {
const int kConfirmationSeconds = 15;
const qreal kConfirmationRadius = 35.0;
const double kMaximumFitError = 0.05;

class CalibrationPage final : public QWidget
{
public:
	explicit CalibrationPage(const QString &configPath)
		: m_store(configPath),
		  m_currentMatrix(CalibrationMatrix::identity()),
		  m_actionButton(new QPushButton(tr("Cancel"), this)),
		  m_countdownTimer(new QTimer(this)),
		  m_state(Collecting),
		  m_targetIndex(0),
		  m_secondsRemaining(kConfirmationSeconds),
		  m_restoreNeeded(false)
	{
		setObjectName(QStringLiteral("calibrationPage"));
		setAttribute(Qt::WA_OpaquePaintEvent);
		setFocusPolicy(Qt::StrongFocus);
		m_raccoon.load(QStringLiteral(":/raccoon.png"));

		m_targets << QPointF(0.12, 0.12)
		          << QPointF(0.88, 0.12)
		          << QPointF(0.50, 0.50)
		          << QPointF(0.12, 0.88)
		          << QPointF(0.88, 0.88);
		m_confirmationTarget = QPointF(0.50, 0.25);

		m_actionButton->setObjectName(QStringLiteral("calibrationActionButton"));
		m_actionButton->setMinimumSize(150, 48);
		m_actionButton->setStyleSheet(
			QStringLiteral("QPushButton { background: #e5dfd2; border: 1px solid #665f52; "
			"border-radius: 4px; color: #1c1e22; font-weight: bold; } "
			"QPushButton:pressed { background: #d2c9b8; }"));
		connect(m_actionButton, &QPushButton::clicked, this, [this]() { handleAction(); });
		m_countdownTimer->setInterval(1000);
		connect(m_countdownTimer, &QTimer::timeout, this, [this]() { countdown(); });

		QString errorMessage;
		if (!m_store.load(&m_currentMatrix, &errorMessage))
			showError(errorMessage, false);
	}

protected:
	void paintEvent(QPaintEvent *) override
	{
		QPainter painter(this);
		painter.fillRect(rect(), QColor(250, 248, 241));
		painter.setRenderHint(QPainter::Antialiasing);

		QFont headingFont = font();
		headingFont.setPointSize(18);
		headingFont.setBold(true);
		painter.setFont(headingFont);
		painter.setPen(QColor(28, 30, 34));

		QString heading;
		if (m_state == Collecting) {
			heading = tr("Touch the center of each target (%1 of %2)")
				.arg(m_targetIndex + 1).arg(m_targets.size());
		} else if (m_state == Confirming) {
			heading = tr("Touch the confirmation target to keep this calibration");
		} else {
			heading = m_message;
		}
		painter.drawText(QRect(180, 18, qMax(0, width() - 360), 72),
		                 Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, heading);

		if (m_state == Collecting || m_state == Confirming) {
			const QPointF normalized = m_state == Collecting
				? m_targets.at(m_targetIndex) : m_confirmationTarget;
			drawTarget(&painter, pixelPoint(normalized));
		}

		if (m_state == Confirming) {
			QFont countdownFont = font();
			countdownFont.setPointSize(15);
			painter.setFont(countdownFont);
			painter.setPen(QColor(96, 63, 0));
			painter.drawText(QRect(20, qMax(0, height() - 120),
			                       qMax(0, width() - 40), 40),
			                 Qt::AlignHCenter | Qt::AlignTop,
			                 tr("Reverting in %1 seconds").arg(m_secondsRemaining));
		}
	}

	void resizeEvent(QResizeEvent *event) override
	{
		QWidget::resizeEvent(event);
		const int buttonWidth = 170;
		const int buttonHeight = 48;
		m_actionButton->setGeometry((width() - buttonWidth) / 2,
		                            qMax(0, height() - buttonHeight - 12),
		                            buttonWidth, buttonHeight);
	}

	void mouseReleaseEvent(QMouseEvent *event) override
	{
		if (event->button() != Qt::LeftButton) {
			QWidget::mouseReleaseEvent(event);
			return;
		}

		if (m_state == Collecting) {
			const qreal usableWidth = qMax(1, width() - 1);
			const qreal usableHeight = qMax(1, height() - 1);
			m_observed.append(QPointF(event->position().x() / usableWidth,
			                          event->position().y() / usableHeight));
			++m_targetIndex;
			if (m_targetIndex == m_targets.size())
				applyCandidate();
			else
				update();
			event->accept();
			return;
		}

		if (m_state == Confirming) {
			const QPointF difference = event->position() - pixelPoint(m_confirmationTarget);
			if (std::sqrt(QPointF::dotProduct(difference, difference)) <= kConfirmationRadius)
				confirmCandidate();
			event->accept();
			return;
		}

		QWidget::mouseReleaseEvent(event);
	}

private:
	enum State
	{
		Collecting,
		Confirming,
		Message
	};

	QPointF pixelPoint(const QPointF &normalized) const
	{
		return QPointF(normalized.x() * qMax(1, width() - 1),
		               normalized.y() * qMax(1, height() - 1));
	}

	void drawTarget(QPainter *painter, const QPointF &target) const
	{
		if (!m_raccoon.isNull()) {
			const qreal maximumSize = qMax<qreal>(56.0,
				qMin<qreal>(100.0, qMin(width(), height()) * 0.21));
			QSize artworkSize = m_raccoon.size();
			artworkSize.scale(QSize(qRound(maximumSize), qRound(maximumSize)), Qt::KeepAspectRatio);
			const QRectF artworkRect(target.x() - artworkSize.width() / 2.0,
			                         target.y() - artworkSize.height() / 2.0,
			                         artworkSize.width(), artworkSize.height());
			painter->drawImage(artworkRect, m_raccoon);
		}

		painter->setPen(QPen(QColor(214, 45, 45), 4));
		painter->setBrush(Qt::NoBrush);
		painter->drawEllipse(target, 16, 16);
		painter->drawLine(QPointF(target.x() - 25, target.y()),
		                  QPointF(target.x() + 25, target.y()));
		painter->drawLine(QPointF(target.x(), target.y() - 25),
		                  QPointF(target.x(), target.y() + 25));
	}

	void applyCandidate()
	{
		CalibrationMatrix correction;
		double fitError = 0.0;
		if (!TouchCalibration::fitCorrection(m_observed, m_targets, &correction, &fitError) ||
		    fitError > kMaximumFitError) {
			showError(tr("The calibration taps were inconsistent. Please go back and try again."), false);
			return;
		}

		const CalibrationMatrix candidate = TouchCalibration::compose(correction, m_currentMatrix);
		if (!TouchCalibration::isSafeCandidate(candidate)) {
			showError(tr("The calculated calibration was outside the safe range. Please go back and try again."), false);
			return;
		}

		QString errorMessage;
		if (!m_store.installCandidate(candidate, &errorMessage)) {
			if (m_store.candidateInstalled())
				rollback(tr("The Labwc configuration could not be updated: %1").arg(errorMessage));
			else
				showError(errorMessage, false);
			return;
		}
		if (!reconfigureLabwc(&errorMessage)) {
			rollback(tr("Labwc could not apply the new calibration: %1").arg(errorMessage));
			return;
		}

		m_state = Confirming;
		m_secondsRemaining = kConfirmationSeconds;
		m_actionButton->setText(tr("Revert Now"));
		m_countdownTimer->start();
		update();
	}

	bool reconfigureLabwc(QString *errorMessage)
	{
		const QString executable = QStandardPaths::findExecutable(QStringLiteral("labwc"));
		if (executable.isEmpty()) {
			if (errorMessage)
				*errorMessage = tr("labwc executable was not found");
			return false;
		}

		QProcess process;
		process.setProcessChannelMode(QProcess::MergedChannels);
		process.start(executable, QStringList() << QStringLiteral("--reconfigure"));
		if (!process.waitForStarted(1000)) {
			if (errorMessage)
				*errorMessage = process.errorString();
			return false;
		}
		if (!process.waitForFinished(3000)) {
			process.kill();
			process.waitForFinished(500);
			if (errorMessage)
				*errorMessage = tr("labwc --reconfigure timed out");
			return false;
		}
		if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
			if (errorMessage) {
				const QString output = QString::fromLocal8Bit(process.readAll()).trimmed();
				*errorMessage = output.isEmpty()
					? tr("labwc --reconfigure exited with code %1").arg(process.exitCode())
					: output;
			}
			return false;
		}
		return true;
	}

	void countdown()
	{
		--m_secondsRemaining;
		if (m_secondsRemaining <= 0)
			rollback(tr("The calibration was not confirmed in time."));
		else
			update();
	}

	void confirmCandidate()
	{
		m_countdownTimer->stop();
		m_state = Message;
		m_message = tr("Calibration saved.");
		m_actionButton->hide();
		update();
		QTimer::singleShot(350, this, [this]() { leavePage(); });
	}

	void rollback(const QString &reason)
	{
		m_countdownTimer->stop();
		QString restoreError;
		if (m_store.candidateInstalled() && !m_store.restoreBackup(&restoreError)) {
			showError(tr("%1\n\nThe backup could not be restored: %2")
			          .arg(reason, restoreError), true);
			return;
		}

		QString reconfigureError;
		if (!reconfigureLabwc(&reconfigureError)) {
			showError(tr("%1\n\nThe backup was restored, but Labwc could not reload it: %2")
			          .arg(reason, reconfigureError), false);
			return;
		}

		m_state = Message;
		m_message = tr("%1\n\nThe previous calibration has been restored.").arg(reason);
		m_actionButton->hide();
		update();
		QTimer::singleShot(700, this, [this]() { leavePage(); });
	}

	void showError(const QString &message, bool restoreNeeded)
	{
		m_countdownTimer->stop();
		m_state = Message;
		m_message = message;
		m_restoreNeeded = restoreNeeded;
		m_actionButton->setText(restoreNeeded ? tr("Retry Restore") : tr("Back"));
		m_actionButton->show();
		update();
	}

	void handleAction()
	{
		if (m_state == Collecting) {
			leavePage();
		} else if (m_state == Confirming) {
			rollback(tr("Calibration was reverted."));
		} else if (m_restoreNeeded) {
			rollback(tr("Retrying calibration rollback."));
		} else {
			leavePage();
		}
	}

	void leavePage()
	{
		m_countdownTimer->stop();
		RootController::ref().setDismissable(true);
		RootController::ref().dismissWidget();
	}

	LabwcCalibrationStore m_store;
	CalibrationMatrix m_currentMatrix;
	QPushButton *m_actionButton;
	QTimer *m_countdownTimer;
	QImage m_raccoon;
	State m_state;
	QVector<QPointF> m_targets;
	QVector<QPointF> m_observed;
	QPointF m_confirmationTarget;
	int m_targetIndex;
	int m_secondsRemaining;
	QString m_message;
	bool m_restoreNeeded;
};
}

void Calibrate::calibrate()
{
	CalibrationPage *page = new CalibrationPage(LabwcCalibrationStore::defaultConfigPath());
	RootController::ref().presentWidget(page);
	if (RootController::ref().currentWidget() == page)
		RootController::ref().setDismissable(false);
}

Calibrate::Calibrate()
{
}
