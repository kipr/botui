#include "TouchCalibration.h"

#include <QDomDocument>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include <cmath>
#include <limits>

namespace {
const char kHeader[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
const char kNamespace[] = "http://openbox.org/3.4/rc";

bool writeFile(const QString &path, const QByteArray &content)
{
	QFile file(path);
	return file.open(QIODevice::WriteOnly) && file.write(content) == content.size();
}

QByteArray readFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return QByteArray();
	return file.readAll();
}

void compareMatrix(const CalibrationMatrix &actual, const CalibrationMatrix &expected,
	               double tolerance = 1.0e-6)
{
	for (int index = 0; index < 6; ++index)
		QVERIFY2(std::abs(actual.values[index] - expected.values[index]) <= tolerance,
		         qPrintable(QStringLiteral("matrix value %1 differs: %2 != %3")
		                    .arg(index).arg(actual.values[index]).arg(expected.values[index])));
}
}

class TouchCalibrationTest : public QObject
{
Q_OBJECT
private slots:
	void fitsAffineCorrectionAndComposesWithCurrentMatrix();
	void fitsNoisyCalibrationTaps();
	void rejectsSingularAndUnsafeMatrices();
	void replacesNamespacedMatrixAndRestoresExactBackup();
	void insertsMissingNamespacedElements_data();
	void insertsMissingNamespacedElements();
	void rejectsInvalidOrAmbiguousConfiguration_data();
	void rejectsInvalidOrAmbiguousConfiguration();
};

void TouchCalibrationTest::fitsAffineCorrectionAndComposesWithCurrentMatrix()
{
	const CalibrationMatrix expectedCorrection = {{1.03, 0.02, -0.015, -0.01, 0.97, 0.025}};
	const QVector<QPointF> observed = {
		QPointF(0.1, 0.1), QPointF(0.9, 0.1), QPointF(0.5, 0.5),
		QPointF(0.1, 0.9), QPointF(0.9, 0.9)
	};
	CalibrationMatrix identityFit;
	QVERIFY(TouchCalibration::fitCorrection(observed, observed, &identityFit));
	compareMatrix(identityFit, CalibrationMatrix::identity());

	QVector<QPointF> expected;
	for (const QPointF &point : observed)
		expected.append(expectedCorrection.map(point));

	CalibrationMatrix fitted;
	double error = -1.0;
	QVERIFY(TouchCalibration::fitCorrection(observed, expected, &fitted, &error));
	compareMatrix(fitted, expectedCorrection);
	QVERIFY(error < 1.0e-10);

	const CalibrationMatrix current = {{1.08, 0.0, -0.04, 0.0, 1.12, -0.06}};
	const CalibrationMatrix composed = TouchCalibration::compose(fitted, current);
	const QPointF sample(0.37, 0.63);
	const QPointF expectedPoint = fitted.map(current.map(sample));
	const QPointF actualPoint = composed.map(sample);
	QVERIFY(QPointF::dotProduct(expectedPoint - actualPoint, expectedPoint - actualPoint) < 1.0e-20);
}

void TouchCalibrationTest::fitsNoisyCalibrationTaps()
{
	const CalibrationMatrix expectedCorrection = {{0.96, 0.035, 0.018, -0.025, 1.04, -0.012}};
	const QVector<QPointF> observed = {
		QPointF(0.12, 0.12), QPointF(0.88, 0.12), QPointF(0.50, 0.50),
		QPointF(0.12, 0.88), QPointF(0.88, 0.88)
	};
	const QVector<QPointF> noise = {
		QPointF(0.001, -0.002), QPointF(-0.002, 0.001), QPointF(0.0015, 0.001),
		QPointF(-0.001, -0.0015), QPointF(0.0005, 0.0015)
	};
	QVector<QPointF> expected;
	for (int index = 0; index < observed.size(); ++index)
		expected.append(expectedCorrection.map(observed.at(index)) + noise.at(index));

	CalibrationMatrix fitted;
	double error = 0.0;
	QVERIFY(TouchCalibration::fitCorrection(observed, expected, &fitted, &error));
	compareMatrix(fitted, expectedCorrection, 0.006);
	QVERIFY(error > 0.0);
	QVERIFY(error < 0.005);
}

