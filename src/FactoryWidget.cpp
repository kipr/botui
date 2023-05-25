#include "FactoryWidget.h"
#include "Calibrate.h"
#include "Device.h"
#include "MenuBar.h"
#include "NotYetImplementedDialog.h"
#include "NumpadDialog.h"
#include "Options.h"
#include "RootController.h"
#include "SettingsProvider.h"
#include "StatusBar.h"
#include "ui_FactoryWidget.h"

#include <QDebug>
#include <QProcess>
#include <QString>

FactoryWidget::FactoryWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::FactoryWidget),
      m_provider(new NumpadDialog(QString())) {
    // Standard Setup
    ui->setupUi(this);
    performStandardSetup(tr("Factory"));

    // Set the text boxes to use a numpad UI
    ui->serialOne->setInputProvider(m_provider);
    ui->serialTwo->setInputProvider(m_provider);
    ui->serialThree->setInputProvider(m_provider);
    ui->serialFour->setInputProvider(m_provider);

    // Make sure the console doesn't appear until we call it
    ui->console->setVisible(false);

    // Connect the functions to their respective buttons
    connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
    connect(ui->reflash, SIGNAL(clicked()), SLOT(reflash()));
    connect(ui->experimental, SIGNAL(clicked()), SLOT(experimental()));
}

FactoryWidget::~FactoryWidget() {
    delete m_provider;
    delete ui;
}

void FactoryWidget::confirm() {
    // Make console visable and print the requested serial
    ui->console->setVisible(true);
    ui->console->insertPlainText("Setting Serial Number to " +
                                 ui->serialOne->text() + ui->serialTwo->text() +
                                 ui->serialThree->text() +
                                 ui->serialFour->text());

    // Run QProcess
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);
    m_consoleProc->start("sh /home/pi/wallaby_set_serial.sh " +
                         ui->serialOne->text() + " " + ui->serialTwo->text() +
                         " " + ui->serialThree->text() + " " +
                         ui->serialFour->text());

    // Keep loading console until finished
    while (m_consoleProc->state() != QProcess::NotRunning) {
        QApplication::processEvents();
    }

    ui->console->insertPlainText("Serial Number Set to " +
                                 ui->serialOne->text() + ui->serialTwo->text() +
                                 ui->serialThree->text() +
                                 ui->serialFour->text() + "!");
    // ui->console->setProcess(0);
}

void FactoryWidget::reflash() {
    // Disable Buttons
    ui->reflash->setEnabled(false);
    ui->experimental->setEnabled(false);
    ui->confirm->setEnabled(false);

    // Make console appear
    ui->console->setVisible(true);

    // Make textboxes disappear
    ui->serialOne->setVisible(false);
    ui->serialTwo->setVisible(false);
    ui->serialThree->setVisible(false);
    ui->serialFour->setVisible(false);

    // Make buttons and labels disappear
    ui->confirm->setVisible(false);
    ui->reflash->setVisible(false);
    ui->experimental->setVisible(false);
    ui->changeSerialLabel->setText("Flash Progress:");
    ui->subText->setText("The console may not update until the end, wait for "
                         "'Reflash Complete'");

    // Setup QProcess
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);

    // Start QProcess
    ui->console->insertPlainText(
        "Starting Flash, please wait until finished... \n");
    m_consoleProc->setWorkingDirectory("/home/pi");
    m_consoleProc->start("sudo ./wallaby_flash");

    // Keep loading console until finished
    while (m_consoleProc->state() != QProcess::NotRunning) {
        QApplication::processEvents();
    }
    ui->console->insertPlainText("Reflash Complete!");
    // ui->console->setProcess(0);
}

void FactoryWidget::experimental() {

    // Make console appear
    ui->console->setVisible(true);

    // Make textboxes disappear
    ui->serialOne->setVisible(false);
    ui->serialTwo->setVisible(false);
    ui->serialThree->setVisible(false);
    ui->serialFour->setVisible(false);

    // Make buttons and labels disappear
    ui->confirm->setVisible(false);
    ui->reflash->setVisible(false);
    ui->experimental->setVisible(false);
    ui->changeSerialLabel->setText("Experimental Install:");

    // Setup QProcess
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);

    // Start QProcess
    ui->console->insertPlainText("Pulling latest beta software... \n");
    m_consoleProc->setWorkingDirectory("/home/pi");
    m_consoleProc->start("sudo ./getExperimental.sh");

    // Keep loading console until finished
    while (m_consoleProc->state() != QProcess::NotRunning) {
        QApplication::processEvents();
    }
    ui->console->insertPlainText("Experimental Build Complete!");
    // ui->console->setProcess(0);
}
