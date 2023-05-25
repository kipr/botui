#include "SensorListWidget.h"
#include "ui_SensorListWidget.h"

#include "MenuBar.h"
#include "SensorModel.h"

#include <QDebug>
#include <QHeaderView>
#include <QItemDelegate>
#include <QPainter>
#include <QTimer>

class SensorItemDelegate : public QItemDelegate {
  public:
    SensorItemDelegate(SensorModel *const model, QObject *const parent = 0);
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

  private:
    const SensorModel *const _model;
    const QPixmap _up;
    const QPixmap _down;
};

SensorItemDelegate::SensorItemDelegate(SensorModel *const model,
                                       QObject *const parent)
    : QItemDelegate(parent),
      _model(model),
      _up(QIcon(":/icons/arrow_up.png").pixmap(16, 16)),
      _down(QIcon(":/icons/arrow_down.png").pixmap(16, 16)) {}

void SensorItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const {
    QItemDelegate::paint(painter, option, index);
}

SensorListWidget::SensorListWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::SensorListWidget()),
      _model(new SensorModel(this)) {
    ui->setupUi(this);

    performStandardSetup(tr("Sensor List"));
    setStyleSheet(
        "QScrollBar:vertical {border: 2px solid grey;background:grey ;width: "
        "100px; margin: 22px 0 22px 0; } QScrollBar::handle:vertical { border: "
        "2px solid grey;background: white; min-height: 20px; } "
        "QScrollBar::add-line:vertical { border: 2px solid grey;background: "
        "#32CC99; height: 20px; subcontrol-position: bottom; "
        "subcontrol-origin: margin; } QScrollBar::sub-line:vertical {border: "
        "2px solid grey; background: #32CC99; height: 20px; "
        "subcontrol-position: top; subcontrol-origin: margin; } "
        "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { "
        "border: 2px solid grey;width: 3px; height: 3px; background: white; "
        "}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical "
        "{border: 2px solid grey; background: none;}");

    ui->sensors->setModel(_model);
    ui->sensors->setItemDelegate(new SensorItemDelegate(_model, this));
    /*QHeaderView *verticalHeader = sensors->verticalHeader();
    ui->sensors->setResizeMode(QHeaderView::Fixed);
    ui->sensors->setDefaultSectionSize(24);*/

    QTimer *timer = new QTimer(this);
    _model->connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(200); // 5 FPS
}

SensorListWidget::~SensorListWidget() { delete ui; }
