#ifndef _BUTTONPROVIDER_H_
#define _BUTTONPROVIDER_H_

#include <QObject>
#include <QString>

class ButtonProvider : public QObject {
    Q_OBJECT
  public:
    enum ButtonId { A, B, C, X, Y, Z };

    ButtonProvider(QObject *parent = 0);
    virtual ~ButtonProvider();

    virtual bool isExtraShown() const = 0;
    virtual QString text(const ButtonProvider::ButtonId &id) const = 0;
    virtual bool setPressed(const ButtonProvider::ButtonId &id,
                            bool pressed) const = 0;

  public slots:
    virtual void reset() = 0;
    virtual void refresh() = 0;

  signals:
    void buttonTextChanged(const ButtonProvider::ButtonId &id,
                           const QString &text);
    void extraShownChanged(const bool &shown);

  private:
};

#endif
