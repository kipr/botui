#include "TouchCalibration.h"

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QLocale>
#include <QSaveFile>
#include <QStringList>

#include <algorithm>
#include <cmath>

namespace {
const QString kLabwcNamespace = QStringLiteral("http://openbox.org/3.4/rc");
const QString kDeviceName = QStringLiteral("TSC2007 Touchscreen");

bool solveThreeByThree(const double input[3][3], const double rightHandSide[3],
	                   double solution[3])
{
	double augmented[3][4];
	for (int row = 0; row < 3; ++row) {
		for (int column = 0; column < 3; ++column)
			augmented[row][column] = input[row][column];
		augmented[row][3] = rightHandSide[row];
	}

	for (int pivotColumn = 0; pivotColumn < 3; ++pivotColumn) {
		int pivotRow = pivotColumn;
		for (int row = pivotColumn + 1; row < 3; ++row) {
			if (std::abs(augmented[row][pivotColumn]) >
			    std::abs(augmented[pivotRow][pivotColumn])) {
				pivotRow = row;
			}
		}
		if (std::abs(augmented[pivotRow][pivotColumn]) < 1.0e-10)
			return false;
		if (pivotRow != pivotColumn) {
			for (int column = pivotColumn; column < 4; ++column)
				std::swap(augmented[pivotColumn][column], augmented[pivotRow][column]);
		}

		const double pivot = augmented[pivotColumn][pivotColumn];
		for (int column = pivotColumn; column < 4; ++column)
			augmented[pivotColumn][column] /= pivot;

		for (int row = 0; row < 3; ++row) {
			if (row == pivotColumn)
				continue;
			const double factor = augmented[row][pivotColumn];
			for (int column = pivotColumn; column < 4; ++column)
				augmented[row][column] -= factor * augmented[pivotColumn][column];
		}
	}

	for (int row = 0; row < 3; ++row)
		solution[row] = augmented[row][3];
	return true;
}

bool isNamedElement(const QDomElement &element, const QString &localName)
{
	return !element.isNull() && element.namespaceURI() == kLabwcNamespace &&
	       element.localName() == localName;
}

QVector<QDomElement> directChildren(const QDomElement &parent, const QString &localName)
{
	QVector<QDomElement> result;
	for (QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
		const QDomElement element = node.toElement();
		if (isNamedElement(element, localName))
			result.append(element);
	}
	return result;
}

bool parseMatrix(const QString &text, CalibrationMatrix *matrix)
{
	if (!matrix)
		return false;
	const QStringList parts = text.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
	if (parts.size() != 6)
		return false;
	for (int index = 0; index < 6; ++index) {
		bool ok = false;
		matrix->values[index] = QLocale::c().toDouble(parts.at(index), &ok);
		if (!ok || !std::isfinite(matrix->values[index]))
			return false;
	}
	return true;
}

QString matrixText(const CalibrationMatrix &matrix)
{
	QStringList values;
	for (double value : matrix.values)
		values.append(QLocale::c().toString(value, 'f', 6));
	return values.join(QLatin1Char(' '));
}

bool parseDocument(const QByteArray &content, QDomDocument *document, QString *errorMessage)
{
	if (!document)
		return false;
	const QDomDocument::ParseResult result = document->setContent(
		content, QDomDocument::ParseOption::UseNamespaceProcessing);
	if (!result) {
		if (errorMessage) {
			*errorMessage = QStringLiteral("Unable to parse Labwc configuration at line %1, column %2: %3")
				.arg(result.errorLine).arg(result.errorColumn).arg(result.errorMessage);
		}
		return false;
	}
	const QDomElement root = document->documentElement();
	if (!isNamedElement(root, QStringLiteral("openbox_config"))) {
		if (errorMessage) {
			*errorMessage = QStringLiteral("Labwc configuration must have an openbox_config root in namespace %1")
				.arg(kLabwcNamespace);
		}
		return false;
	}
	return true;
}

bool findCalibrationElement(QDomDocument *document, QDomElement *device,
	                        QDomElement *matrixElement, QString *errorMessage)
{
	const QDomElement root = document->documentElement();
	const QVector<QDomElement> libinputElements = directChildren(root, QStringLiteral("libinput"));
	QVector<QDomElement> matchingDevices;
	for (const QDomElement &libinput : libinputElements) {
		const QVector<QDomElement> devices = directChildren(libinput, QStringLiteral("device"));
		for (const QDomElement &candidate : devices) {
			if (candidate.attribute(QStringLiteral("category")) == kDeviceName)
				matchingDevices.append(candidate);
		}
	}
	if (matchingDevices.size() > 1) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Labwc configuration contains multiple TSC2007 Touchscreen profiles");
		return false;
	}

