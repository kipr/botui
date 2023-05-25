#include "BusyIndicator.h"
#include "CursorManager.h"
#include "FactoryWidget.h"
#include "FirstRunWizard.h"
#include "GuiSettingsWidget.h"
#include "HomeWidget.h"
#include "KovanSerialBridge.h"
#include "MechanicalStyle.h"
#include "NetworkManager.h"
#include "NetworkSettingsWidget.h"
#include "RootController.h"
#include "SettingsProvider.h"
#include "StatusBar.h"
#include "TestWizard.h"
#include "WombatDevice.h"
#include <QApplication>
#include <QDir>

#include <QFontDatabase>
#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication::setStyle(new MechanicalStyle);
    QApplication::setOrganizationName("KIPR");
    QApplication::setApplicationName("botui");

    QApplication app(argc, argv);

    QTranslator translator;
    const QString trFile =
        "botui_" + QSettings().value("locale", "en").toString().left(2);
    qDebug() << "Trying to use translation file " << trFile;
    if (trFile != "botui_en" && translator.load(trFile, "/etc/botui/locale/")) {
        qDebug() << "Successfully loaded translation file " << trFile;
        app.installTranslator(&translator);
    }

    QDir::setCurrent(QApplication::applicationDirPath());
    qmlRegisterType<BusyIndicator>("ZapBsComponents", 1, 0, "BusyIndicator");

    QFontDatabase::addApplicationFont(":/fonts/DejaVuSans-ExtraLight.ttf");
    QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
    QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");

    srand(time(NULL));

    Wombat::Device device;
    CursorManager::ref().setDevice(&device);
#ifdef QT_DBUS_LIB
    KovanSerialBridge::ref().init(&device);
    NetworkManager::ref().init(&device);
#endif

    SettingsProvider *const settings = device.settingsProvider();
    const bool fullscreen =
        settings && settings->value("fullscreen", true).toBool();
    RootController::ref().setFullscreen(fullscreen);
    // GuiSettingsWidget::updateStyle(&device);

    RootController::ref().presentWidget(new HomeWidget(&device));

    return app.exec();
}
