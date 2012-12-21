#include "NetworkTestWizardPage.h"
#include "ui_NetworkTestWizardPage.h"

#include "NetworkManager.h"
#include "NetworkItemModel.h"

NetworkTestWizardPage::NetworkTestWizardPage(QWidget *parent)
	: QWizardPage(parent),
	ui(new Ui::NetworkTestWizardPage),
	m_model(new NetworkItemModel(this))
{
	ui->setupUi(this);
	ui->networks->setModel(m_model);
}

NetworkTestWizardPage::~NetworkTestWizardPage()
{
	delete ui;
}

void NetworkTestWizardPage::initializePage()
{
	m_model->setNetworks(NetworkManager::ref().networks());
}

void NetworkTestWizardPage::cleanupPage()
{
	
}