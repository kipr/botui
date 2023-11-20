#include "CreateSensorModel.h"
#include <kipr/create3/client/client.h>
#include <kipr/analog/analog.hpp>
#include <kipr/digital/digital.hpp>
#include <kipr/accel/accel.hpp>
#include <kipr/gyro/gyro.hpp>
#include <kipr/magneto/magneto.hpp>
#include <kipr/sensor/logic.hpp>
#include <kipr/button/button.h>
#include <kipr/button/button.hpp>
#include <math.h>
#include <kipr/sensor/logic.hpp>
#include "SensorModel.h"
#include "kipr/create3/client/BumpSensor.hpp"

using namespace kipr::sensor;
class SensorNameItem : public QStandardItem
{
public:
    SensorNameItem(CreateSensorModel::SensorType type, const int port = -1)
        : m_type(type), m_port(port), m_typeName(typeName(type)), m_optionName(optionName(type))
    {
        setText(m_typeName);
    }

    CreateSensorModel::SensorType sensorType() const
    {
        return m_type;
    }

    int port() const
    {
        return m_port;
    }

    template <typename T>
    static SensorNameItem *cast(T *t)
    {
        return dynamic_cast<SensorNameItem *>(t);
    }

private:
    static QString typeName(CreateSensorModel::SensorType type)
    {
        switch (type)
        {
        case CreateSensorModel::Bump0:
            return QObject::tr("Left Bumper");
        case CreateSensorModel::Bump1:
            return QObject::tr("Left Front Bumper");
        case CreateSensorModel::Bump2:
            return QObject::tr("Center Front Bumper");
        case CreateSensorModel::Bump3:
            return QObject::tr("Right Front Bumper");
        case CreateSensorModel::Bump4:
            return QObject::tr("Right Bumper");
        case CreateSensorModel::Cliff0:
            return QObject::tr("Left Side Cliff");
        case CreateSensorModel::Cliff1:
            return QObject::tr("Left Front Cliff");
        case CreateSensorModel::Cliff2:
            return QObject::tr("Right Front Cliff");
        case CreateSensorModel::Cliff3:
            return QObject::tr("Right Side Cliff");
        case CreateSensorModel::IR0:
            return QObject::tr("Left Side IR");
        case CreateSensorModel::IR1:
            return QObject::tr("Left IR");
        case CreateSensorModel::IR2:
            return QObject::tr("Left Front IR");
        case CreateSensorModel::IR3:
            return QObject::tr("Left Center IR");
        case CreateSensorModel::IR4:
            return QObject::tr("Right Center IR");
        case CreateSensorModel::IR5:
            return QObject::tr("Right Front IR");
        case CreateSensorModel::IR6:
            return QObject::tr("Right IR");

        default:
            break;
        }
        return QObject::tr("Unknown Sensor");
    }

    static QString optionName(CreateSensorModel::SensorType type)
    {
        return QString();
    }

    CreateSensorModel::SensorType m_type;
    int m_port;
    QString m_typeName;
    QString m_optionName;
};

class Updateable
{
public:
    virtual void update() = 0;

    template <typename T>
    static Updateable *cast(T *t)
    {
        return dynamic_cast<Updateable *>(t);
    }
};

template <typename T>
class SensorValueItem : public QStandardItem, public Updateable
{
public:
    SensorValueItem(CreateSensorModel::SensorMethod name, const unsigned char port, const bool owns)
        : m_value(typeName(name,port)), m_owns(owns)
    {
    }

    ~SensorValueItem()
    {
    }

    virtual void update()
    {
        setText(QString("%1").arg(m_value));
    }

    template <typename L>
    static SensorValueItem *cast(L *t)
    {
        return dynamic_cast<SensorValueItem *>(t);
    }

private:

    bool m_owns;
    int m_value;

    static int typeName(CreateSensorModel::SensorMethod name, const unsigned char port)
    {
        switch (name)
        {
        case CreateSensorModel::Bump:
            return create3_sensor_bump(port);
        case CreateSensorModel::Cliff:
            return create3_sensor_cliff(port);
        case CreateSensorModel::IR:
            return create3_sensor_ir(port);
        default:
            break;
        }
    }
};

class SinSensor : public Sensor<double>
{
public:
    virtual double value() const
    {
        static double theta = 0;
        double ret = sin(theta);
        theta += 0.01;
        return ret;
    }
};

CreateSensorModel::CreateSensorModel(QObject *parent)
    : QStandardItemModel(parent)
{
    populate();
}

CreateSensorModel::~CreateSensorModel()
{
}

CreateSensorModel::SensorType CreateSensorModel::type(const QModelIndex &index) const
{
    QModelIndex nameIndex = index.sibling(index.row(), 0);
    SensorNameItem *const item = SensorNameItem::cast(itemFromIndex(nameIndex));
    return item ? item->sensorType() : CreateSensorModel::Other;
}

void CreateSensorModel::update()
{
    for (int i = 0; i < rowCount(); ++i)
    {
        Updateable *updateable = Updateable::cast(item(i, 1));
        if (!updateable)
            continue;
        updateable->update();
    }
}

void CreateSensorModel::populate()
{
    for (unsigned int i = 0; i < 6; ++i)
        populateBump(i);
    //for (unsigned int i = 0; i < 5; ++i)
    //     populateCliff(i);
    // for (unsigned int i = 0; i < 8; ++i)
    //     populateIR(i);
}

void CreateSensorModel::populateBump(const unsigned char port)
{

    switch (port)
    {
    case 0:
        appendRow(QList<QStandardItem *>()
                  << new SensorNameItem(CreateSensorModel::Bump0, port)
                  << new SensorValueItem<unsigned short>(CreateSensorModel::Bump, port,true));
        break;
    // case 1:
    //     appendRow(QList<QStandardItem *>()
    //               << new SensorNameItem(CreateSensorModel::Bump1, port)
    //               << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
    //     break;
    // case 2:
    //     appendRow(QList<QStandardItem *>()
    //               << new SensorNameItem(CreateSensorModel::Bump2, port)
    //               << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
    //     break;
    // case 3:
    //     appendRow(QList<QStandardItem *>()
    //               << new SensorNameItem(CreateSensorModel::Bump3, port)
    //               << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
    //     break;
    // case 4:
    //     appendRow(QList<QStandardItem *>()
    //               << new SensorNameItem(CreateSensorModel::Bump4, port)
    //               << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
    //     break;
    default:
        break;
    }
}

// void CreateSensorModel::populateCliff(const unsigned char port)
// {

//     switch (port)
//     {
//     case 0:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::Cliff0, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 1:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::Cliff1, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 2:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::Cliff2, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 3:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::Cliff3, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;

//     default:
//         break;
//     }
// }

// void CreateSensorModel::populateIR(const unsigned char port)
// {
//     switch (port)
//     {
//     case 0:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR0, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 1:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR1, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 2:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR2, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 3:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR3, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 4:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR4, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 5:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR5, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     case 6:
//         appendRow(QList<QStandardItem *>()
//                   << new SensorNameItem(CreateSensorModel::IR6, port)
//                   << new SensorValueItem<unsigned short>(new kipr::analog::Analog(port), true));
//         break;
//     default:
//         break;
//     }
// }
