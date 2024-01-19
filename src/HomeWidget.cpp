#include "HomeWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>

#include "ui_HomeWidget.h"
#include "MenuBar.h"
#include "StatusBar.h"
#include "RootController.h"
#include "SettingsWidget.h"
#include "AboutWidget.h"
#include "UnderConstructionWidget.h"
#include "MotorsSensorsWidget.h"
#include "ProgramsWidget.h"
#include "FileManagerWidget.h"
#include "Device.h"
#include "ProgramSelectionWidget.h"
#include "Program.h"
#include "UiStandards.h"
#include "LockScreen.h"

HomeWidget::HomeWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::HomeWidget)
{
	ui->setupUi(this);
	performStandardSetup(UiStandards::homeString());
	
	connect(ui->programs, SIGNAL(clicked()), SLOT(programs()));
	connect(ui->fileManager, SIGNAL(clicked()), SLOT(fileManager()));
	connect(ui->motorsSensors, SIGNAL(clicked()), SLOT(motorsSensors()));
	connect(ui->settings, SIGNAL(clicked()), SLOT(settings()));

	// TODO: fix fileManager and then remove this line
	ui->fileManager->setVisible(true);

	//QAction *lock = menuBar()->addAction(UiStandards::lockString());
	// connect(lock, SIGNAL(triggered()), SLOT(lock()));
	QAction *about = menuBar()->addAction(tr("About"));
	QAction *shutDown = menuBar()->addAction(tr("Shut Down"));
	QAction *reboot = menuBar()->addAction(tr("Reboot"));
	menuBar()->adjustSize() ;
	connect(about, SIGNAL(triggered()), SLOT(about()));
	connect(shutDown, SIGNAL(triggered()), SLOT(shutDown()));
	connect(reboot, SIGNAL(triggered()), SLOT(reboot()));
}

HomeWidget::~HomeWidget()
{
	delete ui;
}

void HomeWidget::programs()
{
	RootController::ref().presentWidget(Program::instance()->isRunning()
		? (QWidget *)new ProgramSelectionWidget(device())
		: (QWidget *)new ProgramsWidget(device()));
}

void HomeWidget::fileManager()
{
	RootController::ref().presentWidget(new FileManagerWidget(device()));
}

void HomeWidget::motorsSensors()
{
	RootController::ref().presentWidget(new MotorsSensorsWidget(device()));
}

void HomeWidget::settings()
{
	RootController::ref().presentWidget(new SettingsWidget(device()));
}

void HomeWidget::about()
{
	RootController::ref().presentWidget(new AboutWidget(device()));
	
}

void HomeWidget::shutDown()
{
#ifdef WOMBAT
  if(QMessageBox::question(this, "Shut Down?", "After system halted, slide power switch off or disconnect battery.\n\nContinue?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    return;
  
  const int ret = QProcess::execute("poweroff");
  if(ret < 0)
    QMessageBox::information(this, "Failed", "Shut down failed.");
#else
  QMessageBox::information(this, "Not Available", "Shut down is only available on the kovan.");
#endif
}

void HomeWidget::reboot()
{
	#ifdef WOMBAT
		if(QMessageBox::question(this, "Reboot?", "Please wait up to 10 seconds for the system to begin rebooting.\n\nContinue?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
			return;

		QProcess create3ServerStop;
		create3ServerStop.start("sudo", QStringList() << "systemctl" << "stop" << "create3_server.service");
		bool create3StopRet = create3ServerStop.waitForFinished();
		if(create3StopRet == false)
			QMessageBox::information(this, "Failed", "Create 3 server could not be stopped.");
		const int rebootRet = QProcess::execute("reboot");
		if(create3StopRet == false || rebootRet < 0)
			QMessageBox::information(this, "Failed", "Reboot failed.");
	#else
		QMessageBox::information(this, "Not Available", "Reboot is only available on the kovan.");
	#endif
}

void HomeWidget::lock()
{
	LockScreen::lock();
}
