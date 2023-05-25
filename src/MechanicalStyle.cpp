#include "MechanicalStyle.h"

#include <QPainter>
#include <QStyleOption>

#include <QApplication>
#include <QDebug>

#if QT_VERSION >= 0x050000
#define PARENT_STYLE_CLASS QProxyStyle
#include <QStyleFactory>
#else
#define PARENT_STYLE_CLASS QPlastiqueStyle
#endif

#define BUTTON_DECORATION_OFFSET 4

inline static QColor mechanical_menubar_disabled_top_color() {
    return QColor(220, 220, 220);
}

inline static QColor mechanical_menubar_disabled_bottom_color() {
    return QColor(200, 200, 200);
}

inline static QColor mechanical_menubar_bottom_color(const QColor &top) {
    return QColor(top.red() - 30, top.green() - 30, top.blue() - 30);
}

inline static QColor mechanical_button_top_color() {
    return QColor(230, 230, 230);
}

inline static QColor mechanical_button_bottom_color() {
    return QColor(192, 192, 192);
}

inline static QColor mechanical_disabled_button_top_color() {
    return QColor(130, 130, 130);
}

inline static QColor mechanical_disabled_button_bottom_color() {
    return QColor(92, 92, 92);
}

inline static QColor mechanical_button_pressed_top_color() {
    return QColor(200, 200, 200);
}

inline static QColor mechanical_button_pressed_bottom_color() {
    return QColor(142, 142, 142);
}

inline static QColor mechanical_button_disabled_top_color() {
    return QColor(240, 240, 240);
}

inline static QColor mechanical_button_disabled_bottom_color() {
    return QColor(220, 220, 220);
}

inline static QColor mechanical_stop_button_top_color() {
    return QColor(250, 150, 150);
}

inline static QColor mechanical_stop_button_bottom_color() {
    return QColor(212, 112, 112);
}

inline static QColor mechanical_stop_button_pressed_top_color() {
    return QColor(200, 100, 100);
}

inline static QColor mechanical_stop_button_pressed_bottom_color() {
    return QColor(142, 42, 42);
}

static void mechanical_draw_styled_rectangle(const QRect &rect,
                                             QPainter *p,
                                             bool withScrews = true) {
    const static unsigned int edgeOffset = BUTTON_DECORATION_OFFSET;
    QPoint polygon[9];
    polygon[0] = QPoint(rect.left() + edgeOffset, rect.top());
    polygon[1] = QPoint(rect.right() - edgeOffset, rect.top());
    polygon[2] = QPoint(rect.right(), rect.top() + edgeOffset);
    polygon[3] = QPoint(rect.right(), rect.bottom() - edgeOffset);
    polygon[4] = QPoint(rect.right() - edgeOffset, rect.bottom());
    polygon[5] = QPoint(rect.left() + edgeOffset, rect.bottom());
    polygon[6] = QPoint(rect.left(), rect.bottom() - edgeOffset);
    polygon[7] = QPoint(rect.left(), rect.top() + edgeOffset);
    p->drawPolygon(polygon, 8);

    if (!withScrews) return;

    const static unsigned int screwRadius = 1;
    const unsigned int screwOffset = edgeOffset + 1;
    p->drawEllipse(QPoint(rect.left() + screwOffset, rect.top() + screwOffset),
                   screwRadius,
                   screwRadius);
    p->drawEllipse(QPoint(rect.right() - screwOffset, rect.top() + screwOffset),
                   screwRadius,
                   screwRadius);
    p->drawEllipse(
        QPoint(rect.right() - screwOffset, rect.bottom() - screwOffset),
        screwRadius,
        screwRadius);
    p->drawEllipse(
        QPoint(rect.left() + screwOffset, rect.bottom() - screwOffset),
        screwRadius,
        screwRadius);
}

