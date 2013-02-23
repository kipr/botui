#include "BusyIndicator.h"
#include "RootController.h"
#include "MechanicalStyle.h"
#include "HomeWidget.h"
#include "StatusBar.h"
#include "KovanDevice.h"
#include "SimulatorDevice.h"
#include "SimulatorDevice.h"
#include "FirstRunWizard.h"
#include "FactoryWidget.h"
#include "NetworkManager.h"
#include "GuiSettingsWidget.h"
#include "TestWizard.h"
#include "KovanSerialBridge.h"
#include "CursorManager.h"

#include <QApplication>
#include <QDir>

#include <QFontDatabase>
#include <QDebug>

int main(int argc, char* argv[])
{
	QApplication::setStyle(new MechanicalStyle);
	QApplication app(argc, argv);
	
	QDir::setCurrent(QApplication::applicationDirPath());
	qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");
	
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans-ExtraLight.ttf");
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");
	
	srand(time(NULL));
	
	Kovan::Device device;
	CursorManager::ref().setDevice(&device);
	KovanSerialBridge::ref().init(&device);
	GuiSettingsWidget::updateStyle(&device);
	
	NetworkManager::ref();
	RootController::ref().presentWidget(new HomeWidget(&device));
	
	
	return app.exec();
}