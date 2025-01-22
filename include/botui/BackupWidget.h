#ifndef _BACKUPWIDGET_H_
#define _BACKUPWIDGET_H_

#include "Device.h"
#include "StandardWidget.h"
#include <QProcess>
#include <QDir>

namespace Ui
{
	class BackupWidget;
}

class BackupWidget : public StandardWidget
{
	Q_OBJECT
public:
	BackupWidget(Device *device, QWidget *widget = 0);
	~BackupWidget();

public slots:
	void backupoption();
	// void cleardrive();
	void restore();

private slots:
	void updateFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void restoreFinished(int, QProcess::ExitStatus exitStatus);

private:
	bool isAlreadyMounted(const QString &device, const QString &mountPoint);
	bool mountUsb(const QString device, const QDir dir);
	bool unmountUsb(const QString device);
	static const QDir mountDir;
	Ui::BackupWidget *ui;
	QProcess *backup_process;
	QProcess *restore_process;
	void handleStandardOutput();
	void handleStandardError();
};

#endif
