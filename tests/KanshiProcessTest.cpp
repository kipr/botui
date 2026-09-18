#include "KanshiProcess.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QtTest>

class KanshiProcessTest : public QObject
{
	Q_OBJECT
private slots:
	void findsMultipleKanshiProcesses();
	void usesInvokingUserWhenRunningWithSudo();
};

void KanshiProcessTest::findsMultipleKanshiProcesses()
{
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	for (int pid : {1019, 1025, 23557}) {
		const QString processPath = directory.filePath(QString::number(pid));
		QVERIFY(QDir().mkpath(processPath));
		QFile name(processPath + QStringLiteral("/comm"));
		QVERIFY(name.open(QIODevice::WriteOnly));
		const QByteArray processName = pid == 23557 ? QByteArray("grep\n") : QByteArray("kanshi\n");
		QCOMPARE(name.write(processName), processName.size());
	}
	const uint userId = QFileInfo(directory.filePath(QStringLiteral("1019"))).ownerId();
	const QVector<qint64> found = KanshiProcess::find(directory.path(), userId);
	QCOMPARE(found.size(), 2);
	QVERIFY(found.contains(1019));
	QVERIFY(found.contains(1025));
}

void KanshiProcessTest::usesInvokingUserWhenRunningWithSudo()
{
	QCOMPARE(KanshiProcess::selectUserId(0, QByteArray("1000"), 0), 1000U);
	QCOMPARE(KanshiProcess::selectUserId(0, QByteArray(), 1000), 1000U);
	QCOMPARE(KanshiProcess::selectUserId(1000, QByteArray("2000"), 0), 1000U);
}

QTEST_APPLESS_MAIN(KanshiProcessTest)
#include "KanshiProcessTest.moc"
