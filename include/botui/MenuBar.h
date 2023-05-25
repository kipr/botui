#ifndef _MENUBAR_H_
#define _MENUBAR_H_

#include <QMenuBar>
#include <QPixmap>

class QLabel;
class QHBoxLayout;
class StopButtonWidget;
class Device;
class Program;

class MenuBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
  public:
    MenuBar(Program *program, QWidget *parent = 0);
    MenuBar(QWidget *parent = 0);
    ~MenuBar();

    void setTitle(const QString &title, const QPixmap &icon = QPixmap());
    QString title() const;

    void addHomeAndBackButtons();

    void addAction(QAction *const action);
    QAction *addAction(const QString &str);

  private:
    void init();

    void clear();

    QLabel *m_title;
    Program *m_program;
    StopButtonWidget *m_stopButton;

    QHBoxLayout *m_layout;
};

#endif
