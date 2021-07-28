#include "FactoryWidget.h"
#include "ui_FactoryWidget.h"
#include "SettingsProvider.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "Calibrate.h"
#include "Options.h"
#include "NotYetImplementedDialog.h"
#include "NumpadDialog.h"

#include <QString>
#include <QProcess>
#include <QDebug>

FactoryWidget::FactoryWidget(Device *device, QWidget *parent)
        : StandardWidget(device, parent),
        ui(new Ui::FactoryWidget),
        m_provider(new NumpadDialog(QString()))
{

    ui->setupUi(this);
    performStandardSetup(tr("Factory"));

        //Set the text boxes to use a numpad UI
        ui->serialOne->setInputProvider(m_provider);
        ui->serialTwo->setInputProvider(m_provider);
        ui->serialThree->setInputProvider(m_provider);
        ui->serialFour->setInputProvider(m_provider);


        ui->console->setVisible(false);

        connect(ui->confirm, SIGNAL(clicked()), SLOT(confirm()));
        connect(ui->reflash, SIGNAL(clicked()), SLOT(reflash()));
        connect(ui->experimental, SIGNAL(clicked()), SLOT(experimental()));

}

FactoryWidget::~FactoryWidget()
{
        delete m_provider;
        delete ui;
}

void FactoryWidget::confirm()
{

    ui->console->setVisible(true);

    ui->console->insertPlainText("Setting Serial Number to " + ui->serialOne->text() + ui->serialTwo->text() + ui->serialThree->text() + ui->serialFour->text());

    // Run
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);
    connect(m_consoleProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));

    m_consoleProc->start("sh /home/pi/wallaby_set_serial.sh " + ui->serialOne->text() + " " + ui->serialTwo->text() + " " + ui->serialThree->text() + " " + ui->serialFour->text());

}

void FactoryWidget::reflash()
{
    ui->serialOne->setVisible(false);
    ui->serialTwo->setVisible(false);
    ui->serialThree->setVisible(false);
    ui->serialFour->setVisible(false);

    ui->confirm->setVisible(false);
    ui->reflash->setVisible(false);
    ui->experimental->setVisible(false);
    ui->changeSerialLabel->setVisible(false);

    ui->console->setVisible(true);


    // Run
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);

    ui->console->insertPlainText("Starting Flash, please wait until finished \n");
    m_consoleProc->setWorkingDirectory("/home/pi");
    m_consoleProc->start("sudo ./wallaby_flash");
    m_consoleProc->waitForFinished();
    ui->console->insertPlainText("Flash Complete");

}

void FactoryWidget::experimental()
{


    ui->serialOne->setVisible(false);
    ui->serialTwo->setVisible(false);
    ui->serialThree->setVisible(false);
    ui->serialFour->setVisible(false);

    ui->confirm->setVisible(false);
    ui->reflash->setVisible(false);
    ui->experimental->setVisible(false);
    ui->changeSerialLabel->setVisible(false);

    ui->console->setVisible(true);

    // Run
    m_consoleProc = new QProcess();
    m_consoleProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_consoleProc);

    ui->console->insertPlainText("Pulling latest beta software... \n");

    m_consoleProc->start("sudo ./home/pi/getExperimental.sh");
    m_consoleProc->waitForFinished();
    ui->console->insertPlainText("Experimental Build Installed");


}
