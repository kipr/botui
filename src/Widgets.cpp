#include "Widgets.h"
#include "ui_Widgets.h"
#include "MenuBar.h"

Widgets::Widgets(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::Widgets),
	m_menuBar(new MenuBar(this))
{
	ui->setupUi(this);
	m_menuBar->addMenu("Back");
	m_menuBar->addMenu("Reload");
	m_menuBar->setTitle("Assorted Widgets");
	layout()->setMenuBar(m_menuBar);
}

Widgets::~Widgets()
{
	delete ui;
	delete m_menuBar;
}