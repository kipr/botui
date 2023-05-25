#ifndef _ROOTCONTROLLER_H_
#define _ROOTCONTROLLER_H_

#include "NetworkSettingsWidget.h"
#include "Singleton.h"
#include <QDialog>
#include <QMap>
#include <QObject>
#include <QStack>
#include <QUrl>
#include <QWidget>

class RootController : public QObject, public Singleton<RootController> {
    Q_OBJECT
    Q_PROPERTY(bool dismissable READ isDismissable WRITE setDismissable)
    Q_PROPERTY(unsigned int depth READ depth)
  public:
    RootController();

    const unsigned int depth() const;

    void setDismissable(bool dismissable);
    bool isDismissable() const;

    void setFullscreen(const bool fullscreen);
    bool isFullscreen() const;

    template <typename T> void dismissUntil() {
        while (depth() > 1 && !dynamic_cast<T *>(m_stack.top()))
            m_stack.pop();
    }

    template <typename T> bool containsWidget() {
        QStack<QWidget *>::ConstIterator it = m_stack.begin();
        for (; it != m_stack.end(); ++it) {
            if (dynamic_cast<T *>(*it)) return true;
        }
        return false;
    }

  public slots:
    void presentQml(const QUrl &url);
    int presentDialog(QDialog *dialog);
    void presentWidget(QWidget *widget, bool owns = true);

    void dismissWidget();
    void dismissAllWidgets();

    void minimize();

  private:
    void constrain(QWidget *widget);
    void present(QWidget *widget);

    QMap<QWidget *, bool> m_ownership;
    QStack<QWidget *> m_stack;
    bool m_dismissable;
    bool m_fullscreen;
};

#endif
