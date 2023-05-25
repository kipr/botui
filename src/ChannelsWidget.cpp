#include "ChannelsWidget.h"

#include "ui_ChannelsWidget.h"

#include "ChannelConfigWidget.h"
#include "ChannelConfigWidgetFactory.h"
#include "ChannelSettingsWidget.h"
#include "CreateChannelDialog.h"
#include "RootController.h"

#include "CameraConfigModel.h"
#include <QDebug>
#include <QFileSystemModel>
#include <QItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>

#include <kipr/camera/camera.hpp>
#include <kipr/config/config.hpp>

using namespace kipr::config;

class ChannelItemDelegate : public QItemDelegate {
  public:
    ChannelItemDelegate(CameraConfigModel *model, QObject *parent = 0)
        : QItemDelegate(parent),
          m_model(model),
          m_hsv(QIcon(":/icons/color_wheel.png").pixmap(16, 16)),
          m_qr(QIcon(":/icons/qr.png").pixmap(16, 16)) {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const {
        QItemDelegate::paint(painter, option, index);

        const QString &type = m_model->channelType(index);

        QPixmap pixmap;
        if (type == CAMERA_CHANNEL_TYPE_HSV_KEY)
            pixmap = m_hsv;
        else if (type == CAMERA_CHANNEL_TYPE_QR_KEY)
            pixmap = m_qr;

        const QPoint right = option.rect.topRight();
        painter->drawPixmap(right.x() - 24,
                            right.y() + option.rect.height() / 2 - 8,
                            16,
                            16,
                            pixmap);
    }

  private:
    CameraConfigModel *m_model;
    QPixmap m_hsv;
    QPixmap m_qr;
};

ChannelsWidget::ChannelsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ChannelsWidget),
      m_model(new CameraConfigModel(this)) {
    ui->setupUi(this);
    performStandardSetup(tr("Channels"));
    ui->channels->setModel(m_model);
    ui->channels->setItemDelegate(new ChannelItemDelegate(m_model, this));

    connect(ui->add, SIGNAL(clicked()), SLOT(add()));
    connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->up, SIGNAL(clicked()), SLOT(up()));
    connect(ui->down, SIGNAL(clicked()), SLOT(down()));
    connect(ui->configure, SIGNAL(clicked()), SLOT(configure()));
    // connect(ui->options, SIGNAL(clicked()), SLOT(options()));
    connect(ui->channels->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            SLOT(updateOptions()));

    updateOptions();
}

ChannelsWidget::~ChannelsWidget() {
    if (!save()) qWarning() << "FAILED to save" << m_path;
    delete ui;
}

void ChannelsWidget::setFile(const QString &path) {
    m_path = path;
    Config *loaded = Config::load(m_path.toStdString());
    if (!loaded) return;
    m_model->setConfig(*loaded);
    delete loaded;
}

const QString &ChannelsWidget::file() const { return m_path; }

void ChannelsWidget::configure() {
    const QModelIndexList &indexes =
        ui->channels->selectionModel()->selection().indexes();
    if (indexes.size() != 1) return;
    ChannelConfigWidget *widget = ChannelConfigWidgetFactory::create(
        indexes[0], m_model->channelType(indexes[0]));
    if (!widget) return;
    widget->setConfig(m_model->channelConfig(indexes[0]));
    m_model->connect(widget,
                     SIGNAL(configChanged(QModelIndex, Config)),
                     SLOT(setChannelConfig(QModelIndex, Config)));
    RootController::ref().presentWidget(widget);
}

/*void ChannelsWidget::options() {
    const QModelIndexList &indexes = ui->channels->selectionModel()
        ->selection().indexes();
    if(indexes.size() != 1) return;
    ChannelSettingsWidget *widget = new ChannelSettingsWidget(device());
    widget->setIndex(indexes[0]);
    widget->setConfig(m_model->channelConfig(indexes[0]));
    m_model->connect(widget, SIGNAL(configChanged(QModelIndex, Config)),
        SLOT(setChannelConfig(QModelIndex, Config)));
    RootController::ref().presentWidget(widget);
}*/

void ChannelsWidget::up() {
    QItemSelectionModel *selModel = ui->channels->selectionModel();
    const QModelIndexList &indexes = selModel->selection().indexes();
    if (indexes.size() != 1) return;
    const int i = indexes[0].row();
    if (i - 1 < 0) return;
    m_model->swapChannels(i, i - 1);
    selModel->clearSelection();
    selModel->select(m_model->item(i - 1)->index(),
                     QItemSelectionModel::Select);
}

void ChannelsWidget::down() {
    QItemSelectionModel *selModel = ui->channels->selectionModel();
    const QModelIndexList &indexes = selModel->selection().indexes();
    if (indexes.size() != 1) return;
    const int i = indexes[0].row();
    if (i + 1 >= m_model->rowCount()) return;
    m_model->swapChannels(i, i + 1);
    selModel->clearSelection();
    selModel->select(m_model->item(i + 1)->index(),
                     QItemSelectionModel::Select);
}

void ChannelsWidget::add() {
    if (isFull()) return;
    CreateChannelDialog dialog;
    if (RootController::ref().presentDialog(&dialog) == QDialog::Rejected)
        return;
    m_model->addChannel(dialog.type());
    updateOptions();
}

void ChannelsWidget::remove() {
    const QModelIndexList &indexes =
        ui->channels->selectionModel()->selection().indexes();
    if (indexes.size() != 1) return;
    m_model->removeChannel(indexes[0].row());
    updateOptions();
}

void ChannelsWidget::updateOptions() {
    const QModelIndexList &indexes =
        ui->channels->selectionModel()->selection().indexes();
    const bool sel = indexes.size() == 1;
    ui->add->setEnabled(!isFull());
    ui->remove->setEnabled(sel);
    ui->up->setEnabled(sel && indexes[0].row() > 0);
    ui->down->setEnabled(sel && indexes[0].row() + 1 < m_model->rowCount());
    ui->configure->setEnabled(sel && ChannelConfigWidgetFactory::hasConfig(
                                         m_model->channelType(indexes[0])));
}

bool ChannelsWidget::isFull() { return m_model->rowCount() >= 4; }

bool ChannelsWidget::save() const {
    return m_model->config().save(m_path.toStdString());
}
