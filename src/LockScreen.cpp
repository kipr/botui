#include "LockScreen.h"
#include "RootController.h"

void LockScreen::lock() {
    RootController::ref().presentQml(QUrl("qrc:///qml/lock.qml"));
}