#include "WombatButtonProvider.h"

#include <kipr/button/button.hpp>

#include <QDebug>

Wombat::ButtonProvider::ButtonProvider(QObject *parent)
  : ::ButtonProvider(parent),
  m_shown(false)
{
}

Wombat::ButtonProvider::~ButtonProvider()
{
}

bool Wombat::ButtonProvider::isExtraShown() const
{
  return ExtraButtons::isShown();
}

QString Wombat::ButtonProvider::text(const ButtonProvider::ButtonId& id) const
{
  AbstractTextButton *button = lookup(id);
  return button ? QString::fromUtf8(button->text()) : QString();
}

bool Wombat::ButtonProvider::setPressed(const ButtonProvider::ButtonId& id, bool pressed) const
{
  AbstractTextButton *button = lookup(id);
  if(!button) return false;
  
  publish();
  button->setPressed(pressed);
  publish();
  
  return true;
}

void Wombat::ButtonProvider::reset()
{
  ExtraButtons::hide();
  publish();
  m_shown = false;
  emit extraShownChanged(false);
}

void Wombat::ButtonProvider::refresh()
{
  publish();
  if(m_shown != ExtraButtons::isShown()) {
    qDebug() << "Shown was dirty!";
    m_shown = ExtraButtons::isShown();
    emit extraShownChanged(m_shown);
  }
}

AbstractTextButton *Wombat::ButtonProvider::lookup(const ButtonProvider::ButtonId& id) const
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
