#include "HsvChannelConfigWidget.h"
#include "ui_HsvChannelConfigWidget.h"

#include "CameraInputManager.h"
#include "NumpadDialog.h"
#include "RootController.h"

#include <kipr/camera/camera.h>

#include <QDebug>

HsvChannelConfigWidget::HsvChannelConfigWidget(const QModelIndex &index,
                                               QWidget *parent)
    : ChannelConfigWidget(index, parent),
      ui(new Ui::HsvChannelConfigWidget),
      m_numpad(new NumpadDialog("Enter Value")) {
    ui->setupUi(this);

    Config deviceConfig;
    deviceConfig.beginGroup(CAMERA_GROUP);
    deviceConfig.setValue(CAMERA_NUM_CHANNELS_KEY, 1);
    deviceConfig.beginGroup(
        (QString(CAMERA_CHANNEL_GROUP_PREFIX) + "0").toStdString());
    deviceConfig.setValue(CAMERA_CHANNEL_TYPE_KEY, CAMERA_CHANNEL_TYPE_HSV_KEY);
    deviceConfig.clearGroup();
    ui->camera->setConfig(&deviceConfig);

    visual();

    connect(ui->manualButton, SIGNAL(clicked()), SLOT(manual()));
    connect(ui->visualButton, SIGNAL(clicked()), SLOT(visual()));

    connect(ui->th, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));
    connect(ui->ts, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));
    connect(ui->tv, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));
    connect(ui->bh, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));
    connect(ui->bs, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));
    connect(ui->bv, SIGNAL(textChanged(QString)), SLOT(manualEntry(QString)));

    connect(ui->visual, SIGNAL(minChanged(QColor)), SLOT(visualChanged()));
    connect(ui->visual, SIGNAL(maxChanged(QColor)), SLOT(visualChanged()));

    connect(ui->camera,
            SIGNAL(pressed(kipr::camera::Image, int, int)),
            SLOT(imagePressed(kipr::camera::Image, int, int)));

    connect(ui->done, SIGNAL(clicked()), SLOT(done()));

    ui->th->setInputProvider(m_numpad);
    ui->ts->setInputProvider(m_numpad);
    ui->tv->setInputProvider(m_numpad);
    ui->bh->setInputProvider(m_numpad);
    ui->bs->setInputProvider(m_numpad);
    ui->bv->setInputProvider(m_numpad);

    ui->camera->setShowBbox(true);
    ui->camera->setNumBlobLabels(0);
}

HsvChannelConfigWidget::~HsvChannelConfigWidget() {
    delete ui;
    delete m_numpad;
}

void HsvChannelConfigWidget::refresh() {
    const Config &config = ChannelConfigWidget::config();
    int th = config.intValue("th") * 2;
    int ts = config.intValue("ts");
    int tv = config.intValue("tv");
    int bh = config.intValue("bh") * 2;
    int bs = config.intValue("bs");
    int bv = config.intValue("bv");

    qDebug() << "Values:";
    qDebug() << th << ts << tv;
    qDebug() << bh << bs << bv;

    if (th == bh) {
        th += 5;
        th %= 360;
        bh -= 5;
        if (bh < 0) bh += 360;
    }

    if (ts == bs) {
        ts += 5;
        ts = ts > 255 ? 255 : ts;
        bs -= 5;
        bs = bs < 0 ? 0 : bs;
    }

    if (tv == bv) {
        tv += 5;
        tv = tv > 255 ? 255 : tv;
        bv -= 5;
        bv = bv < 0 ? 0 : bv;
    }

    // Visual
    ui->visual->setMax(QColor::fromHsv(th, ts, tv));
    ui->visual->setMin(QColor::fromHsv(bh, bs, bv));

    // Manual
    ui->th->setText(QString::number(th));
    ui->ts->setText(QString::number(ts));
    ui->tv->setText(QString::number(tv));
    ui->bh->setText(QString::number(bh));
    ui->bs->setText(QString::number(bs));
    ui->bv->setText(QString::number(bv));

    ui->camera->setChannelConfig(config, 0);
}

