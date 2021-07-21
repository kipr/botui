#include "PairWidget.h"
#include "ui_PairWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include <QDebug>

PairWidget::PairWidget(Device *device, QQuickWidget *parent)
	: QQuickWidget(parent),
	ui(new Ui::PairWidget),
	m_device(device),
	m_menuBar(new MenuBar(this))
{
	ui->setupUi(this);
	QAction *cancelAction = m_menuBar->addAction("Cancel");
	connect(cancelAction, SIGNAL(triggered()), SIGNAL(cancel()));
	m_menuBar->setTitle("Incoming Connection");
	layout()->setMenuBar(m_menuBar);
}

PairWidget::~PairWidget()
{
	delete ui;
	delete m_menuBar;
}

void PairWidget::setPassword(const QString& password)
{
	ui->code->setText(password);
}
