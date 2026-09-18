#include "ScreenOrientation.h"
#include "ConfigurationHome.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>

#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace {
const int kKanshiConfig = 0;
const int kKanshiInit = 1;
const int kLabwcConfig = 2;

bool transformOutput(const QByteArray &source, bool desired, QByteArray *result,
                     bool *current, QString *errorMessage)
{
	QString text = QString::fromUtf8(source);
	if (text.toUtf8() != source) {
		if (errorMessage) *errorMessage = QStringLiteral("Kanshi configuration is not valid UTF-8");
		return false;
	}
	const QRegularExpression outputPattern(QStringLiteral("^\\s*output\\s+HDMI-A-1(?=\\s)"));
	const QRegularExpression transformPattern(QStringLiteral("\\btransform\\s+(\\S+)"));
	QStringList lines = text.split(QLatin1Char('\n'), Qt::KeepEmptyParts);
	int matches = 0;
	for (QString &line : lines) {
		if (line.trimmed().startsWith(QLatin1Char('#')) ||
		    !outputPattern.match(line).hasMatch())
			continue;
		++matches;
		const QRegularExpressionMatch match = transformPattern.match(line);
		if (!match.hasMatch()) {
			if (errorMessage) *errorMessage = QStringLiteral("HDMI-A-1 needs a transform directive");
			return false;
		}
		auto tokens = transformPattern.globalMatch(line);
		int count = 0;
		while (tokens.hasNext()) { tokens.next(); ++count; }
		if (count != 1 || (match.captured(1) != QStringLiteral("normal") &&
		                   match.captured(1) != QStringLiteral("180"))) {
			if (errorMessage) *errorMessage = QStringLiteral("HDMI-A-1 has an ambiguous or unsupported transform");
			return false;
		}
		if (current) *current = match.captured(1) == QStringLiteral("180");
		line.replace(match.capturedStart(1), match.capturedLength(1),
		             desired ? QStringLiteral("180") : QStringLiteral("normal"));
	}
	if (matches != 1) {
		if (errorMessage) *errorMessage = QStringLiteral("Expected exactly one HDMI-A-1 output directive");
		return false;
	}
	if (result) *result = lines.join(QLatin1Char('\n')).toUtf8();
	return true;
}
}

ScreenOrientationPaths ScreenOrientationPaths::defaults()
{
	const QString home = ConfigurationHome::path();
	return {QDir(home).filePath(QStringLiteral(".config/kanshi/config")),
	        QDir(home).filePath(QStringLiteral(".config/kanshi/config.init")),
	        QDir(home).filePath(QStringLiteral(".config/labwc/rc.xml"))};
}

ScreenOrientationStore::ScreenOrientationStore(const ScreenOrientationPaths &paths)
	: m_paths(paths), m_loaded(false), m_inverted(false)
{
}

QString ScreenOrientationStore::path(int index) const
{
	return index == kKanshiConfig ? m_paths.kanshiConfig :
	       index == kKanshiInit ? m_paths.kanshiInit : m_paths.labwcConfig;
}

QString ScreenOrientationStore::backupPath(int index) const
{
	return path(index) + QStringLiteral(".invert.pending.bak");
}

QString ScreenOrientationStore::markerPath() const
{
	return m_paths.kanshiConfig + QStringLiteral(".invert.pending");
}

bool ScreenOrientationStore::pending() const
{
	return QFile::exists(markerPath());
}

bool ScreenOrientationStore::read(const QString &filePath, Snapshot *snapshot,
                                  QString *errorMessage) const
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		if (errorMessage) *errorMessage = QStringLiteral("Unable to read %1: %2").arg(filePath, file.errorString());
		return false;
	}
	snapshot->content = file.readAll();
	snapshot->permissions = file.permissions();
	const QFileInfo info(filePath);
	snapshot->ownerId = info.ownerId();
	snapshot->groupId = info.groupId();
	return true;
}

bool ScreenOrientationStore::write(const QString &filePath, const Snapshot &snapshot,
                                   QString *errorMessage) const
{
	QSaveFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		if (errorMessage) *errorMessage = QStringLiteral("Unable to write %1: %2").arg(filePath, file.errorString());
		return false;
	}
	if (::fchown(file.handle(), snapshot.ownerId, snapshot.groupId) != 0) {
		if (errorMessage) *errorMessage = QStringLiteral("Unable to preserve owner of %1: %2")
			.arg(filePath, QString::fromLocal8Bit(std::strerror(errno)));
		file.cancelWriting();
		return false;
	}
	if (!file.setPermissions(snapshot.permissions) ||
	    file.write(snapshot.content) != snapshot.content.size() || !file.commit()) {
		if (errorMessage) *errorMessage = QStringLiteral("Unable to write %1: %2").arg(filePath, file.errorString());
		return false;
	}
	return true;
}

