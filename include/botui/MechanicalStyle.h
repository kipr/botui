#ifndef _MECHANICALSTYLE_H_
#define _MECHANICALSTYLE_H_

#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QProxyStyle>
#else
#include <QPlastiqueStyle>
#endif

#if QT_VERSION >= 0x050000
class MechanicalStyle : public QProxyStyle
#else
class MechanicalStyle : public QPlastiqueStyle
#endif
{
    Q_OBJECT
  public:
    enum CustomControlElement { CE_StopButton = CE_CustomBase + 1 };

    MechanicalStyle();

    void setUserColor(const QColor &userColor);
    const QColor &userColor() const;

    virtual QRect subControlRect(ComplexControl cc,
                                 const QStyleOptionComplex *opt,
                                 SubControl sc,
                                 const QWidget *widget) const;
    virtual void drawComplexControl(ComplexControl cc,
                                    const QStyleOptionComplex *opt,
                                    QPainter *p,
                                    const QWidget *widget = 0) const;
    virtual void drawControl(ControlElement ce,
                             const QStyleOption *opt,
                             QPainter *p,
                             const QWidget *widget = 0) const;
    virtual void drawPrimitive(PrimitiveElement pe,
                               const QStyleOption *opt,
                               QPainter *p,
                               const QWidget *w = 0) const;
    virtual int pixelMetric(PixelMetric pm,
                            const QStyleOption *opt = 0,
                            const QWidget *widget = 0) const;
    virtual void polish(QApplication *app);
    virtual void polish(QWidget *widget);
    virtual void polish(QPalette &pal);
    virtual QSize sizeFromContents(ContentsType ct,
                                   const QStyleOption *opt,
                                   const QSize &csz,
                                   const QWidget *widget = 0) const;
    virtual QPixmap standardPixmap(StandardPixmap standardPixmap,
                                   const QStyleOption *opt,
                                   const QWidget *widget = 0) const;
    virtual int styleHint(StyleHint hint,
                          const QStyleOption *opt = 0,
                          const QWidget *widget = 0,
                          QStyleHintReturn *returnData = 0) const;
    virtual QRect subElementRect(SubElement sr,
                                 const QStyleOption *opt,
                                 const QWidget *w = 0) const;
    virtual void unpolish(QApplication *app);
    virtual void unpolish(QWidget *widget);

  private:
    QColor m_userColor;
};

#endif