	if (matchingDevices.isEmpty()) {
		QDomElement libinput;
		if (libinputElements.isEmpty()) {
			libinput = document->createElementNS(kLabwcNamespace, QStringLiteral("libinput"));
			document->documentElement().appendChild(libinput);
		} else {
			libinput = libinputElements.constFirst();
		}
		*device = document->createElementNS(kLabwcNamespace, QStringLiteral("device"));
		device->setAttribute(QStringLiteral("category"), kDeviceName);
		libinput.appendChild(*device);
	} else {
		*device = matchingDevices.constFirst();
	}

	const QVector<QDomElement> matrices = directChildren(*device, QStringLiteral("calibrationMatrix"));
	if (matrices.size() > 1) {
		if (errorMessage)
			*errorMessage = QStringLiteral("TSC2007 Touchscreen profile contains multiple calibrationMatrix elements");
		return false;
	}
	if (matrices.isEmpty()) {
		*matrixElement = document->createElementNS(kLabwcNamespace,
		                                                   QStringLiteral("calibrationMatrix"));
		device->appendChild(*matrixElement);
	} else {
		*matrixElement = matrices.constFirst();
	}
	return true;
}
}

CalibrationMatrix CalibrationMatrix::identity()
{
	CalibrationMatrix matrix = {{1.0, 0.0, 0.0, 0.0, 1.0, 0.0}};
	return matrix;
}

QPointF CalibrationMatrix::map(const QPointF &point) const
{
	return QPointF(values[0] * point.x() + values[1] * point.y() + values[2],
	               values[3] * point.x() + values[4] * point.y() + values[5]);
}

bool CalibrationMatrix::isFinite() const
{
	for (double value : values) {
		if (!std::isfinite(value))
			return false;
	}
	return true;
}

bool TouchCalibration::fitCorrection(const QVector<QPointF> &observed,
	                                  const QVector<QPointF> &expected,
	                                  CalibrationMatrix *correction,
	                                  double *rootMeanSquareError)
{
	if (!correction || observed.size() != expected.size() || observed.size() < 3)
		return false;

	double normal[3][3] = {};
	double rightX[3] = {};
	double rightY[3] = {};
	for (int index = 0; index < observed.size(); ++index) {
		const double row[3] = {observed.at(index).x(), observed.at(index).y(), 1.0};
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j)
				normal[i][j] += row[i] * row[j];
			rightX[i] += row[i] * expected.at(index).x();
			rightY[i] += row[i] * expected.at(index).y();
		}
	}

	double horizontal[3];
	double vertical[3];
	if (!solveThreeByThree(normal, rightX, horizontal) ||
	    !solveThreeByThree(normal, rightY, vertical)) {
		return false;
	}
	for (int index = 0; index < 3; ++index) {
		correction->values[index] = horizontal[index];
		correction->values[index + 3] = vertical[index];
	}
	if (!correction->isFinite())
		return false;

	double squaredError = 0.0;
	for (int index = 0; index < observed.size(); ++index) {
		const QPointF difference = correction->map(observed.at(index)) - expected.at(index);
		squaredError += QPointF::dotProduct(difference, difference);
	}
	if (rootMeanSquareError)
		*rootMeanSquareError = std::sqrt(squaredError / observed.size());
	return true;
}

CalibrationMatrix TouchCalibration::compose(const CalibrationMatrix &left,
	                                         const CalibrationMatrix &right)
{
	CalibrationMatrix result;
	result.values[0] = left.values[0] * right.values[0] + left.values[1] * right.values[3];
	result.values[1] = left.values[0] * right.values[1] + left.values[1] * right.values[4];
	result.values[2] = left.values[0] * right.values[2] + left.values[1] * right.values[5] + left.values[2];
	result.values[3] = left.values[3] * right.values[0] + left.values[4] * right.values[3];
	result.values[4] = left.values[3] * right.values[1] + left.values[4] * right.values[4];
	result.values[5] = left.values[3] * right.values[2] + left.values[4] * right.values[5] + left.values[5];
	return result;
}

bool TouchCalibration::isSafeCandidate(const CalibrationMatrix &matrix)
{
	if (!matrix.isFinite())
		return false;
	const double determinant = matrix.values[0] * matrix.values[4] -
	                           matrix.values[1] * matrix.values[3];
	if (std::abs(determinant) < 0.01)
		return false;

	const QPointF corners[] = {
		QPointF(0.0, 0.0), QPointF(1.0, 0.0),
		QPointF(0.0, 1.0), QPointF(1.0, 1.0)
	};
	for (const QPointF &corner : corners) {
		const QPointF mapped = matrix.map(corner);
		if (mapped.x() < -0.25 || mapped.x() > 1.25 ||
		    mapped.y() < -0.25 || mapped.y() > 1.25) {
			return false;
		}
	}
	return true;
}

