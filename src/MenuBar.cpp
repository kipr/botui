#include "MenuBar.h"
#include "RootController.h"

#include "UiStandards.h"

#include <QLabel>

MenuBar::MenuBar(QWidget *parent)
	: QMenuBar(parent)
{
	setNativeMenuBar(false);
	m_title = new QLabel("", this);
	m_title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_title->setMinimumSize(50, 21);
	QPalette palette = m_title->palette();
	palette.setColor(m_title->foregroundRole(), Qt::white);
	m_title->setPalette(palette);
	setCornerWidget(m_title);
	
	setTitle("Untitled");
}

MenuBar::~MenuBar()
{
	delete m_title;
}

void MenuBar::setTitle(const QString& title, const QPixmap& icon)
{
	m_title->setText(title + " ");
	if(!icon.isNull()) m_title->setPixmap(icon);
}

QString MenuBar::title() const
{
	return m_title->text();
}

void MenuBar::addHomeAndBackButtons()
{
	QAction *homeAction = addAction(UiStandards::homeString());
	connect(homeAction, SIGNAL(activated()), RootController::ptr(), SLOT(dismissAllWidgets()));

	if(RootController::ref().depth() > 1) {
		QAction *backAction = addAction(UiStandards::backString());
		connect(backAction, SIGNAL(activated()),
			RootController::ptr(), SLOT(dismissWidget()));
	}
}