#include "WombatButtonProvider.h"

#include <kipr/button/button.hpp>

#include <QDebug>

using namespace kipr::button;

Wombat::ButtonProvider::ButtonProvider(QObject *parent)
    : ::ButtonProvider(parent), m_shown(false) {}

Wombat::ButtonProvider::~ButtonProvider() {}

bool Wombat::ButtonProvider::isExtraShown() const {
    return ExtraButtons::isShown();
}

QString Wombat::ButtonProvider::text(const ButtonProvider::ButtonId &id) const {
    AbstractTextButton *button = lookup(id);
    return button ? QString::fromUtf8(button->text()) : QString();
}

bool Wombat::ButtonProvider::setPressed(const ButtonProvider::ButtonId &id,
                                        bool pressed) const {
    AbstractTextButton *button = lookup(id);
    if (!button) return false;

    button->setPressed(pressed);

    return true;
}

void Wombat::ButtonProvider::reset() {
    ExtraButtons::hide();
    m_shown = false;
    emit extraShownChanged(false);
}

void Wombat::ButtonProvider::refresh() {
    if (m_shown != ExtraButtons::isShown()) {
        qDebug() << "Shown was dirty!";
        m_shown = ExtraButtons::isShown();
        emit extraShownChanged(m_shown);
    }
}

AbstractTextButton *
Wombat::ButtonProvider::lookup(const ButtonProvider::ButtonId &id) const {
    switch (id) {
    case ButtonProvider::A:
        return &kipr::button::A;
    case ButtonProvider::B:
        return &kipr::button::B;
    case ButtonProvider::C:
        return &kipr::button::C;
    case ButtonProvider::X:
        return &kipr::button::X;
    case ButtonProvider::Y:
        return &kipr::button::Y;
    case ButtonProvider::Z:
        return &kipr::button::Z;
    }
    return 0;
}
