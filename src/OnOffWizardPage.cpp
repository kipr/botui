#include "OnOffWizardPage.h"
#include "ui_OnOffWizardPage.h"

#include "ArrayHelpers.h"

OnOffWizardPage::OnOffWizardPage(QWidget *parent)
	: QWizardPage(parent),
	ui(new Ui::OnOffWizardPage)
{
	ui->setupUi(this);
	
	m_off[0] = ui->off0;
	m_off[1] = ui->off1;
	m_off[2] = ui->off2;
	m_off[3] = ui->off3;
	
	m_on[0] = ui->on0;
	m_on[1] = ui->on1;
	m_on[2] = ui->on2;
	m_on[3] = ui->on3;
}

OnOffWizardPage::~OnOffWizardPage()
{
	delete ui;
}

void OnOffWizardPage::on_allOff_clicked()
{
	for(quint16 i = 0; i < sizeof_array(m_off); ++i) m_off[i]->click();
}

void OnOffWizardPage::on_allOn_clicked()
{
	for(quint16 i = 0; i < sizeof_array(m_on); ++i) m_on[i]->click();
}

void OnOffWizardPage::clicked()
{
	QPushButton *from = qobject_cast<QPushButton *>(sender());
	if(!from) return;
	
	quint16 i = 0;
	bool isOn = false;
	// This assumes on and off are the same size
	for(; i < sizeof_array(m_on); ++i) {
		if(m_on[i] == from) {
			isOn = true;
			break;
		} else if(m_off[i] == from) break;
	}
	m_off[i]->setEnabled(isOn);
	m_on[i]->setEnabled(!isOn);
	
	if(isOn) emit on(i);
	else emit off(i);
}