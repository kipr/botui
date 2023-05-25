#ifndef _STOPBUTTONWIDGET_H_
#define _STOPBUTTONWIDGET_H_

#include <QWidget>

class Program;

class StopButtonWidget : public QWidget {
    Q_OBJECT
  public:
    StopButtonWidget(Program *program, QWidget *parent = 0);
    virtual QSize sizeHint() const;
  signals:
    void clicked();

  protected:
    void paintEvent(QPaintEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
  private slots:
    void started();
    void finished();

  private:
    Program *m_program;
    bool m_running;
    bool m_pressed;
};

#endif