void TouchCalibrationTest::rejectsSingularAndUnsafeMatrices()
{
	const QVector<QPointF> singular(3, QPointF(0.5, 0.5));
	CalibrationMatrix fitted;
	QVERIFY(!TouchCalibration::fitCorrection(singular, singular, &fitted));

	CalibrationMatrix collapsed = CalibrationMatrix::identity();
	collapsed.values[0] = 0.0;
	QVERIFY(!TouchCalibration::isSafeCandidate(collapsed));
	CalibrationMatrix farOutside = CalibrationMatrix::identity();
	farOutside.values[2] = 0.5;
	QVERIFY(!TouchCalibration::isSafeCandidate(farOutside));
	CalibrationMatrix notFinite = CalibrationMatrix::identity();
	notFinite.values[4] = std::numeric_limits<double>::infinity();
	QVERIFY(!TouchCalibration::isSafeCandidate(notFinite));
	QVERIFY(TouchCalibration::isSafeCandidate(CalibrationMatrix::identity()));
}

void TouchCalibrationTest::replacesNamespacedMatrixAndRestoresExactBackup()
{
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const QString path = directory.filePath(QStringLiteral("rc.xml"));
	const QByteArray original = QByteArray(kHeader) +
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\">\n"
		"  <!-- keep this comment -->\n"
		"  <touch deviceName=\"TSC2007 Touchscreen\" mapToOutput=\"HDMI-A-1\" mouseEmulation=\"yes\"/>\n"
		"  <libinput><device category=\"TSC2007 Touchscreen\">\n"
		"    <calibrationMatrix>1 0 0 0 1 0</calibrationMatrix>\n"
		"    <pointerSpeed>0.25</pointerSpeed>\n"
		"  </device></libinput>\n"
		"</openbox_config>\n";
	QVERIFY(writeFile(path, original));
	const QFileDevice::Permissions permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner;
	QVERIFY(QFile::setPermissions(path, permissions));
	const QFileDevice::Permissions originalPermissions = QFile::permissions(path);

	LabwcCalibrationStore store(path);
	CalibrationMatrix current;
	QString error;
	QVERIFY2(store.load(&current, &error), qPrintable(error));
	compareMatrix(current, CalibrationMatrix::identity());

	const CalibrationMatrix candidate = {{1.08, -0.01, -0.04, 0.02, 1.10, -0.07}};
	QVERIFY2(store.installCandidate(candidate, &error), qPrintable(error));
	QCOMPARE(readFile(store.backupPath()), original);
	QCOMPARE(QFile::permissions(path), originalPermissions);

	const QByteArray updated = readFile(path);
	QVERIFY(updated.startsWith("<?xml"));
	QVERIFY(updated.contains("keep this comment"));
	QVERIFY(updated.contains("mapToOutput=\"HDMI-A-1\""));
	QDomDocument document;
	const QDomDocument::ParseResult parseResult = document.setContent(
		updated, QDomDocument::ParseOption::UseNamespaceProcessing);
	QVERIFY2(parseResult, qPrintable(parseResult.errorMessage));
	QCOMPARE(document.documentElement().localName(), QStringLiteral("openbox_config"));
	QCOMPARE(document.documentElement().namespaceURI(), QString::fromLatin1(kNamespace));
	const QDomNodeList pointerSpeeds = document.elementsByTagNameNS(
		QString::fromLatin1(kNamespace), QStringLiteral("pointerSpeed"));
	QCOMPARE(pointerSpeeds.size(), 1);
	QCOMPARE(pointerSpeeds.at(0).toElement().text(), QStringLiteral("0.25"));
	const QDomNodeList matrices = document.elementsByTagNameNS(
		QString::fromLatin1(kNamespace), QStringLiteral("calibrationMatrix"));
	QCOMPARE(matrices.size(), 1);
	QCOMPARE(matrices.at(0).toElement().text(),
	         QStringLiteral("1.080000 -0.010000 -0.040000 0.020000 1.100000 -0.070000"));

	QVERIFY2(store.restoreBackup(&error), qPrintable(error));
	QCOMPARE(readFile(path), original);
	QCOMPARE(QFile::permissions(path), originalPermissions);
}