static void mechanical_draw_rectangle(const QRect &rect,
                                      QPainter *p,
                                      bool withScrews = true) {
    const static unsigned int edgeOffset = BUTTON_DECORATION_OFFSET;
    QPoint polygon[9];
    polygon[0] = QPoint(rect.left(), rect.top());
    polygon[1] = QPoint(rect.right(), rect.top());
    polygon[2] = QPoint(rect.right(), rect.top());
    polygon[3] = QPoint(rect.right(), rect.bottom());
    polygon[4] = QPoint(rect.right(), rect.bottom());
    polygon[5] = QPoint(rect.left(), rect.bottom());
    polygon[6] = QPoint(rect.left(), rect.bottom());
    polygon[7] = QPoint(rect.left(), rect.top());
    p->drawPolygon(polygon, 8);

    if (!withScrews) return;

    const static unsigned int screwRadius = 1;
    const unsigned int screwOffset = edgeOffset + 1;
    p->drawEllipse(QPoint(rect.left() + screwOffset, rect.top() + screwOffset),
                   screwRadius,
                   screwRadius);
    p->drawEllipse(QPoint(rect.right() - screwOffset, rect.top() + screwOffset),
                   screwRadius,
                   screwRadius);
    p->drawEllipse(
        QPoint(rect.right() - screwOffset, rect.bottom() - screwOffset),
        screwRadius,
        screwRadius);
    p->drawEllipse(
        QPoint(rect.left() + screwOffset, rect.bottom() - screwOffset),
        screwRadius,
        screwRadius);
}

static void mechanical_draw_styled_button(const QRect &rect,
                                          const QStyleOption *opt,
                                          QPainter *p,
                                          bool withScrews = true) {
    QColor top = mechanical_button_top_color();
    QColor bottom = mechanical_button_bottom_color();

    if (opt->state & QStyle::State_Sunken) {
        top = mechanical_button_pressed_top_color();
        bottom = mechanical_button_pressed_bottom_color();
    }

    if (!(opt->state & QStyle::State_Enabled)) {
        top = mechanical_button_disabled_top_color();
        bottom = mechanical_button_disabled_bottom_color();
    }

    QLinearGradient gradient =
        QLinearGradient(rect.right(), rect.top(), rect.right(), rect.bottom());
    gradient.setColorAt(0, top);
    gradient.setColorAt(1, bottom);
    p->setPen(Qt::black);
    p->setBrush(gradient);
    mechanical_draw_styled_rectangle(rect, p, withScrews);
}

MechanicalStyle::MechanicalStyle() {
#if QT_VERSION >= 0x050000
    setBaseStyle(QStyleFactory::create("fusion"));
#endif
}

void MechanicalStyle::setUserColor(const QColor &userColor) {
    m_userColor = userColor;
}

const QColor &MechanicalStyle::userColor() const { return m_userColor; }

QRect MechanicalStyle::subControlRect(ComplexControl cc,
                                      const QStyleOptionComplex *opt,
                                      SubControl sc,
                                      const QWidget *widget) const {
    QRect ret = PARENT_STYLE_CLASS ::subControlRect(cc, opt, sc, widget);
    if (cc == CC_ScrollBar) {
        const QStyleOptionSlider *scrollbar =
            qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!scrollbar) return ret;
        const QRect scrollBarRect = scrollbar->rect;
        int sbextent =
            proxy()->pixelMetric(PM_ScrollBarExtent, scrollbar, widget);
        int maxlen = ((scrollbar->orientation == Qt::Horizontal)
                          ? scrollBarRect.width()
                          : scrollBarRect.height()) -
                     (sbextent * 2);
        int sliderlen;

        // calculate slider length
        if (scrollbar->maximum != scrollbar->minimum) {
            uint range = scrollbar->maximum - scrollbar->minimum;
            sliderlen = (qint64(scrollbar->pageStep) * maxlen) /
                        (range + scrollbar->pageStep);
            int slidermin =
                proxy()->pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
            if (sliderlen < slidermin || range > INT_MAX / 2)
                sliderlen = slidermin;
            if (sliderlen > maxlen) sliderlen = maxlen;
        }
        else
            sliderlen = maxlen;

        int sliderstart =
            sbextent + sliderPositionFromValue(scrollbar->minimum,
                                               scrollbar->maximum,
                                               scrollbar->sliderPosition,
                                               maxlen - sliderlen,
                                               scrollbar->upsideDown);

        switch (sc) {
        case SC_ScrollBarSubLine: // top/left button
            if (scrollbar->orientation == Qt::Horizontal) {
                int buttonWidth = qMin(scrollBarRect.width() / 2, sbextent);
                ret.setRect(0, 0, buttonWidth, scrollBarRect.height());
            }
            else {
                int buttonHeight = qMin(scrollBarRect.height() / 2, sbextent);
                ret.setRect(0, 0, scrollBarRect.width(), buttonHeight);
            }
            break;
        case SC_ScrollBarAddLine: // bottom/right button
            if (scrollbar->orientation == Qt::Horizontal) {
                int buttonWidth = qMin(scrollBarRect.width() / 2, sbextent);
                ret.setRect(scrollBarRect.width() - buttonWidth,
                            0,
                            buttonWidth,
                            scrollBarRect.height());
            }
            else {
                int buttonHeight = qMin(scrollBarRect.height() / 2, sbextent);
                ret.setRect(0,
                            scrollBarRect.height() - buttonHeight,
                            scrollBarRect.width(),
                            buttonHeight);
            }
            break;
        case SC_ScrollBarSubPage: // between top/left button and slider
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sbextent,
                            0,
                            sliderstart - sbextent,
                            scrollBarRect.height());
            else
                ret.setRect(
                    0, sbextent, scrollBarRect.width(), sliderstart - sbextent);
            break;
        case SC_ScrollBarAddPage: // between bottom/right button and slider
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sliderstart + sliderlen,
                            0,
                            maxlen - sliderstart - sliderlen + sbextent,
                            scrollBarRect.height());
            else
                ret.setRect(0,
                            sliderstart + sliderlen,
                            scrollBarRect.width(),
                            maxlen - sliderstart - sliderlen + sbextent);
            break;
        case SC_ScrollBarGroove:
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sbextent,
                            0,
                            scrollBarRect.width() - sbextent * 2,
                            scrollBarRect.height());
            else
                ret.setRect(0,
                            sbextent,
                            scrollBarRect.width(),
                            scrollBarRect.height() - sbextent * 2);
            break;
        case SC_ScrollBarSlider:
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
            else
                ret.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
            break;
        default:
            break;
        }
        ret = visualRect(scrollbar->direction, scrollBarRect, ret);
    }
    return ret;
}

