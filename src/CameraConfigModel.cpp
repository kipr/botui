
#include "CameraConfigModel.h"

#include <kipr/camera/camera.hpp>

#include <QDebug>
#include <QStandardItem>

class ChannelItem : public QStandardItem {
  public:
    ChannelItem(const unsigned &i,
                const QString &type,
                const Config &config = Config())
        : QStandardItem(""), m_i(i), m_type(type), m_config(config) {
        setText(title());
        setSizeHint(QSize(0, 22));
        setEditable(false);
    }

    void setI(const unsigned &i) {
        m_i = i;
        setText(title());
    }

    const unsigned &i() const { return m_i; }

    template <typename T> static ChannelItem *cast(T *t) {
        return dynamic_cast<ChannelItem *>(t);
    }

    void setConfig(const Config &config) { m_config = config; }

    const Config &config() const { return m_config; }

    void setChannelType(const QString &type) {
        m_type = type;
        setText(m_type);
    }

    const QString &channelType() const { return m_type; }

  private:
    QString title() const { return QObject::tr("Channel %1").arg(m_i); }

    unsigned m_i;
    QString m_type;
    Config m_config;
};

CameraConfigModel::CameraConfigModel(QObject *parent)
    : QStandardItemModel(parent) {
    setColumnCount(2);
}

CameraConfigModel::~CameraConfigModel() {}

const QString &CameraConfigModel::channelType(const QModelIndex &index) const {
    ChannelItem *item = ChannelItem::cast(itemFromIndex(index));
    if (!item) return m_blank;
    return item->channelType();
}

Config CameraConfigModel::channelConfig(const QModelIndex &index) const {
    ChannelItem *item = ChannelItem::cast(itemFromIndex(index));
    if (!item) return Config();
    return item->config();
}

void CameraConfigModel::setChannelConfig(const QModelIndex &index,
                                         const Config &config) {
    ChannelItem *item = ChannelItem::cast(itemFromIndex(index));
    if (!item) return;
    item->setConfig(config);
    updateConfig();
}

void CameraConfigModel::setConfig(const Config &config) {
    clear();
    m_config = config;
    m_config.clearGroup();
    m_config.beginGroup(CAMERA_GROUP);
    int numChannels = m_config.intValue(CAMERA_NUM_CHANNELS_KEY);
    for (int i = 0; i < numChannels; ++i) {
        char buffer[32];
        memset(buffer, 0, 32);
        sprintf(buffer, "%s%d", CAMERA_CHANNEL_GROUP_PREFIX, i);
        m_config.beginGroup(buffer);
        QString type = QString::fromStdString(
            m_config.stringValue(CAMERA_CHANNEL_TYPE_KEY));
        appendRow(new ChannelItem(i, type, m_config.values()));
        m_config.endGroup();
    }
    m_config.endGroup();
}

const Config &CameraConfigModel::config() const { return m_config; }

void CameraConfigModel::addChannel(const QString &type) {
    appendRow(new ChannelItem(rowCount(), type));
    updateConfig();
}

void CameraConfigModel::removeChannel(const int &i) {
    removeRow(i);
    updateConfig();
    for (int j = i; j < rowCount(); ++j) {
        ChannelItem *item = ChannelItem::cast(CameraConfigModel::item(j));
        if (!item) continue;
        item->setI(item->i() - 1);
    }
}

void CameraConfigModel::swapChannels(const int &i, const int &j) {
    if (i < 0 || j < 0 || i >= rowCount() || j >= rowCount()) return;
    int a = qMin(i, j);
    int b = qMax(i, j);
    QList<QStandardItem *> bItems = takeRow(b);
    QList<QStandardItem *> aItems = takeRow(a);
    insertRow(a, bItems);
    insertRow(b, aItems);
    ChannelItem *newA = ChannelItem::cast(bItems[0]);
    ChannelItem *newB = ChannelItem::cast(aItems[0]);
    newA->setI(a);
    newB->setI(b);
    updateConfig();
}

void CameraConfigModel::updateConfig() {
    m_config.clear();
    m_config.beginGroup(CAMERA_GROUP);
    m_config.setValue(CAMERA_NUM_CHANNELS_KEY, rowCount());
    for (int i = 0; i < rowCount(); ++i) {
        ChannelItem *channelItem = ChannelItem::cast(item(i, 0));
        if (!channelItem) {
            qWarning() << "Failed to cast channel item";
            continue;
        }

        char buffer[32];
        sprintf(buffer, "%s%d", CAMERA_CHANNEL_GROUP_PREFIX, channelItem->i());
        m_config.beginGroup(buffer);
        m_config.setValue(CAMERA_CHANNEL_TYPE_KEY,
                          channelItem->channelType().toStdString());
        m_config.addValues(channelItem->config());
        m_config.endGroup();
    }
    m_config.endGroup();
}
