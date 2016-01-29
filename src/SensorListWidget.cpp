#include "SensorListWidget.h"
#include "ui_SensorListWidget.h"

#include "SensorModel.h"
#include "MenuBar.h"

#include <QItemDelegate>
#include <QTimer>
#include <QDebug>
#include <QPainter>

class SensorItemDelegate : public QItemDelegate
{
public:
  SensorItemDelegate(SensorModel *const model, QObject *const parent = 0);
  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  
private:
  const SensorModel *const _model;
  const QPixmap _up;
  const QPixmap _down;
};

SensorItemDelegate::SensorItemDelegate(SensorModel *const model, QObject *const parent)
  : QItemDelegate(parent)
  , _model(model)
  , _up(QIcon(":/icons/arrow_up.png").pixmap(16, 16))
  , _down(QIcon(":/icons/arrow_down.png").pixmap(16, 16))
{
}

void SensorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QItemDelegate::paint(painter, option, index);
}

SensorListWidget::SensorListWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent)
	, ui(new Ui::SensorListWidget())
  , _model(new SensorModel(this))
{
	ui->setupUi(this);
	
	performStandardSetup(tr("Sensor List"));
	
	ui->sensors->setModel(_model);
  ui->sensors->setItemDelegate(new SensorItemDelegate(_model, this));
	
  QTimer *timer = new QTimer(this);
  _model->connect(timer, SIGNAL(timeout()), SLOT(update()));
  timer->start(200); // 5 FPS
}

SensorListWidget::~SensorListWidget()
{
	delete ui;
}