void MechanicalStyle::drawComplexControl(ComplexControl cc,
                                         const QStyleOptionComplex *opt,
                                         QPainter *p,
                                         const QWidget *widget) const {
    if (cc == CC_ScrollBar) {
        const QStyleOptionSlider *scrollbar =
            qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!scrollbar) return;
        QStyleOptionSlider newScrollbar = *scrollbar;
        State saveFlags = scrollbar->state;

        if (scrollbar->subControls & SC_ScrollBarSubLine) {
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarSubLine, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarSubLine))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarSubLine, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarAddLine) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarAddLine, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarAddLine))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarAddLine, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarSubPage) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarSubPage, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarSubPage))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarSubPage, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarAddPage) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarAddPage, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarAddPage))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarAddPage, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarFirst) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarFirst, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarFirst))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarFirst, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarLast) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarLast, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarLast))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarLast, &newScrollbar, p, widget);
            }
        }
        if (scrollbar->subControls & SC_ScrollBarSlider) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(
                cc, &newScrollbar, SC_ScrollBarSlider, widget);
            if (newScrollbar.rect.isValid()) {
                if (!(scrollbar->activeSubControls & SC_ScrollBarSlider))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(
                    CE_ScrollBarSlider, &newScrollbar, p, widget);
            }
        }

        return;
    }
    if (cc == CC_Slider) {
        const QStyleOptionSlider *so =
            qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!so) return;
        const QRect &rect = opt->rect;
        // qDebug() << "Slider rect:" << rect;
        QLinearGradient gradient = QLinearGradient(
            rect.right(), rect.top(), rect.right(), rect.bottom());
        gradient.setColorAt(0, m_userColor);
        gradient.setColorAt(1, mechanical_menubar_bottom_color(m_userColor));
        p->save();
        p->setBrush(QColor(0, 0, 0));
        p->setPen(QPen(QColor(0, 0, 0, 0)));
        mechanical_draw_styled_rectangle(
            rect.adjusted(rect.width() / 3, 0, -rect.width() / 3, 0), p, false);
        // qDebug() << "slider pos" << so->sliderPosition;
        double pos = (so->sliderPosition - so->minimum) /
                     (double)(so->maximum - so->minimum);
        if (so->orientation == Qt::Vertical) {
            static const QColor wood = QColor(173, 134, 106);
            const int yOff = rect.bottom() - pos * rect.height();
            QRectF lever(2 * rect.width() / 5.0,
                         rect.center().y(),
                         rect.width() / 5.0 - 1,
                         yOff - rect.center().y());
            const bool reverse = yOff - rect.center().y();
            QLinearGradient gradient = QLinearGradient(
                lever.right(), lever.top(), lever.right(), lever.bottom());
            gradient.setColorAt(reverse ? 1 : 0, wood);
            gradient.setColorAt(reverse ? 0 : 1, QColor(0, 0, 0));
            p->setBrush(gradient);
            p->drawRect(lever);

            QRect button(rect.left(), yOff - 10, rect.right(), 20);

            QStyleOption pass = *opt;
            pass.state = QStyle::State_None;
            mechanical_draw_styled_button(button, &pass, p);
            p->setPen(QColor(130, 130, 130));
            p->drawText(button,
                        QObject::tr("Drag"),
                        QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter |
                                    Qt::AlignVCenter));
        }
        p->restore();

        return;
    }
    if (cc == CC_ComboBox) {
        const QStyleOptionComboBox *co =
            qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if (!co) return;
        mechanical_draw_styled_button(co->rect, co, p, false);
        const int top = co->rect.top() + co->rect.height() / 2 - 2;
        p->setBrush(Qt::black);
        QPoint polygon[] = {QPoint(co->rect.right() - 20, top + 0),
                            QPoint(co->rect.right() - 10, top + 0),
                            QPoint(co->rect.right() - 15, top + 5)};
        p->drawPolygon(polygon, 3);

        return;
    }

    PARENT_STYLE_CLASS ::drawComplexControl(cc, opt, p, widget);
}

