#include "StringNumberEditWidget.h"
#include "ui_StringNumberEditWidget.h"
#include "MenuBar.h"
#include "RootController.h"
#include "StatusBar.h"
#include "Device.h"
#include "QNumpadDialog.h"
#include "KeyboardDialog.h"

#include <QDebug>

StringNumberEditWidget::StringNumberEditWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	ui(new Ui::StringNumberEditWidget),
	m_device(device),
	m_menuBar(new MenuBar(this)),
	m_statusBar(new StatusBar(this)),
	m_numpad(new QNumpadDialog(tr("Number Edit"))),
	m_keyboard(new KeyboardDialog(tr("String Edit")))
{
	ui->setupUi(this);
	m_menuBar->addHomeAndBackButtons();
	m_menuBar->setTitle("String/Number Edit");
	layout()->setMenuBar(m_menuBar);
	m_statusBar->loadDefaultWidgets(m_device);
	layout()->addWidget(m_statusBar);
	ui->number->setInputProvider(m_numpad);
	ui->string->setInputProvider(m_keyboard);
}

StringNumberEditWidget::~StringNumberEditWidget()
{
	delete m_numpad;
	delete ui;
	delete m_menuBar;
	delete m_statusBar;
}
