#include "SensorModel.h"

#include <kipr/accel/accel.hpp>
#include <kipr/analog/analog.hpp>
#include <kipr/button/button.h>
#include <kipr/button/button.hpp>
#include <kipr/digital/digital.hpp>
#include <kipr/gyro/gyro.hpp>
#include <kipr/magneto/magneto.hpp>
#include <kipr/sensor/logic.hpp>

using namespace kipr::sensor;

#include <math.h>

class SensorNameItem : public QStandardItem {
  public:
    SensorNameItem(SensorModel::SensorType type, const int port = -1)
        : m_type(type),
          m_port(port),
          m_typeName(typeName(type)),
          m_optionName(optionName(type)) {
        setText(m_typeName +
                (port >= 0 ? QObject::tr(" %1").arg(port) : QString()));
    }

    SensorModel::SensorType sensorType() const { return m_type; }

    int port() const { return m_port; }

    template <typename T> static SensorNameItem *cast(T *t) {
        return dynamic_cast<SensorNameItem *>(t);
    }

  private:
    static QString typeName(SensorModel::SensorType type) {
        switch (type) {
        case SensorModel::Analog:
            return QObject::tr("Analog Sensor");
        case SensorModel::Digital:
            return QObject::tr("Digital Sensor");
        case SensorModel::AccelX:
            return QObject::tr("Accelerometer X");
        case SensorModel::AccelY:
            return QObject::tr("Accelerometer Y");
        case SensorModel::AccelZ:
            return QObject::tr("Accelerometer Z");
#ifdef WOMBAT
        case SensorModel::GyroX:
            return QObject::tr("Gyrometer X");
        case SensorModel::GyroY:
            return QObject::tr("Gyrometer Y");
        case SensorModel::GyroZ:
            return QObject::tr("Gyrometer Z");
        case SensorModel::MagnetoX:
            return QObject::tr("Magnetometer X");
        case SensorModel::MagnetoY:
            return QObject::tr("Magnetometer Y");
        case SensorModel::MagnetoZ:
            return QObject::tr("Magnetometer Z");
        // case SensorModel::ButtonLeft: return QObject::tr("Button - Left");
        case SensorModel::Button:
            return QObject::tr("Button");

#endif
        default:
            break;
        }
        return QObject::tr("Unknown Sensor");
    }

    static QString optionName(SensorModel::SensorType type) {
        return QString();
    }

    SensorModel::SensorType m_type;
    int m_port;
    QString m_typeName;
    QString m_optionName;
};

class Updateable {
  public:
    virtual void update() = 0;

    template <typename T> static Updateable *cast(T *t) {
        return dynamic_cast<Updateable *>(t);
    }
};

template <typename T>
class SensorValueItem : public QStandardItem, public Updateable {
  public:
    SensorValueItem(Sensor<T> *const sensor, const bool owns)
        : m_sensor(sensor), m_owns(owns) {}

    ~SensorValueItem() {
        if (!m_owns) return;
        delete m_sensor;
    }

    virtual void update() { setText(QString("%1").arg(m_sensor->value())); }

    template <typename L> static SensorValueItem *cast(L *t) {
        return dynamic_cast<SensorValueItem *>(t);
    }

  private:
    Sensor<T> *const m_sensor;
    bool m_owns;
};

class SinSensor : public Sensor<double> {
  public:
    virtual double value() const {
        static double theta = 0;
        double ret = sin(theta);
        theta += 0.01;
        return ret;
    }
};

SensorModel::SensorModel(QObject *parent) : QStandardItemModel(parent) {
    populate();
}

SensorModel::~SensorModel() {}

SensorModel::SensorType SensorModel::type(const QModelIndex &index) const {
    QModelIndex nameIndex = index.sibling(index.row(), 0);
    SensorNameItem *const item = SensorNameItem::cast(itemFromIndex(nameIndex));
    return item ? item->sensorType() : SensorModel::Other;
}

void SensorModel::update() {
    for (int i = 0; i < rowCount(); ++i) {
        Updateable *updateable = Updateable::cast(item(i, 1));
        if (!updateable) continue;
        updateable->update();
    }
}

void SensorModel::populate() {
#ifdef WOMBAT
    for (unsigned int i = 0; i < 6; ++i)
        populateAnalog(i);
    for (unsigned int i = 0; i < 10; ++i)
        populateDigital(i);
#else
    for (unsigned int i = 0; i < 8; ++i)
        populateAnalog(i);
    for (unsigned int i = 0; i < 16; ++i)
        populateDigital(i);
#endif
    populateAccel();
#ifdef WOMBAT
    populateGyro();
    populateMagneto();
    populateButtons();
#endif
}

void SensorModel::populateAnalog(const unsigned char port) {
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::Analog, port)
              << new SensorValueItem<unsigned short>(
                     new kipr::analog::Analog(port), true));
}

void SensorModel::populateDigital(const unsigned char port) {
#ifdef WOMBAT
    appendRow(
        QList<QStandardItem *>()
        << new SensorNameItem(SensorModel::Digital, port)
        << new SensorValueItem<bool>(new kipr::digital::Digital(port), true));
#else
    appendRow(
        QList<QStandardItem *>()
        << new SensorNameItem(SensorModel::Digital, port)
        << new SensorValueItem<bool>(
               new logic::Not(new kipr::digital::Digital(port), true), true));
#endif
}

void SensorModel::populateAccel() {
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::AccelX)
              << new SensorValueItem<short>(new kipr::accel::AccelX(), true));
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::AccelY)
              << new SensorValueItem<short>(new kipr::accel::AccelY(), true));
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::AccelZ)
              << new SensorValueItem<short>(new kipr::accel::AccelZ(), true));
}

#ifdef WOMBAT
void SensorModel::populateGyro() {
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::GyroX)
              << new SensorValueItem<short>(new kipr::gyro::GyroX(), true));
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::GyroY)
              << new SensorValueItem<short>(new kipr::gyro::GyroY(), true));
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::GyroZ)
              << new SensorValueItem<short>(new kipr::gyro::GyroZ(), true));
}

void SensorModel::populateMagneto() {
    appendRow(
        QList<QStandardItem *>()
        << new SensorNameItem(SensorModel::MagnetoX)
        << new SensorValueItem<short>(new kipr::magneto::MagnetoX(), true));
    appendRow(
        QList<QStandardItem *>()
        << new SensorNameItem(SensorModel::MagnetoY)
        << new SensorValueItem<short>(new kipr::magneto::MagnetoY(), true));
    appendRow(
        QList<QStandardItem *>()
        << new SensorNameItem(SensorModel::MagnetoZ)
        << new SensorValueItem<short>(new kipr::magneto::MagnetoZ(), true));
}

void SensorModel::populateButtons() {
    appendRow(QList<QStandardItem *>()
              << new SensorNameItem(SensorModel::Button)
              << new SensorValueItem<bool>(
                     &kipr::button::Right,
                     false)); // can't instantiate new buttons since Id is
                              // private, so thus this doesn't belong to it
}

#endif
