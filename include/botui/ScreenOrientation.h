#ifndef SCREENORIENTATION_H
#define SCREENORIENTATION_H

#include <QByteArray>
#include <QFileDevice>
#include <QString>

struct ScreenOrientationPaths
{
	QString kanshiConfig;
	QString kanshiInit;
	QString labwcConfig;

	static ScreenOrientationPaths defaults();
};

class ScreenOrientationStore
{
public:
	explicit ScreenOrientationStore(const ScreenOrientationPaths &paths = ScreenOrientationPaths::defaults());

	bool load(bool *inverted, QString *errorMessage);
	bool stage(bool inverted, QString *errorMessage);
	bool restore(QString *errorMessage);
	bool confirm(QString *errorMessage);
	bool backupOrientation(bool *inverted, QString *errorMessage) const;
	bool pending() const;
	QString markerPath() const;

private:
	struct Snapshot {
		QByteArray content;
		QFileDevice::Permissions permissions;
		uint ownerId;
		uint groupId;
	};
	bool read(const QString &path, Snapshot *snapshot, QString *errorMessage) const;
	bool write(const QString &path, const Snapshot &snapshot, QString *errorMessage) const;
	bool readBackup(int index, Snapshot *snapshot, QString *errorMessage) const;
	QString path(int index) const;
	QString backupPath(int index) const;
	bool unchanged(QString *errorMessage) const;

	ScreenOrientationPaths m_paths;
	Snapshot m_original[3];
	bool m_loaded;
	bool m_inverted;
};

#endif