void MechanicalStyle::drawControl(ControlElement ce,
                                  const QStyleOption *opt,
                                  QPainter *p,
                                  const QWidget *widget) const {
    CustomControlElement cce = (CustomControlElement)ce;
    if (ce == CE_PushButton) {
        const QStyleOptionButton *bo =
            qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!bo) return;
        p->save();
        mechanical_draw_styled_button(bo->rect, opt, p);
        p->restore();
        drawControl(CE_PushButtonLabel, opt, p, widget);

        return;
    }
    if (cce == CE_StopButton) {
        const QStyleOptionButton *bo =
            qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!bo) return;
        if (!(opt->state & QStyle::State_Enabled)) return;
        p->save();

        QColor top = mechanical_stop_button_top_color();
        QColor bottom = mechanical_stop_button_bottom_color();

        if (opt->state & QStyle::State_Sunken) {
            top = mechanical_stop_button_pressed_top_color();
            bottom = mechanical_stop_button_pressed_bottom_color();
        }
        const QRect &rect = opt->rect;
        QLinearGradient gradient = QLinearGradient(
            rect.right(), rect.top(), rect.right(), rect.bottom());
        gradient.setColorAt(0, top);
        gradient.setColorAt(1, bottom);
        p->setPen(Qt::black);
        p->setBrush(gradient);
        mechanical_draw_styled_rectangle(rect, p, false);

        p->restore();
        p->drawText(rect,
                    bo->text,
                    QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter |
                                Qt::AlignVCenter));

        return;
    }
    if (ce == CE_MenuBarEmptyArea) {
        // const QStyleOptionMenuItem *bo = qstyleoption_cast<const
        // QStyleOptionMenuItem *>(opt); if(!bo) return;
        const QRect &rect = opt->rect;

        QLinearGradient gradient = QLinearGradient(
            rect.right(), rect.top(), rect.right(), rect.bottom());
        gradient.setColorAt(0, m_userColor);
        gradient.setColorAt(1, mechanical_menubar_bottom_color(m_userColor));
        p->save();
        p->setPen(QPen(QColor(Qt::black), 0));
        p->setBrush(gradient);
        mechanical_draw_styled_rectangle(
            opt->rect.adjusted(1, 1, -1, -1), p, false);
        p->restore();

        return;
    }
    if (ce == CE_PushButtonLabel) {
        const QStyleOptionButton *button =
            qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!button) return;
        QRect textRect = button->rect;
        uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, button, widget))
            tf |= Qt::TextHideMnemonic;

        if (!button->icon.isNull()) {
            QRect iconRect;
            QIcon::Mode mode =
                button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
            if (mode == QIcon::Normal && button->state & State_HasFocus)
                mode = QIcon::Active;
            QIcon::State state = QIcon::Off;
            if (button->state & State_On) state = QIcon::On;

            QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
            const int labelWidth = pixmap.width();
            const int labelHeight = pixmap.height();
            int iconSpacing =
                4; //### 4 is currently hardcoded in QPushButton::sizeHint()

            iconRect = QRect(BUTTON_DECORATION_OFFSET * 2 + iconSpacing,
                             opt->rect.top() + opt->rect.height() / 2 -
                                 labelHeight / 2,
                             labelWidth,
                             labelHeight);

            iconRect = visualRect(button->direction, textRect, iconRect);

            if (button->state & (State_On | State_Sunken))
                iconRect.translate(
                    proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget),
                    proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget));
            p->drawPixmap(iconRect, pixmap);
        }
        tf |= Qt::AlignHCenter;

        if (button->state & (State_On | State_Sunken))
            textRect.translate(
                proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget),
                proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget));

        if (button->features & QStyleOptionButton::HasMenu) {
            int indicatorSize =
                proxy()->pixelMetric(PM_MenuButtonIndicator, button, widget);
            if (button->direction == Qt::LeftToRight)
                textRect = textRect.adjusted(0, 0, -indicatorSize, 0);
            else
                textRect = textRect.adjusted(indicatorSize, 0, 0, 0);
        }

        p->drawText(textRect, tf, button->text);

        return;
    }
    if (ce == CE_MenuHMargin) {

        return;
    }
    if (ce == CE_MenuVMargin) {

        return;
    }
    if (ce == CE_MenuBarItem) {
        const QStyleOptionMenuItem *mo =
            qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if (!mo) return;
        const QRect &rect = opt->rect;
        QLinearGradient gradient = QLinearGradient(
            rect.right(), rect.top(), rect.right(), rect.bottom());
        gradient.setColorAt(0, m_userColor);
        gradient.setColorAt(1, mechanical_menubar_bottom_color(m_userColor));
        p->save();
        p->setBrush(gradient);
        p->setPen(QPen(QColor(0, 0, 0, 0)));
        p->drawRect(rect.adjusted(0, 0, 0, 0));
        mechanical_draw_styled_button(mo->rect, opt, p, false);
        QFont font(p->font());
        font.setPointSizeF(font.pointSizeF() - 1.0);
        p->setFont(font);
        p->drawText(opt->rect,
                    mo->text,
                    QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter |
                                Qt::AlignVCenter));
        p->restore();
        return;
    }

    if (ce == CE_ScrollBarAddLine) {
        QRect rect = opt->rect;
        p->save();
        mechanical_draw_styled_button(opt->rect, opt, p);
        p->setPen(Qt::darkGray);
        p->setBrush(Qt::darkGray);
        QPointF polygon[3] = {QPoint(rect.left() + rect.width() / 2 - 5,
                                     rect.top() + rect.height() / 2 - 2),
                              QPoint(rect.left() + rect.width() / 2 + 5,
                                     rect.top() + rect.height() / 2 - 2),
                              QPoint(rect.left() + rect.width() / 2 + 0,
                                     rect.top() + rect.height() / 2 + 4)};
        p->drawPolygon(polygon, 3);
        p->restore();
        return;
    }

    if (ce == CE_ScrollBarSubLine) {
        p->save();
        QRect rect = opt->rect;
        mechanical_draw_styled_button(opt->rect, opt, p);
        p->setPen(Qt::darkGray);
        p->setBrush(Qt::darkGray);
        QPointF polygon[3] = {QPoint(rect.left() + rect.width() / 2 - 5,
                                     rect.top() + rect.height() / 2 + 2),
                              QPoint(rect.left() + rect.width() / 2 + 5,
                                     rect.top() + rect.height() / 2 + 2),
                              QPoint(rect.left() + rect.width() / 2 + 0,
                                     rect.top() + rect.height() / 2 - 4)};
        p->drawPolygon(polygon, 3);
        p->restore();
        return;
    }

    if (ce == CE_ScrollBarSlider) {
        p->save();
        QRect rect = opt->rect;
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::white);
        p->drawRect(rect);
        mechanical_draw_styled_button(opt->rect, opt, p, false);
        p->restore();
        return;
    }

    if (ce == QStyle::CE_TabBarTab) {
        p->save();
        QRect rect = opt->rect;
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::white);
        p->drawRect(rect);

        QColor top = mechanical_button_top_color();
        QColor bottom = mechanical_button_bottom_color();

        const bool selected = opt->state & QStyle::State_Selected;
        if (selected) {
            top = mechanical_button_pressed_top_color();
            bottom = mechanical_button_pressed_bottom_color();
        }

        QLinearGradient gradient = QLinearGradient(
            rect.right(), rect.top(), rect.right(), rect.bottom());
        gradient.setColorAt(0, top);
        gradient.setColorAt(1, bottom);
        p->setPen(Qt::black);
        p->setBrush(gradient);

        mechanical_draw_rectangle(
            rect.adjusted(0, 0, 0, selected ? 0 : -2), p, false);
        p->restore();

        PARENT_STYLE_CLASS ::drawControl(
            QStyle::CE_TabBarTabLabel, opt, p, widget);
        return;
    }

    if (ce == CE_FocusFrame) return; // No focus

    PARENT_STYLE_CLASS ::drawControl(ce, opt, p, widget);
}

