#include "Calibrate.h"

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

#define CALIB_FILE "/etc/X11/xorg.conf.d/99-calibration.conf"

bool Calibrate::calibrate() {
    QProcess xcalib;
    xcalib.setProcessChannelMode(QProcess::MergedChannels);
    xcalib.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    xcalib.start("xinput_calibrator",
                 QStringList() << "--output-type"
                               << "xorg.conf.d");
    xcalib.waitForStarted(2000);
    if (xcalib.state() != QProcess::Running) {
        qWarning() << "Couldn't start calibrator";
        return false;
    }

    // Wait until calibrator exits
    xcalib.waitForFinished(-1);

    QByteArray output = xcalib.readAll();

    // The output has a line with the config file path on it.
    // the data we want is after this line.
    qDebug() << output;
    int i = output.indexOf(CALIB_FILE);
    if (i < 0) {
        qWarning() << "xinput has malformed output?";
        return false;
    }
    i = output.indexOf("\n", i);
    if (i < 0) {
        qWarning() << "xinput has malformed output?";
        return false;
    }

    QDir().mkpath(QFileInfo(CALIB_FILE).absolutePath());
    QFile file(CALIB_FILE);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to open " CALIB_FILE;
        return false;
    }
    file.write(output.mid(i + 1));
    file.close();

    return true;
}

Calibrate::Calibrate() {}
