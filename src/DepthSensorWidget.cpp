#include "DepthSensorWidget.h"
#include "ui_DepthSensorWidget.h"

#include <kovan/depth_driver.hpp>
#include <QTimer>
#include <QDebug>

using namespace depth;

DepthSensorWidget::DepthSensorWidget(Device *device, QWidget *const parent)
  : StandardWidget(device, parent)
  , ui(new Ui::DepthSensorWidget)
{
  ui->setupUi(this);
  performStandardSetup(tr("Depth Sensor"));
  
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), SLOT(update()));
  timer->start(100); // 10 Hz
}

DepthSensorWidget::~DepthSensorWidget()
{
  try {
    DepthDriver::instance().close();
  } catch(...) {}
  delete ui;
}

void DepthSensorWidget::update()
{
  DepthDriver &driver = DepthDriver::instance();
  if(!driver.isOpen()) {
    try {
      driver.open();
    } catch(...) {}
    if(!driver.isOpen()) {
      qDebug() << "Not open yet...";
      ui->view->setPleaseWait(false);
      ui->view->setDepthImage(0);
      return;
    }
  }
  
  ui->view->setDepthImage(driver.depthImage());
}