void HsvChannelConfigWidget::visual() {
    setUpdatesEnabled(false);
    ui->visualButton->setEnabled(false);
    ui->manualButton->setEnabled(true);
    ui->manual->hide();
    ui->visual->show();
    setUpdatesEnabled(true);
}

void HsvChannelConfigWidget::manual() {
    setUpdatesEnabled(false);
    ui->manualButton->setEnabled(false);
    ui->visualButton->setEnabled(true);
    ui->visual->hide();
    ui->manual->show();
    setUpdatesEnabled(true);
}

void HsvChannelConfigWidget::manualEntry(const QString &number) {
    QObject *from = sender();
    if (!from) return;

    int num = number.toInt();
    if (num < 0) num = 0;

    Config config = ChannelConfigWidget::config();

    if (from == ui->th) {
        config.setValue("th", qMin(num, 359) >> 1);
    }
    else if (from == ui->ts) {
        config.setValue("ts", qMin(num, 255));
    }
    else if (from == ui->tv) {
        config.setValue("tv", qMin(num, 255));
    }
    else if (from == ui->bh) {
        config.setValue("bh", qMin(num, 359) >> 1);
    }
    else if (from == ui->bs) {
        config.setValue("bs", qMin(num, 255));
    }
    else if (from == ui->bv) {
        config.setValue("bv", qMin(num, 255));
    }

    blockChildSignals(true);
    setConfig(config);
    blockChildSignals(false);
}

void HsvChannelConfigWidget::visualChanged() {
    const QColor &max = ui->visual->max();
    const QColor &min = ui->visual->min();

    Config config = ChannelConfigWidget::config();
    config.setValue("th", max.hue() / 2);
    config.setValue("ts", max.saturation());
    config.setValue("tv", max.value());
    config.setValue("bh", min.hue() / 2);
    config.setValue("bs", min.saturation());
    config.setValue("bv", min.value());

    blockChildSignals(true);
    setConfig(config);
    blockChildSignals(false);
}

void HsvChannelConfigWidget::imagePressed(const kipr::camera::Image &image,
                                          const int &x,
                                          const int &y) {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    switch (image.getType()) {
    case kipr::camera::Image::Type::Bgr888: {
        const unsigned char *const data =
            image.getData() + y * image.getStride() + x * 3;
        r = data[2];
        g = data[1];
        b = data[0];
        break;
    }
    case kipr::camera::Image::Type::Rgb888: {
        const unsigned char *const data =
            image.getData() + y * image.getStride() + x * 3;
        r = data[0];
        g = data[1];
        b = data[2];
        break;
    }
    default: {
        qWarning() << "Unsupported image format";
        return;
    }
    }

    const QColor c(r, g, b);

    int th = (c.hue() / 2 + 5) % 180;
    int ts = c.saturation() + 40;
    int tv = c.value() + 40;
    int bh = c.hue() / 2 - 5;
    int bs = c.saturation() - 40;
    int bv = c.value() - 40;

    if (ts > 255) ts = 255;
    if (tv > 255) tv = 255;

    if (bh < 0) bh += 180;
    if (bs < 0) bs = 0;
    if (bv < 0) bv = 0;

    qDebug() << "touch bh: " << bh;

    Config config = ChannelConfigWidget::config();
    config.setValue("th", th);
    config.setValue("ts", ts);
    config.setValue("tv", tv);
    config.setValue("bh", bh);
    config.setValue("bs", bs);
    config.setValue("bv", bv);

    blockChildSignals(true);
    setConfig(config);
    blockChildSignals(false);
}

void HsvChannelConfigWidget::done() { RootController::ref().dismissWidget(); }

void HsvChannelConfigWidget::blockChildSignals(const bool &block) {
    ui->th->blockSignals(block);
    ui->ts->blockSignals(block);
    ui->tv->blockSignals(block);
    ui->bh->blockSignals(block);
    ui->bs->blockSignals(block);
    ui->bv->blockSignals(block);
    ui->visual->blockSignals(block);
}
