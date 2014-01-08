#include "MenuBar.h"
#include "RootController.h"

#include "UiStandards.h"
#include "Program.h"

#include <QLabel>
#include "StopButtonWidget.h"

MenuBar::MenuBar(Program *program, QWidget *parent)
	: QMenuBar(parent),
	m_title(new QLabel("", 0)),
	m_program(program),
	m_stopButton(new StopButtonWidget(m_program))
{
	init();
}

MenuBar::MenuBar(QWidget *parent)
	: QMenuBar(parent),
	m_title(new QLabel("", 0)),
	m_program(Program::instance()),
	m_stopButton(new StopButtonWidget(m_program))
{
	init();
}

MenuBar::~MenuBar()
{
	delete m_title;
}

void MenuBar::setTitle(const QString& title, const QPixmap& icon)
{
	m_title->setText(title + "  ");
	if(!icon.isNull()) m_title->setPixmap(icon);
}

QString MenuBar::title() const
{
	return m_title->text();
}

void MenuBar::addHomeAndBackButtons()
{
	clear();
	if(RootController::ref().depth() > 0) {
		QAction *homeAction = addAction(UiStandards::homeString());
		connect(homeAction, SIGNAL(triggered()), RootController::ptr(), SLOT(dismissAllWidgets()));
	}

	if(RootController::ref().depth() > 1) {
		QAction *backAction = addAction(UiStandards::backString());
		connect(backAction, SIGNAL(triggered()),
			RootController::ptr(), SLOT(dismissWidget()));
	}
}

void MenuBar::init()
{
	setNativeMenuBar(false);
	m_title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_title->setMinimumSize(50, 21);
	QPalette palette = m_title->palette();
	palette.setColor(m_title->foregroundRole(), Qt::white);
	m_title->setPalette(palette);
	//setCornerWidget(m_title);
	m_stopButton->setMinimumSize(10, 22);
	m_stopButton->resize(50, 22);
	m_stopButton->setMaximumSize(100, 22);
	
	m_program->connect(m_stopButton, SIGNAL(clicked()), SLOT(stop()));
	setCornerWidget(m_stopButton);
	
	setTitle("Untitled");
}