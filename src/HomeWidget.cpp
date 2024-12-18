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
QLabel *backgroundImageLabel;
HomeWidget::HomeWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    performStandardSetup(UiStandards::homeString());

    backgroundImageLabel = new QLabel(this);
    backgroundImageLabel->setGeometry(0, 0, 800, 480);
    backgroundImageLabel->setPixmap(QPixmap(":/qml/Event_Mode_Background.png"));
    backgroundImageLabel->setScaledContents(true);
    backgroundImageLabel->lower();
    backgroundImageLabel->hide();

    bool defaultEventState = getEventModeStateDefault();

    if (defaultEventState)
    {
        
        backgroundImageLabel->show();
    }
    qDebug() << "HOME default event state: " << defaultEventState;
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

void HomeWidget::handleEventMode()
{
    qDebug() << "Inside handleEvent Mode HOME";
    backgroundImageLabel->show();

}
void HomeWidget::updateEventModeLabel()
{

    backgroundImageLabel->hide();

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
    AboutWidget *aboutWidget = new AboutWidget(device());

    connect(aboutWidget, &AboutWidget::eventModeEnabled, this, &HomeWidget::handleEventMode);
    connect(aboutWidget, &AboutWidget::eventModeDisabled, this, &HomeWidget::updateEventModeLabel);

    RootController::ref().presentWidget(aboutWidget);
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
        qDebug() << "Rebooting the system...";
        // Reboot the system
       const int rebootRet = QProcess::execute("reboot");


        msgBox->close(); });

#else
    QMessageBox::information(this, "Not Available", "Reboot is only available on the kovan.");
#endif
}
void HomeWidget::lock()
{
    LockScreen::lock();
}