LabwcCalibrationStore::LabwcCalibrationStore(const QString &configPath)
	: m_configPath(configPath),
	  m_originalPermissions(),
	  m_loaded(false),
	  m_candidateInstalled(false)
{
}

QString LabwcCalibrationStore::defaultConfigPath()
{
	return QDir(QDir::homePath()).filePath(QStringLiteral(".config/labwc/rc.xml"));
}

QString LabwcCalibrationStore::configPath() const
{
	return m_configPath;
}

QString LabwcCalibrationStore::backupPath() const
{
	return m_configPath + QStringLiteral(".bak");
}

bool LabwcCalibrationStore::load(CalibrationMatrix *currentMatrix, QString *errorMessage)
{
	m_loaded = false;
	m_candidateInstalled = false;
	QFile file(m_configPath);
	if (!file.open(QIODevice::ReadOnly)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to open %1: %2").arg(m_configPath, file.errorString());
		return false;
	}
	m_originalContent = file.readAll();
	m_originalPermissions = file.permissions();
	file.close();

	QDomDocument document;
	if (!parseDocument(m_originalContent, &document, errorMessage))
		return false;
	QDomElement device;
	QDomElement matrixElement;
	if (!findCalibrationElement(&document, &device, &matrixElement, errorMessage))
		return false;

	if (matrixElement.text().trimmed().isEmpty()) {
		*currentMatrix = CalibrationMatrix::identity();
	} else if (!parseMatrix(matrixElement.text(), currentMatrix)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("TSC2007 Touchscreen calibrationMatrix must contain six finite numbers");
		return false;
	}
	m_loaded = true;
	return true;
}

bool LabwcCalibrationStore::installCandidate(const CalibrationMatrix &candidate,
	                                         QString *errorMessage)
{
	if (!m_loaded) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Labwc configuration has not been loaded");
		return false;
	}
	if (!TouchCalibration::isSafeCandidate(candidate)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Refusing to install an unsafe calibration matrix");
		return false;
	}

	QFile currentFile(m_configPath);
	if (!currentFile.open(QIODevice::ReadOnly)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to re-read %1: %2").arg(m_configPath, currentFile.errorString());
		return false;
	}
	const QByteArray currentContent = currentFile.readAll();
	currentFile.close();
	if (currentContent != m_originalContent) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Labwc configuration changed while calibration was running");
		return false;
	}

	QDomDocument document;
	if (!parseDocument(m_originalContent, &document, errorMessage))
		return false;
	QDomElement device;
	QDomElement matrixElement;
	if (!findCalibrationElement(&document, &device, &matrixElement, errorMessage))
		return false;
	while (!matrixElement.firstChild().isNull())
		matrixElement.removeChild(matrixElement.firstChild());
	matrixElement.appendChild(document.createTextNode(matrixText(candidate)));

	if (!writeAtomically(backupPath(), m_originalContent, m_originalPermissions, errorMessage))
		return false;
	// From this point on the caller must use the backup recovery path if any
	// subsequent step fails. QSaveFile normally leaves the old rc.xml intact on
	// failure, but treating it as needing restoration also covers uncertain I/O
	// failures and guarantees the requested final Labwc reload.
	m_candidateInstalled = true;
	if (!writeAtomically(m_configPath, document.toByteArray(2), m_originalPermissions, errorMessage))
		return false;
	return true;
}

bool LabwcCalibrationStore::restoreBackup(QString *errorMessage)
{
	QFile backup(backupPath());
	if (!backup.open(QIODevice::ReadOnly)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to open calibration backup %1: %2")
				.arg(backupPath(), backup.errorString());
		return false;
	}
	const QByteArray content = backup.readAll();
	backup.close();

	QDomDocument document;
	if (!parseDocument(content, &document, errorMessage))
		return false;
	if (!writeAtomically(m_configPath, content, m_originalPermissions, errorMessage))
		return false;
	m_candidateInstalled = false;
	return true;
}

bool LabwcCalibrationStore::candidateInstalled() const
{
	return m_candidateInstalled;
}

bool LabwcCalibrationStore::writeAtomically(const QString &path, const QByteArray &content,
	                                        QFileDevice::Permissions permissions,
	                                        QString *errorMessage) const
{
	QSaveFile file(path);
	if (!file.open(QIODevice::WriteOnly)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to write %1: %2").arg(path, file.errorString());
		return false;
	}
	if (!file.setPermissions(permissions)) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to preserve permissions for %1: %2")
				.arg(path, file.errorString());
		file.cancelWriting();
		return false;
	}
	if (file.write(content) != content.size()) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to write all data to %1: %2").arg(path, file.errorString());
		file.cancelWriting();
		return false;
	}
	if (!file.commit()) {
		if (errorMessage)
			*errorMessage = QStringLiteral("Unable to commit %1: %2").arg(path, file.errorString());
		return false;
	}
	return true;
}
