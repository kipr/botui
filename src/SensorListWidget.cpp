#include "SensorListWidget.h"
#include "ui_SensorListWidget.h"

#include "SensorModel.h"

#include <QTimer>

SensorListWidget::SensorListWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent)
	, ui(new Ui::SensorListWidget())
{
	ui->setupUi(this);
	
	performStandardSetup(tr("Sensor List"));
	
	SensorModel *model = new SensorModel(this);
	ui->sensors->setModel(model);
	
	QTimer *timer = new QTimer(this);
	model->connect(timer, SIGNAL(timeout()), SLOT(update()));
	timer->start(50); // 20 FPS
}

SensorListWidget::~SensorListWidget()
{
	delete ui;
}
