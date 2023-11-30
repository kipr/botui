#include "Create3SensorListWidget.h"
#include "ui_Create3SensorListWidget.h"

#include "Create3SensorModel.h"
#include <kipr/create3/client/client.h>
#include <unistd.h>
#include "NumpadDialog.h"
#include <QDebug>
#include <stdio.h>
#include <QRegularExpression>
#include <QProcess>
#include <QHeaderView>
#include <cmath>
#include <QTimer>
#include <QPainter>
#include <QItemDelegate>

class Create3SensorItemDelegate : public QItemDelegate
{
public:
    Create3SensorItemDelegate(Create3SensorModel *const model, QObject *const parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const Create3SensorModel *const _model;
    const QPixmap _up;
    const QPixmap _down;
};

Create3SensorItemDelegate::Create3SensorItemDelegate(Create3SensorModel *const model, QObject *const parent)
    : QItemDelegate(parent), _model(model), _up(QIcon(":/icons/arrow_up.png").pixmap(16, 16)), _down(QIcon(":/icons/arrow_down.png").pixmap(16, 16))
{
}

void Create3SensorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

Create3SensorListWidget::Create3SensorListWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::Create3SensorListWidget), _model(new Create3SensorModel(this))
{
    ui->setupUi(this);
    performStandardSetup(tr("Create 3"), false);
    setStyleSheet("QScrollBar:vertical {border: 2px solid grey;background:grey ;width: 100px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: 2px solid grey;background: white; min-height: 20px; } QScrollBar::add-line:vertical { border: 2px solid grey;background: #32CC99; height: 20px; subcontrol-position: bottom; subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: 2px solid grey; background: #32CC99; height: 20px; subcontrol-position: top; subcontrol-origin: margin; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid grey;width: 3px; height: 3px; background: white; }QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {border: 2px solid grey; background: none;}");
    // connect(ui->Create3ConnectButton, SIGNAL(clicked()), SLOT(create3Connect()));

    ui->sensors->setModel(_model);
    ui->sensors->setItemDelegate(new Create3SensorItemDelegate(_model, this));
    QTimer *timer = new QTimer(this);
    _model->connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(200); // 5 FPS
}

Create3SensorListWidget::~Create3SensorListWidget()
{
    delete ui;
}