void TouchCalibrationTest::insertsMissingNamespacedElements_data()
{
	QTest::addColumn<QByteArray>("body");
	QTest::newRow("missing-matrix")
		<< QByteArray("<libinput><device category=\"TSC2007 Touchscreen\"/></libinput>");
	QTest::newRow("missing-device") << QByteArray("<libinput/>");
	QTest::newRow("missing-libinput") << QByteArray("<touch mouseEmulation=\"yes\"/>");
}

void TouchCalibrationTest::insertsMissingNamespacedElements()
{
	QFETCH(QByteArray, body);
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const QString path = directory.filePath(QStringLiteral("rc.xml"));
	const QByteArray original = QByteArray(kHeader) +
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\">" + body +
		"</openbox_config>\n";
	QVERIFY(writeFile(path, original));

	LabwcCalibrationStore store(path);
	CalibrationMatrix current;
	QString error;
	QVERIFY2(store.load(&current, &error), qPrintable(error));
	compareMatrix(current, CalibrationMatrix::identity());
	const CalibrationMatrix candidate = {{1.02, 0.0, -0.01, 0.0, 1.03, -0.02}};
	QVERIFY2(store.installCandidate(candidate, &error), qPrintable(error));

	QDomDocument document;
	const QDomDocument::ParseResult parseResult = document.setContent(
		readFile(path), QDomDocument::ParseOption::UseNamespaceProcessing);
	QVERIFY2(parseResult, qPrintable(parseResult.errorMessage));
	QCOMPARE(document.elementsByTagNameNS(QString::fromLatin1(kNamespace),
	                                      QStringLiteral("libinput")).size(), 1);
	QCOMPARE(document.elementsByTagNameNS(QString::fromLatin1(kNamespace),
	                                      QStringLiteral("device")).size(), 1);
	QCOMPARE(document.elementsByTagNameNS(QString::fromLatin1(kNamespace),
	                                      QStringLiteral("calibrationMatrix")).size(), 1);
}

void TouchCalibrationTest::rejectsInvalidOrAmbiguousConfiguration_data()
{
	QTest::addColumn<QByteArray>("document");
	QTest::newRow("malformed") << QByteArray("<openbox_config>");
	QTest::newRow("wrong-root") << QByteArray(kHeader) +
		"<labwc_config xmlns=\"http://openbox.org/3.4/rc\"/>";
	QTest::newRow("wrong-namespace") << QByteArray(kHeader) +
		"<openbox_config xmlns=\"urn:not-labwc\"/>";
	QTest::newRow("bad-matrix") << QByteArray(kHeader) +
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\"><libinput>"
		"<device category=\"TSC2007 Touchscreen\"><calibrationMatrix>1 2 3</calibrationMatrix>"
		"</device></libinput></openbox_config>";
	QTest::newRow("duplicate-device") << QByteArray(kHeader) +
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\"><libinput>"
		"<device category=\"TSC2007 Touchscreen\"/><device category=\"TSC2007 Touchscreen\"/>"
		"</libinput></openbox_config>";
	QTest::newRow("duplicate-matrix") << QByteArray(kHeader) +
		"<openbox_config xmlns=\"http://openbox.org/3.4/rc\"><libinput>"
		"<device category=\"TSC2007 Touchscreen\"><calibrationMatrix>1 0 0 0 1 0</calibrationMatrix>"
		"<calibrationMatrix>1 0 0 0 1 0</calibrationMatrix></device></libinput></openbox_config>";
}

void TouchCalibrationTest::rejectsInvalidOrAmbiguousConfiguration()
{
	QFETCH(QByteArray, document);
	QTemporaryDir directory;
	QVERIFY(directory.isValid());
	const QString path = directory.filePath(QStringLiteral("rc.xml"));
	QVERIFY(writeFile(path, document));
	LabwcCalibrationStore store(path);
	CalibrationMatrix current;
	QString error;
	QVERIFY(!store.load(&current, &error));
	QVERIFY(!error.isEmpty());
	QCOMPARE(readFile(path), document);
	QVERIFY(!QFile::exists(store.backupPath()));
}

QTEST_APPLESS_MAIN(TouchCalibrationTest)
#include "TouchCalibrationTest.moc"
