#include "HomeWidget.h"

#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QMovie>
#include <QSize>
#include <QProcess>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>

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

    // Create the QMessageBox
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Reboot");
    msgBox->setMaximumSize(500, 480); // Limit the size of the QMessageBox
    msgBox->setStandardButtons(QMessageBox::NoButton);

    // Create QLabel for the GIF
    QLabel *gifLabel = new QLabel();
    gifLabel->setAlignment(Qt::AlignCenter); // Center the GIF label

    // Create QLabel for the message text
    QLabel *messageLabel = new QLabel("Rebooting now...");
    messageLabel->setAlignment(Qt::AlignCenter); // Center the message label

    // Create a container widget and a new vertical layout
    QWidget *container = new QWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(container);

    // Add the GIF label and message label to the vertical layout
    vLayout->addWidget(gifLabel);
    vLayout->addWidget(messageLabel);

    // Adjust the vertical layout spacing and margins
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(10, 10, 10, 10);

    // Set the layout of the container
    container->setLayout(vLayout);

    // Access the internal layout of the QMessageBox
    QGridLayout *msgBoxLayout = qobject_cast<QGridLayout *>(msgBox->layout());
    if (msgBoxLayout)
    {
        msgBoxLayout->addWidget(container, 0, 0, 1, msgBoxLayout->columnCount());
    }

    // Setup and start the GIF movie
    QMovie *movie = new QMovie("://qml/botguy_noMargin.gif");
    movie->setScaledSize(QSize(200, 240));
    gifLabel->setMovie(movie);
    movie->start();

    // Show the QMessageBox non-blocking
    msgBox->setText(""); // Hide the default text to avoid duplication
    msgBox->show();

    // Debug information
    qDebug() << "Message box displayed, starting reboot sequence...";

    // Use a QTimer to delay the reboot process
    QTimer::singleShot(2000, this, [this, msgBox]()
                       {
        qDebug() << "Stopping create3_server.service...";
        // Stop create3_server.service
        QProcess create3ServerStop;
        create3ServerStop.start("sudo", QStringList() << "systemctl" << "stop" << "create3_server.service");
        bool create3StopRet = create3ServerStop.waitForFinished();
        qDebug() << "Create 3 server stop return value: " << create3StopRet;
        qDebug() << "Create 3 server exit code: " << create3ServerStop.exitCode();
       if(create3StopRet == false){
        qDebug() << "create3StopRet: " << create3StopRet;
        QMessageBox::information(this, "Failed", "Create 3 server could not be stopped.");

       }
			

        qDebug() << "Rebooting the system...";
        // Reboot the system
       const int rebootRet = QProcess::execute("reboot");
		if(create3StopRet == false || rebootRet < 0)
			QMessageBox::information(this, "Failed", "Reboot failed.");

        msgBox->close(); });

#else
    QMessageBox::information(this, "Not Available", "Reboot is only available on the kovan.");
#endif
}
void HomeWidget::lock()
{
    LockScreen::lock();
}
