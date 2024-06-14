#include "HomeWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QMovie>
#include <QSize>
#include <QProcess>
#include <QVBoxLayout>
#include <QGridLayout>

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

	// QAction *lock = menuBar()->addAction(UiStandards::lockString());
	//  connect(lock, SIGNAL(triggered()), SLOT(lock()));
	QAction *about = menuBar()->addAction(tr("About"));
	QAction *shutDown = menuBar()->addAction(tr("Shut Down"));
	QAction *reboot = menuBar()->addAction(tr("Reboot"));
	menuBar()->adjustSize();
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
	if (QMessageBox::question(this, "Shut Down?", "After system halted, slide power switch off or disconnect battery.\n\nContinue?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		return;

	const int ret = QProcess::execute("poweroff");
	if (ret < 0)
		QMessageBox::information(this, "Failed", "Shut down failed.");
#else
	QMessageBox::information(this, "Not Available", "Shut down is only available on the kovan.");
#endif
}

void HomeWidget::reboot()
{
#ifdef WOMBAT
	if (QMessageBox::question(this, "Reboot?", "Please wait up to 10 seconds for the system to begin rebooting.\n\nContinue?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		return;

	QMessageBox msgBox(this);
	msgBox.setWindowTitle("Reboot");
	msgBox.setText("Rebooting now...");

	// msgBox.setText("Rebooting...");
	msgBox.setMaximumSize(500, 480);
	// msgBox.setStyleSheet("QLabel{min-width: 450px; min-height: 280px;}");
	msgBox.setStandardButtons(QMessageBox::NoButton);

	QLabel *gifLabel = new QLabel();
	QLabel *messageLabel = new QLabel(msgBox.text());

	QGridLayout *msgBoxLayout = qobject_cast<QGridLayout *>(msgBox.layout());

	msgBoxLayout->setVerticalSpacing(0);

	QWidget *container = new QWidget();
	QVBoxLayout *vLayout = new QVBoxLayout(container);

	vLayout->addWidget(gifLabel);

	vLayout->addWidget(messageLabel);
	vLayout->setAlignment(Qt::AlignCenter);
	gifLabel->setAlignment(Qt::AlignCenter);
	messageLabel->setAlignment(Qt::AlignCenter);

	msgBoxLayout->setSpacing(0);
	vLayout->setSpacing(10);

	container->setLayout(vLayout);

	if (msgBoxLayout)
	{
		msgBoxLayout->addWidget(container, 0, 0, 1, msgBoxLayout->columnCount());
	}

	gifLabel->move(200, -50);
	gifLabel->resize(400, 1100);

	QMovie *movie = new QMovie("://qml/botguy_noMargin.gif");
	movie->setScaledSize(QSize(200, 240));
	gifLabel->setMovie(movie);
	movie->start();
	gifLabel->show();

	msgBox.setText("");
	msgBox.exec();
	 // Stop create3_server.service
    QProcess create3ServerStop;
    create3ServerStop.start("sudo", QStringList() << "systemctl" << "stop" << "create3_server.service");
    bool create3StopRet = create3ServerStop.waitForFinished();
    if (!create3StopRet || create3ServerStop.exitCode() != 0)
    {
        QMessageBox::information(this, "Failed", "Create 3 server could not be stopped.");
        return;
    }

    // Reboot the system
    QProcess rebootProcess;
    rebootProcess.start("sudo", QStringList() << "reboot");
    bool rebootRet = rebootProcess.waitForFinished();
    if (!rebootRet || rebootProcess.exitCode() != 0)
    {
        QMessageBox::information(this, "Failed", "Reboot failed.");
    }
#else
    QMessageBox::information(this, "Not Available", "Reboot is only available on the kovan.");
#endif
}

void HomeWidget::lock()
{
	LockScreen::lock();
}
