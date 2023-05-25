/* FIXME
#include "CameraInputSelectorWidget.h"
#include "ui_CameraInputSelectorWidget.h"

#include "CameraInputManager.h"

CameraInputSelectorWidget::CameraInputSelectorWidget(Device *device, QWidget
*parent) : StandardWidget(device, parent), ui(new Ui::CameraInputSelectorWidget)
{
    ui->setupUi(this);
    performStandardSetup(tr("Camera Input Selector"), false);

    CameraInputManager::ref().retain();
    connect(ui->source, SIGNAL(currentIndexChanged(int)),
SLOT(sourceIndexChanged(int))); ui->camera->setInvalid(false);
    ui->camera->connect(&CameraInputManager::ref(),
SIGNAL(frameAvailable(cv::Mat)), SLOT(updateImage(cv::Mat)));
    ui->source->setCurrentIndex((int)CameraInputManager::ref().source() - 1);
}

CameraInputSelectorWidget::~CameraInputSelectorWidget()
{
    CameraInputManager::ref().release();
    delete ui;
}

void CameraInputSelectorWidget::sourceIndexChanged(const int index)
{
    // TODO: This is pretty bad...
    CameraInputManager::Source source = (CameraInputManager::Source)(index + 1);
    CameraInputManager::ref().setSource(source);
}
*/
