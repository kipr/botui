#include "StopButtonWidget.h"

#include "MechanicalStyle.h"
#include "Program.h"

#include <QStyleOptionButton>
#include <QStylePainter>

StopButtonWidget::StopButtonWidget(Program *program, QWidget *parent)
    : QWidget(parent),
      m_program(program),
      m_running(m_program->isRunning()),
      m_pressed(false) {
    connect(m_program, SIGNAL(started()), SLOT(started()));
    connect(m_program,
            SIGNAL(finished(int, QProcess::ExitStatus)),
            SLOT(finished()));
}

QSize StopButtonWidget::sizeHint() const { return QSize(80, 21); }

void StopButtonWidget::paintEvent(QPaintEvent *event) {
    QStylePainter p(this);
    QStyleOptionButton opt;
    opt.rect = rect().adjusted(0, 0, -2, -1);
    opt.text = tr("Stop");
    if (m_running) opt.state |= QStyle::State_Enabled;
    if (m_running && m_pressed) opt.state |= QStyle::State_Sunken;
    p.drawControl((QStyle::ControlElement)MechanicalStyle::CE_StopButton, opt);
}

void StopButtonWidget::mousePressEvent(QMouseEvent *event) {
    m_pressed = true;
    update();
}

void StopButtonWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (m_pressed != false) emit clicked();
    m_pressed = false;
    update();
}

void StopButtonWidget::started() {
    m_running = true;
    update();
}

void StopButtonWidget::finished() {
    m_running = false;
    update();
}