void MechanicalStyle::drawPrimitive(PrimitiveElement pe,
                                    const QStyleOption *opt,
                                    QPainter *p,
                                    const QWidget *w) const {
    // Disable focus rectangle
    if (pe == PE_FrameFocusRect) return;
    if (pe == PE_FrameStatusBarItem) return;

    PARENT_STYLE_CLASS ::drawPrimitive(pe, opt, p, w);
}

int MechanicalStyle::pixelMetric(PixelMetric pm,
                                 const QStyleOption *opt,
                                 const QWidget *widget) const {
    if (pm == PM_MenuBarItemSpacing) {
        return 2;
    }
    if (pm == PM_MenuBarHMargin) {
        return 0;
    }
    if (pm == PM_ScrollBarExtent) {
        return 27;
    }
    if (pm == PM_ScrollBarSliderMin) return 50;
    return PARENT_STYLE_CLASS ::pixelMetric(pm, opt, widget);
}

void MechanicalStyle::polish(QApplication *app) {
    PARENT_STYLE_CLASS ::polish(app);
}

void MechanicalStyle::polish(QWidget *widget) {
    PARENT_STYLE_CLASS ::polish(widget);
}

void MechanicalStyle::polish(QPalette &pal) {
    PARENT_STYLE_CLASS ::polish(pal);
    // pal.setBrush(QPalette::Button, Qt::black);
}

