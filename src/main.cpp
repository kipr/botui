#include "BusyIndicator.h"
#include "RootController.h"
#include "RootWindow.h"
#include "MechanicalStyle.h"
#include "HomeWidget.h"
#include "StatusBar.h"
#include "WombatDevice.h"
#include "FirstRunWizard.h"
#include "FactoryWidget.h"
#include "NetworkManager.h"
#include "GuiSettingsWidget.h"
#include "ScreenInversion.h"
#include "TestWizard.h"
#include "CursorManager.h"
#include "NetworkSettingsWidget.h"
#include <QApplication>
#include <QDir>

#include <QFont>
#include <QFontDatabase>
#include <QSettings>
#include <QStringList>
#include <QTranslator>
#include <QTimer>

int main(int argc, char* argv[])
{ 
	QApplication::setStyle(new MechanicalStyle);
  QApplication::setOrganizationName("KIPR");
  QApplication::setApplicationName("botui");
  
	QApplication app(argc, argv);
	QString screenRecoveryError;
	const bool screenRecovered = ScreenInversion::recoverPending(&screenRecoveryError);
	if (!screenRecovered)
		qWarning() << "Unable to recover pending screen inversion:" << screenRecoveryError;
	QTimer screenSyncTimer;
	screenSyncTimer.setInterval(2000);
	QObject::connect(&screenSyncTimer, &QTimer::timeout, &app, [&screenSyncTimer]() {
		QString error;
		if (ScreenInversion::synchronizeConfiguredOutput(&error))
			screenSyncTimer.stop();
	});
	QTimer screenRecoveryTimer;
	screenRecoveryTimer.setInterval(2000);
	QObject::connect(&screenRecoveryTimer, &QTimer::timeout, &app, [&screenRecoveryTimer, &screenSyncTimer]() {
		QString error;
		if (ScreenInversion::recoverPending(&error)) {
			screenRecoveryTimer.stop();
			screenSyncTimer.start();
		}
	});
	if (!screenRecovered)
		screenRecoveryTimer.start();
	else
		screenSyncTimer.start();
  
  QTranslator translator;
  const QString trFile = "botui_" + QSettings().value("locale", "en").toString().left(2);
  qDebug() << "Trying to use translation file " << trFile;
  if(trFile != "botui_en" && translator.load(trFile, "/etc/botui/locale/"))
  {
    qDebug() << "Successfully loaded translation file " << trFile;
    app.installTranslator(&translator);
  }
	
	QDir::setCurrent(QApplication::applicationDirPath());
	qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");
	
	// const int sansFontId = QFontDatabase::addApplicationFont(":/fonts/DejaVuSans-ExtraLight.ttf");
	// const QString sansStyle = "ExtraLight";
	// To test regular DejaVu Sans, comment out the two lines above and uncomment these:
	const int sansFontId = QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
	const QString sansStyle = "Book";
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");

	const QStringList sansFamilies = QFontDatabase::applicationFontFamilies(sansFontId);
	if (!sansFamilies.isEmpty()) {
		QFont appFont = QApplication::font();
		appFont.setFamily(sansFamilies.constFirst());
		appFont.setStyleName(sansStyle);
		QApplication::setFont(appFont);
	} else {
		qWarning() << "Unable to load bundled DejaVu Sans font";
	}
	
	srand(time(NULL));
	
	Wombat::Device device;
	CursorManager::ref().setDevice(&device);
	RootWindow rootWindow;
	RootController::ref().initialize(&rootWindow, new HomeWidget(&device));
#ifdef QT_DBUS_LIB
  NetworkManager::ref().init(&device);
#endif

	rootWindow.showFullScreen();

	return app.exec();
}
