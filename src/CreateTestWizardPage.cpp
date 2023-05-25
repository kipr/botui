#include "CreateTestWizardPage.h"

#include "ui_CreateTestWizardPage.h"

#include <kipr/create/create.hpp>
using namespace kipr::create;

#include <QTimer>

CreateTestWizardPage::CreateTestWizardPage(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::CreateTestWizardPage),
      m_connectTimer(new QTimer(this)) {
    ui->setupUi(this);
    connect(m_connectTimer, SIGNAL(timeout()), SLOT(createConnect()));
}

CreateTestWizardPage::~CreateTestWizardPage() { delete ui; }

void CreateTestWizardPage::initializePage() {
    m_connectTimer->start(1000);
    createConnect();
}

void CreateTestWizardPage::cleanupPage() {
    Create::instance()->disconnect();
    ui->status->setText(tr("Disconnected"));
}

void CreateTestWizardPage::createConnect() {
    if (!Create::instance()->connect()) return;
    m_connectTimer->stop();
    ui->turn->setEnabled(true);
    ui->status->setText(tr("Connected"));
}

void CreateTestWizardPage::turn() {
    Create::instance()->turn(50, 300);
    Create::instance()->turn(-50, 300);
}
