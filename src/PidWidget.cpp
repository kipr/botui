#include "PidWidget.h"
#include "ui_PidWidget.h"

#include "ArrayHelpers.h"
#include "NumpadDialog.h"

#include <kipr/motor/motor.h>

#include <QDebug>
#include <QTimer>

PidWidget::PidWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent), ui(new Ui::PidWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Motor Control"));

    m_ports[0] = ui->_0;
    m_ports[1] = ui->_1;
    m_ports[2] = ui->_2;
    m_ports[3] = ui->_3;

    ui->position->setInputProvider(
        new NumpadDialog(tr("Position"),
                         NumpadDialog::Integer,
                         std::numeric_limits<short>::min(),
                         std::numeric_limits<short>::max(),
                         this));

    ui->speed->setMinimumValue(-2600);
    ui->speed->setMaximumValue(2600);
    ui->speed->setValue(0.0);

    connect(ui->go, SIGNAL(clicked()), SLOT(go()));

    for (quint8 i = 0; i < sizeof_array(m_ports); ++i) {
        connect(m_ports[i], SIGNAL(clicked()), SLOT(portClicked()));
    }
}

PidWidget::~PidWidget() { delete ui; }

void PidWidget::portClicked() {
    QObject *from = sender();
    if (!from) return;

    m_currentPort = 0;
    for (; m_currentPort < sizeof_array(m_ports); ++m_currentPort) {
        if (m_ports[m_currentPort] == from) break;
    }

    for (quint8 j = 0; j < sizeof_array(m_ports); ++j) {
        m_ports[j]->setEnabled(j != m_currentPort);
    }

    ui->speed->setLabel(m_currentPort);
}

void PidWidget::go() {
    int position = ui->position->text().toDouble();
    qDebug() << position;
    move_to_position(m_currentPort, position, ui->speed->value());
}