bool ScreenOrientationStore::readBackup(int index, Snapshot *snapshot, QString *errorMessage) const
{
	return read(backupPath(index), snapshot, errorMessage);
}

bool ScreenOrientationStore::load(bool *inverted, QString *errorMessage)
{
	m_loaded = false;
	if (pending()) {
		if (errorMessage) *errorMessage = QStringLiteral("An unconfirmed screen change needs recovery");
		return false;
	}
	for (int index = 0; index < 3; ++index) {
		if (!read(path(index), &m_original[index], errorMessage)) return false;
	}
	const QByteArray active = m_original[kKanshiConfig].content.trimmed().isEmpty()
		? m_original[kKanshiInit].content : m_original[kKanshiConfig].content;
	QByteArray ignored;
	if (!transformOutput(active, false, &ignored, &m_inverted, errorMessage) ||
	    !transformOutput(m_original[kKanshiInit].content, false, &ignored, nullptr, errorMessage))
		return false;
	m_loaded = true;
	if (inverted) *inverted = m_inverted;
	return true;
}

bool ScreenOrientationStore::unchanged(QString *errorMessage) const
{
	for (int index = 0; index < 3; ++index) {
		Snapshot current;
		if (!read(path(index), &current, errorMessage)) return false;
		if (current.content != m_original[index].content) {
			if (errorMessage) *errorMessage = QStringLiteral("Screen configuration changed while preparing inversion");
			return false;
		}
	}
	return true;
}

bool ScreenOrientationStore::stage(bool inverted, QString *errorMessage)
{
	if (!m_loaded || pending() || !unchanged(errorMessage)) {
		if (errorMessage && errorMessage->isEmpty())
			*errorMessage = QStringLiteral("Screen configuration is not ready");
		return false;
	}
	if (inverted == m_inverted) {
		if (errorMessage) *errorMessage = QStringLiteral("Screen is already in the requested orientation");
		return false;
	}
	Snapshot changed[2] = {m_original[0], m_original[1]};
	if (changed[0].content.trimmed().isEmpty())
		changed[0].content = m_original[1].content;
	if (!transformOutput(changed[0].content, inverted, &changed[0].content, nullptr, errorMessage) ||
	    !transformOutput(changed[1].content, inverted, &changed[1].content, nullptr, errorMessage))
		return false;
	for (int index = 0; index < 3; ++index) {
		if (!write(backupPath(index), m_original[index], errorMessage)) return false;
	}
	Snapshot marker = {QByteArray("pending\n"), QFileDevice::ReadOwner | QFileDevice::WriteOwner,
	                   m_original[kKanshiConfig].ownerId, m_original[kKanshiConfig].groupId};
	if (!write(markerPath(), marker, errorMessage)) return false;
	for (int index = 0; index < 2; ++index) {
		if (!write(path(index), changed[index], errorMessage)) return false;
	}
	return true;
}

bool ScreenOrientationStore::restore(QString *errorMessage)
{
	if (!pending()) return true;
	Snapshot backups[3];
	for (int index = 0; index < 3; ++index) {
		if (!readBackup(index, &backups[index], errorMessage)) return false;
	}
	for (int index = 0; index < 3; ++index) {
		if (!write(path(index), backups[index], errorMessage)) return false;
	}
	return true;
}

bool ScreenOrientationStore::confirm(QString *errorMessage)
{
	if (!pending()) return true;
	if (!QFile::remove(markerPath())) {
		if (errorMessage) *errorMessage = QStringLiteral("Unable to clear screen change marker");
		return false;
	}
	return true;
}

bool ScreenOrientationStore::backupOrientation(bool *inverted, QString *errorMessage) const
{
	Snapshot config;
	Snapshot init;
	if (!readBackup(kKanshiConfig, &config, errorMessage) ||
	    !readBackup(kKanshiInit, &init, errorMessage)) return false;
	const QByteArray active = config.content.trimmed().isEmpty() ? init.content : config.content;
	return transformOutput(active, false, nullptr, inverted, errorMessage);
}
