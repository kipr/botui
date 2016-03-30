#include "WallabyButtonProvider.h"

#include <wallaby/button.hpp>
#include <wallaby/general.h>

#include <QDebug>

Wallaby::ButtonProvider::ButtonProvider(QObject *parent)
  : ::ButtonProvider(parent),
  m_shown(false)
{
}

Wallaby::ButtonProvider::~ButtonProvider()
{
}

bool Wallaby::ButtonProvider::isExtraShown() const
{
  return ExtraButtons::isShown();
}

QString Wallaby::ButtonProvider::text(const ButtonProvider::ButtonId& id) const
{
  AbstractTextButton *button = lookup(id);
  return button ? QString::fromUtf8(button->text()) : QString();
}

bool Wallaby::ButtonProvider::setPressed(const ButtonProvider::ButtonId& id, bool pressed) const
{
  AbstractTextButton *button = lookup(id);
  if(!button) return false;
  
  publish();
  button->setPressed(pressed);
  publish();
  
  return true;
}

void Wallaby::ButtonProvider::reset()
{
  ExtraButtons::hide();
  publish();
  m_shown = false;
  emit extraShownChanged(false);
}

void Wallaby::ButtonProvider::refresh()
{
  publish();
  if(m_shown != ExtraButtons::isShown()) {
    qDebug() << "Shown was dirty!";
    m_shown = ExtraButtons::isShown();
    emit extraShownChanged(m_shown);
  }
}

AbstractTextButton *Wallaby::ButtonProvider::lookup(const ButtonProvider::ButtonId& id) const
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
