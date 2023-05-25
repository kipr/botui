#include "Options.h"

#ifdef NETWORK_ENABLED

#include "NetworkStatusWidget.h"

#include "NetworkManager.h"

#include <QDebug>
#include <QPainter>
#include <QTime>
#include <QTimer>

#ifdef WOMBAT
#include <fstream>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

void setWiFiStatusLED(bool on) {
#ifdef WOMBAT
    std::fstream fs;
    fs.open("/sys/class/leds/botball:D11:blue/brightness", std::fstream::out);

    if (on) {
        fs << "1";
    }
    else {
        fs << "0";
    }

    fs.close();
#endif
}

NetworkStatusWidget::NetworkStatusWidget(QWidget *parent) : QWidget(parent) {
    constrain();
    connect(&NetworkManager::ref(),
            SIGNAL(stateChanged(NetworkManager::State, NetworkManager::State)),
            SLOT(networkStateChanged()));
    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(update()));
    updateTimer->start(10000);
}

// #ifdef WOMBAT

// // bool NetworkStatusWidget::isNetworkUp(const std::string networkName)
// // {
// // 	bool wifi_up = false;

// // 	struct ifreq ifr;

// // 	memset(&ifr, 0, sizeof(ifr));
// // 	strcpy(ifr.ifr_name, networkName.c_str());

// // 	int dummy_fd = socket(AF_INET, SOCK_DGRAM, 0);

// // 	if (ioctl(dummy_fd, SIOCGIFFLAGS, &ifr) != -1)
// // 	{
// // 		wifi_up = (ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) == (IFF_UP |
// IFF_RUNNING);
// // 	}
// // 	else
// // 	{
// // 		// an error checking network status
// // 		wifi_up = false;
// // 	}

// // 	return wifi_up;
// // }

// #endif

void NetworkStatusWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const int w = width();
    const int h = height();
    const int ellipseSize = w / 6;
    const int startAngle = 30 * 16;
    const int spanAngle = 120 * 16;

    static const QColor green = QColor(50, 150, 50);
    static const QColor red = QColor(250, 100, 100);
    static const QColor orange = QColor(250, 127, 0);

#ifdef WOMBAT

    const bool off = !NetworkManager::ref().isOn();
    QColor color = off ? red : green;
    if (!off && NetworkManager::ref().state() < NetworkManager::Activated) {
        color = orange;
    }
#endif

    painter.setPen(QPen(color, w / 10));
    painter.setBrush(color);

    if (off) {
        const int min = w > h ? h : w;
        const float minOff = min / 3.0;
        painter.drawLine(w / 2.0 - minOff,
                         h / 2.0 - minOff,
                         w / 2.0 + minOff,
                         h / 2.0 + minOff);
        painter.drawLine(w / 2.0 - minOff,
                         h / 2.0 + minOff,
                         w / 2.0 + minOff,
                         h / 2.0 - minOff);
        return;
    }

#ifdef WOMBAT
    const float percentage = 100.0f;
#else
    const float percentage = NetworkManager::ref().active().strength() / 100.0;
#endif

    QRectF rectangle(0, h / 5.0, w, h);
    if (percentage > 0.66) {
        painter.drawArc(rectangle, startAngle, spanAngle);
    }
    rectangle = QRectF(w / 5.0, 2.0 * h / 5.0, 3.0 * w / 5.0, h / 1.5);
    if (percentage > 0.33) {
        painter.drawArc(rectangle, startAngle, spanAngle);
    }
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(w / 2.0 - ellipseSize / 2.0,
                        3.5 * h / 5.0 - ellipseSize / 2.0,
                        ellipseSize,
                        ellipseSize);
}

void NetworkStatusWidget::networkStateChanged() { update(); }

void NetworkStatusWidget::constrain() {
    setMinimumSize(24, 20);
    setMaximumSize(24, 20);
}

#endif
