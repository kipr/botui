#include "CameraLiveWidget.h"

#include <QDebug>

#include "ui_CameraLiveWidget.h"
#include "Device.h"
#include "ChannelConfigurationsModel.h"

CameraLiveWidget::CameraLiveWidget(Device *device, QWidget *parent)
  : StandardWidget(device, parent),
  ui(new Ui::CameraLiveWidget),
  m_model(new ChannelConfigurationsModel(this))
{
  ui->setupUi(this);
  performStandardSetup(tr("Camera"), false);
  
  ui->config->setModel(m_model);
  ui->config->setRootModelIndex(m_model->index(m_model->rootPath()));
  ui->config->setCurrentIndex(m_model->defaultConfiguration().row());
  
  connect(ui->config, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
  
  ui->camera->setTrackBlobs(true);
}

CameraLiveWidget::~CameraLiveWidget()
{
  delete ui;
}

void CameraLiveWidget::currentIndexChanged(const int &index)
{
  const QString path = m_model->filePath(m_model->index(index,
    0, ui->config->rootModelIndex()));
  Config *conf = Config::load(path.toStdString());
  ui->camera->setConfig(conf);
  delete conf;
}
