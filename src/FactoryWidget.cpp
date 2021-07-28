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

    QProcess backup_process;
    ui->console->setVisible(true);
    ui->console->insertPlainText("Setting Serial Number to " + ui->serialOne->text() + ui->serialTwo->text() + ui->serialThree->text() + ui->serialFour->text());

    // Run
    m_updateProc = new QProcess();
    m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_updateProc);
    connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
    m_updateProc->start("sh /home/pi/wallaby_set_serial.sh " + ui->serialOne->text() + " " + ui->serialTwo->text() + " " + ui->serialThree->text() + " " + ui->serialFour->text());

}

void FactoryWidget::reflash()
{

    QProcess backup_process;
    ui->console->setVisible(true);

    // Run
    m_updateProc = new QProcess();
    m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_updateProc);
    connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
    m_updateProc->start("sudo ./home/pi/wallaby_flash.sh");
    ui->console->insertPlainText("Flash Complete");

}

void FactoryWidget::experimental()
{

    QProcess backup_process;
    ui->console->setVisible(true);

    // Run
    m_updateProc = new QProcess();
    m_updateProc->setProcessChannelMode(QProcess::MergedChannels);
    ui->console->setProcess(m_updateProc);
    connect(m_updateProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(updateFinished(int, QProcess::ExitStatus)));
    m_updateProc->start("sudo ./home/pi/getExperimental.sh");
    ui->console->insertPlainText("Experimental Build Installed");


}
