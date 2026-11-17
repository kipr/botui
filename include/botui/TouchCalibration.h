#ifndef TOUCHCALIBRATION_H
#define TOUCHCALIBRATION_H

#include <QByteArray>
#include <QFileDevice>
#include <QPointF>
#include <QString>
#include <QVector>

struct CalibrationMatrix
{
	double values[6];

	static CalibrationMatrix identity();
	QPointF map(const QPointF &point) const;
	bool isFinite() const;
};

namespace TouchCalibration
{
	bool fitCorrection(const QVector<QPointF> &observed,
	                   const QVector<QPointF> &expected,
	                   CalibrationMatrix *correction,
	                   double *rootMeanSquareError = nullptr);
	CalibrationMatrix compose(const CalibrationMatrix &left,
	                          const CalibrationMatrix &right);
	bool isSafeCandidate(const CalibrationMatrix &matrix);
}

class LabwcCalibrationStore
{
public:
	explicit LabwcCalibrationStore(const QString &configPath = defaultConfigPath());

	static QString defaultConfigPath();
	QString configPath() const;
	QString backupPath() const;

	bool load(CalibrationMatrix *currentMatrix, QString *errorMessage);
	bool installCandidate(const CalibrationMatrix &candidate, QString *errorMessage);
	bool restoreBackup(QString *errorMessage);
	bool candidateInstalled() const;

private:
	bool writeAtomically(const QString &path, const QByteArray &content,
	                     QFileDevice::Permissions permissions,
	                     QString *errorMessage) const;

	QString m_configPath;
	QByteArray m_originalContent;
	QFileDevice::Permissions m_originalPermissions;
	bool m_loaded;
	bool m_candidateInstalled;
};

#endif // TOUCHCALIBRATION_H
