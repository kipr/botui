#include "KovanButtonProvider.h"
#include <kovan/button.hpp>

Kovan::ButtonProvider::ButtonProvider(QObject *parent)
	: ::ButtonProvider(parent),
	m_a(new AButton()),
	m_b(new BButton()),
	m_z(new ZButton())
{
}

Kovan::ButtonProvider::~ButtonProvider()
{
	delete m_a;
	delete m_b;
	delete m_z;
}

QString Kovan::ButtonProvider::text(const ButtonProvider::ButtonId& id) const
{
	switch(id) {
		case A: return QString::fromAscii(m_a->text());
		case B: return QString::fromAscii(m_b->text());
		case Z: return QString::fromAscii(m_z->text());
	}
	return QString();
}

bool Kovan::ButtonProvider::setPressed(const ButtonProvider::ButtonId& id, bool pressed) const
{
	switch(id) {
		case A: m_a->setPressed(pressed);
		case B: m_b->setPressed(pressed);
		case Z: m_z->setPressed(pressed);
	}
	return false;
}

void Kovan::ButtonProvider::refresh()
{
	if(m_a->isTextDirty()) emit buttonTextChanged(A, text(A));
	if(m_b->isTextDirty()) emit buttonTextChanged(B, text(B));
	if(m_z->isTextDirty()) emit buttonTextChanged(Z, text(Z));
}