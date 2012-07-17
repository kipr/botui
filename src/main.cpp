#include "BusyIndicator.h"
#include "RootController.h"
#include "MechanicalStyle.h"
#include "HomeWidget.h"
#include "StatusBar.h"
#include "KovanDevice.h"
#include "SimulatorDevice.h"
#include "NetworkManager.h"
#include <QApplication>
#include <QDir>

int main(int argc, char* argv[])
{
	QApplication::setStyle(new MechanicalStyle);
	QApplication app(argc, argv);
	QDir::setCurrent(QApplication::applicationDirPath());
	qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");
	
	NetworkManager network;
	
	Kovan::Device device;
	//RootController::ref().presentQml(QUrl::fromLocalFile("loading.qml"));
	RootController::ref().presentWidget(new HomeWidget(&device));
	
	return app.exec();
}