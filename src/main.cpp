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
#include <QSettings>
#include <QTranslator>

int main(int argc, char* argv[])
{ 
	QApplication::setStyle(new MechanicalStyle);
	QApplication app(argc, argv);
  
  QTranslator translator;
  const QString trFile = "link_" + QSettings().value("locale", "en").toString();
  if(trFile != "en" && translator.load(trFile, "/etc/botui/locale/"))
    app.installTranslator(&translator);
	
	QDir::setCurrent(QApplication::applicationDirPath());
	qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");
	
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans-ExtraLight.ttf");
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");
	
	srand(time(NULL));
	
	Kovan::Device device;
	CursorManager::ref().setDevice(&device);
#ifdef QT_DBUS_LIB
  KovanSerialBridge::ref().init(&device);
  NetworkManager::ref();
#endif
	GuiSettingsWidget::updateStyle(&device);
	RootController::ref().presentWidget(new HomeWidget(&device));

	return app.exec();
}