#include "KovanButtonProvider.h"

#include <kovan/button.hpp>
#include <kovan/general.h>

#include <QDebug>

Kovan::ButtonProvider::ButtonProvider(QObject *parent)
	: ::ButtonProvider(parent),
	m_shown(false)
{
}

Kovan::ButtonProvider::~ButtonProvider()
{
}

bool Kovan::ButtonProvider::isExtraShown() const
{
	return ExtraButtons::isShown();
}

QString Kovan::ButtonProvider::text(const ButtonProvider::ButtonId& id) const
{
	AbstractTextButton *button = lookup(id);
	return button ? QString::fromUtf8(button->text()) : QString();
}

bool Kovan::ButtonProvider::setPressed(const ButtonProvider::ButtonId& id, bool pressed) const
{
	qDebug() << "setPressed lookup";
	AbstractTextButton *button = lookup(id);
	if(!button) return false;
	qDebug() << "Setting" << id << "pressed =" << pressed;
	
	publish();
	button->setPressed(pressed);
	publish();
	return true;
}

template <typename T, size_t N>
inline size_t sizeOfArray(const T(&)[N])
{
	return N;
}

void Kovan::ButtonProvider::reset()
{
	const size_t len = sizeOfArray(Button::all);
	for(size_t i = 0; i < len; ++i) {
		Button::all[i]->resetText();
	}
	ExtraButtons::hide();
	publish();
	m_shown = false;
	emit extraShownChanged(false);
}

void Kovan::ButtonProvider::refresh()
{
	publish();
	if(Button::A.isTextDirty()) emit buttonTextChanged(A, text(A));
	if(Button::B.isTextDirty()) emit buttonTextChanged(B, text(B));
	if(Button::C.isTextDirty()) emit buttonTextChanged(C, text(C));
	if(Button::X.isTextDirty()) emit buttonTextChanged(X, text(X));
	if(Button::Y.isTextDirty()) emit buttonTextChanged(Y, text(Y));
	if(Button::Z.isTextDirty()) emit buttonTextChanged(Z, text(Z));
	if(m_shown != ExtraButtons::isShown()) {
		qDebug() << "Shown was dirty!";
		m_shown = ExtraButtons::isShown();
		emit extraShownChanged(m_shown);
	}
}

AbstractTextButton *Kovan::ButtonProvider::lookup(const ButtonProvider::ButtonId& id) const
{
	switch(id) {
		case ButtonProvider::A: return &Button::A;
		case ButtonProvider::B: return &Button::B;
		case ButtonProvider::C: return &Button::C;
		case ButtonProvider::X: return &Button::X;
		case ButtonProvider::Y: return &Button::Y;
		case ButtonProvider::Z: return &Button::Z;
	}
	return 0;
}