QSize MechanicalStyle::sizeFromContents(ContentsType ct,
                                        const QStyleOption *opt,
                                        const QSize &csz,
                                        const QWidget *widget) const {
    QSize size = PARENT_STYLE_CLASS ::sizeFromContents(ct, opt, csz, widget);
    if (ct == CT_MenuBarItem) {
        return QSize(size.width() + BUTTON_DECORATION_OFFSET * 2,
                     size.height() < 21 ? 21 : size.height()); // Hack for X11
    }
    if (ct == CT_ComboBox) {
        return QSize(size.width() + 5, size.height() + 5);
    }
    if (ct == CT_PushButton) {
        const QStyleOptionButton *button =
            qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!button) return size;
        QIcon::Mode mode =
            button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
        if (mode == QIcon::Normal && button->state & State_HasFocus)
            mode = QIcon::Active;
        QIcon::State state = QIcon::Off;
        if (button->state & State_On) state = QIcon::On;

        QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
        return QSize(size.width() + pixmap.width() * 2, size.height());
    }
    if (ct == CT_SpinBox) {
        return QSize(size.width(), size.height() + 5);
    }
    return size;
}

QPixmap MechanicalStyle::standardPixmap(StandardPixmap standardPixmap,
                                        const QStyleOption *opt,
                                        const QWidget *widget) const {
    return PARENT_STYLE_CLASS ::standardPixmap(standardPixmap, opt, widget);
}

int MechanicalStyle::styleHint(StyleHint hint,
                               const QStyleOption *opt,
                               const QWidget *widget,
                               QStyleHintReturn *returnData) const {
    if (hint == SH_ItemView_ChangeHighlightOnFocus) return 0;

    return PARENT_STYLE_CLASS ::styleHint(hint, opt, widget, returnData);
}

QRect MechanicalStyle::subElementRect(SubElement sr,
                                      const QStyleOption *opt,
                                      const QWidget *w) const {
    return PARENT_STYLE_CLASS ::subElementRect(sr, opt, w);
}

void MechanicalStyle::unpolish(QApplication *app) {
    PARENT_STYLE_CLASS ::unpolish(app);
}

void MechanicalStyle::unpolish(QWidget *widget) {
    PARENT_STYLE_CLASS ::unpolish(widget);
}
