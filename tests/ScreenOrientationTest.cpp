#include "ScreenOrientation.h"

#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QtTest>

namespace {
QByteArray readFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) return {};
	return file.readAll();
}

bool writeFile(const QString &path, const QByteArray &data)
{
	QFile file(path);
	return file.open(QIODevice::WriteOnly) && file.write(data) == data.size();
}

const QByteArray kInitial =
	"# keep this comment\nprofile {\n\toutput HDMI-A-1 enable scale 1.000000 "
	"mode 800x480@60.004 position 0,0 transform normal\n}\n";
const QByteArray kLabwc =
	"<?xml version=\"1.0\"?><openbox_config xmlns=\"http://openbox.org/3.4/rc\">"
	"<libinput><device category=\"TSC2007 Touchscreen\">"
	"<calibrationMatrix>1.04 0 -0.02 0 1.02 -0.01</calibrationMatrix>"
	"</device></libinput></openbox_config>";

ScreenOrientationPaths paths(const QTemporaryDir &directory)
{
	return {directory.filePath(QStringLiteral("config")),
	        directory.filePath(QStringLiteral("config.init")),
	        directory.filePath(QStringLiteral("rc.xml"))};
}
}

class ScreenOrientationTest : public QObject
{
	Q_OBJECT
private slots:
	void togglesAndPreservesFiles();
	void rejectsConcurrentChange();
	void rejectsAmbiguousAndUnsupportedProfiles();
	void recoversPendingChange();
};

void ScreenOrientationTest::togglesAndPreservesFiles()
{
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const auto p = paths(directory);
	QVERIFY(writeFile(p.kanshiConfig, QByteArray()));
	QVERIFY(writeFile(p.kanshiInit, kInitial));
	QVERIFY(writeFile(p.labwcConfig, kLabwc));
	const auto permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner;
	QVERIFY(QFile::setPermissions(p.kanshiConfig, permissions));
	const auto originalPermissions = QFile::permissions(p.kanshiConfig);
	const uint originalOwner = QFileInfo(p.kanshiConfig).ownerId();
	const uint originalGroup = QFileInfo(p.kanshiConfig).groupId();
	ScreenOrientationStore store(p);
	bool inverted = true;
	QString error;
	QVERIFY2(store.load(&inverted, &error), qPrintable(error));
	QVERIFY(!inverted);
	QVERIFY2(store.stage(true, &error), qPrintable(error));
	QVERIFY(store.pending());
	QVERIFY(readFile(p.kanshiConfig).contains("transform 180"));
	QVERIFY(readFile(p.kanshiInit).contains("transform 180"));
	QVERIFY(readFile(p.kanshiConfig).contains("mode 800x480@60.004 position 0,0"));
	QVERIFY(readFile(p.kanshiConfig).contains("keep this comment"));
	QCOMPARE(QFile::permissions(p.kanshiConfig), originalPermissions);
	QCOMPARE(QFileInfo(p.kanshiConfig).ownerId(), originalOwner);
	QCOMPARE(QFileInfo(p.kanshiConfig).groupId(), originalGroup);
	QCOMPARE(readFile(p.labwcConfig), kLabwc);
	QCOMPARE(readFile(p.labwcConfig + QStringLiteral(".invert.pending.bak")), kLabwc);
	QVERIFY(!QFile::exists(p.labwcConfig + QStringLiteral(".bak")));
	QVERIFY2(store.confirm(&error), qPrintable(error));
	QVERIFY(!store.pending());
	ScreenOrientationStore second(p);
	QVERIFY2(second.load(&inverted, &error), qPrintable(error));
	QVERIFY(inverted);
	QVERIFY2(second.stage(false, &error), qPrintable(error));
	QVERIFY(readFile(p.kanshiConfig).contains("transform normal"));
	QCOMPARE(readFile(p.labwcConfig), kLabwc);
	QVERIFY2(second.restore(&error), qPrintable(error));
	QCOMPARE(readFile(p.kanshiConfig), readFile(p.kanshiConfig + QStringLiteral(".invert.pending.bak")));
	QVERIFY2(second.confirm(&error), qPrintable(error));
}

void ScreenOrientationTest::rejectsConcurrentChange()
{
	QTemporaryDir directory;
	const auto p = paths(directory);
	QVERIFY(writeFile(p.kanshiConfig, kInitial));
	QVERIFY(writeFile(p.kanshiInit, kInitial));
	QVERIFY(writeFile(p.labwcConfig, kLabwc));
	ScreenOrientationStore store(p);
	bool inverted = false;
	QString error;
	QVERIFY(store.load(&inverted, &error));
	QVERIFY(writeFile(p.kanshiConfig, kInitial + "# changed\n"));
	QVERIFY(!store.stage(true, &error));
	QVERIFY(!store.pending());
	QVERIFY(!QFile::exists(p.labwcConfig + QStringLiteral(".invert.pending.bak")));
}

void ScreenOrientationTest::rejectsAmbiguousAndUnsupportedProfiles()
{
	const QByteArray bad[] = {
		"profile { output HDMI-A-1 transform 90 }\n",
		"profile {\n output HDMI-A-1 transform 90\n}\n",
		"profile {\n output HDMI-A-1 transform normal\n output HDMI-A-1 transform 180\n}\n",
		"profile {\n output HDMI-A-1 transform normal transform 180\n}\n"
	};
	for (const QByteArray &content : bad) {
		QTemporaryDir directory;
		const auto p = paths(directory);
		QVERIFY(writeFile(p.kanshiConfig, content));
		QVERIFY(writeFile(p.kanshiInit, kInitial));
		QVERIFY(writeFile(p.labwcConfig, kLabwc));
		ScreenOrientationStore store(p);
		bool inverted = false;
		QString error;
		QVERIFY(!store.load(&inverted, &error));
		QVERIFY(!error.isEmpty());
	}
}

void ScreenOrientationTest::recoversPendingChange()
{
	QTemporaryDir directory;
	const auto p = paths(directory);
	QVERIFY(writeFile(p.kanshiConfig, QByteArray()));
	QVERIFY(writeFile(p.kanshiInit, kInitial));
	QVERIFY(writeFile(p.labwcConfig, kLabwc));
	QString error;
	bool inverted = false;
	{
		ScreenOrientationStore first(p);
		QVERIFY(first.load(&inverted, &error));
		QVERIFY(first.stage(true, &error));
	}
	ScreenOrientationStore restarted(p);
	QVERIFY(restarted.pending());
	QVERIFY2(restarted.backupOrientation(&inverted, &error), qPrintable(error));
	QVERIFY(!inverted);
	QVERIFY2(restarted.restore(&error), qPrintable(error));
	QCOMPARE(readFile(p.kanshiConfig), QByteArray());
	QCOMPARE(readFile(p.kanshiInit), kInitial);
	QCOMPARE(readFile(p.labwcConfig), kLabwc);
	QVERIFY2(restarted.confirm(&error), qPrintable(error));
	QVERIFY(!restarted.pending());
}

QTEST_APPLESS_MAIN(ScreenOrientationTest)
#include "ScreenOrientationTest.moc"
