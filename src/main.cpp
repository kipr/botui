#include "BusyIndicator.h"
#include "RootController.h"
#include "MechanicalStyle.h"
#include "HomeWidget.h"
#include "StatusBar.h"
#include "KovanDevice.h"
#include <kiss-compiler/CompilerPluginManager.h>
#include "SimulatorDevice.h"
#include "FirstRunWizard.h"
#include <QApplication>
#include <QDir>

int main(int argc, char* argv[])
{
	QApplication::setStyle(new MechanicalStyle);
	QApplication app(argc, argv);
	app.setOverrideCursor(QCursor(Qt::BlankCursor));
	QDir::setCurrent(QApplication::applicationDirPath());
	qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");
	
	CompilerPluginManager::ref().loadAll();
	
	Kovan::Device device;
	//RootController::ref().presentQml(QUrl::fromLocalFile("loading.qml"));
	RootController::ref().presentWidget(new HomeWidget(&device));
	RootController::ref().presentDialog(new FirstRunWizard(&device));
	
	return app.exec();
}