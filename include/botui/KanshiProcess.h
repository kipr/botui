#ifndef KANSHIPROCESS_H
#define KANSHIPROCESS_H

#include <QByteArray>
#include <QString>
#include <QVector>

namespace KanshiProcess
{
	uint selectUserId(uint effectiveId, const QByteArray &sudoId, uint homeOwnerId);
	uint sessionUserId();
	QVector<qint64> find(const QString &procPath, uint userId);
	QVector<qint64> findForSession();
	bool reload(QString *errorMessage);
}

#endif
