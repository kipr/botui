#include "Create3SensorModel.h"
#include <kipr/create3/client/client.h>
#include <math.h>
#include <kipr/create3/client/BumpSensor.hpp>

class Create3SensorNameItem : public QStandardItem
{
public:
    Create3SensorNameItem(Create3SensorModel::SensorType type, const int port = -1)
        : m_type(type), m_port(port), m_typeName(typeName(type)), m_optionName(optionName(type))
    {
        setText(m_typeName);
    }

    Create3SensorModel::SensorType sensorType() const
    {
        return m_type;
    }

    int port() const
    {
        return m_port;
    }

    template <typename T>
    static Create3SensorNameItem *cast(T *t)
    {
        return dynamic_cast<Create3SensorNameItem *>(t);
    }

private:
    static QString typeName(Create3SensorModel::SensorType type)
    {
        switch (type)
        {
        case Create3SensorModel::Bump0:
            return QObject::tr("Left Bumper");
        case Create3SensorModel::Bump1:
            return QObject::tr("Left Front Bumper");
        case Create3SensorModel::Bump2:
            return QObject::tr("Center Front Bumper");
        case Create3SensorModel::Bump3:
            return QObject::tr("Right Front Bumper");
        case Create3SensorModel::Bump4:
            return QObject::tr("Right Bumper");
        case Create3SensorModel::Cliff0:
            return QObject::tr("Left Side Cliff");
        case Create3SensorModel::Cliff1:
            return QObject::tr("Left Front Cliff");
        case Create3SensorModel::Cliff2:
            return QObject::tr("Right Front Cliff");
        case Create3SensorModel::Cliff3:
            return QObject::tr("Right Side Cliff");
        case Create3SensorModel::IR0:
            return QObject::tr("Left Side IR");
        case Create3SensorModel::IR1:
            return QObject::tr("Left IR");
        case Create3SensorModel::IR2:
            return QObject::tr("Left Front IR");
        case Create3SensorModel::IR3:
            return QObject::tr("Left Center IR");
        case Create3SensorModel::IR4:
            return QObject::tr("Right Center IR");
        case Create3SensorModel::IR5:
            return QObject::tr("Right Front IR");
        case Create3SensorModel::IR6:
            return QObject::tr("Right IR");

        default:
            break;
        }
        return QObject::tr("Unknown Sensor");
    }

    static QString optionName(Create3SensorModel::SensorType type)
    {
        return QString();
    }

    Create3SensorModel::SensorType m_type;
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
class Create3SensorValueItem : public QStandardItem, public Updateable
{
public:
    Create3SensorValueItem(Create3SensorModel::SensorMethod name, const unsigned char port, const bool owns)
        : m_value(typeName(name, port)), m_owns(owns)
    {
    }

    ~Create3SensorValueItem()
    {
    }

    virtual void update()
    {
        setText(QString("%1").arg(m_value));
    }

    template <typename L>
    static Create3SensorValueItem *cast(L *t)
    {
        return dynamic_cast<Create3SensorValueItem *>(t);
    }

private:
    bool m_owns;
    int m_value;

    static int typeName(Create3SensorModel::SensorMethod name, int port)
    {
        switch (name)
        {
        case Create3SensorModel::Bump:
            return create3_sensor_bump(port);
        case Create3SensorModel::Cliff:
            return create3_sensor_cliff(port);
        case Create3SensorModel::IR:
            return create3_sensor_ir(port);
        default:
            break;
        }
    }
};

// class SinSensor : public Sensor<double>
// {
// public:
//     virtual double value() const
//     {
//         static double theta = 0;
//         double ret = sin(theta);
//         theta += 0.01;
//         return ret;
//     }
// };

Create3SensorModel::Create3SensorModel(QObject *parent)
    : QStandardItemModel(parent)
{
    populate();
}

Create3SensorModel::~Create3SensorModel()
{
}

Create3SensorModel::SensorType Create3SensorModel::type(const QModelIndex &index) const
{
    QModelIndex nameIndex = index.sibling(index.row(), 0);
    Create3SensorNameItem *const item = Create3SensorNameItem::cast(itemFromIndex(nameIndex));
    return item ? item->sensorType() : Create3SensorModel::Other;
}

void Create3SensorModel::update()
{
    for (int i = 0; i < rowCount(); ++i)
    {
        Updateable *updateable = Updateable::cast(item(i, 1));
        if (!updateable)
            continue;
        updateable->update();
    }
}

void Create3SensorModel::populate()
{
    for (unsigned int i = 0; i < 6; ++i)
        populateBump(i);
    for (unsigned int i = 0; i < 5; ++i)
        populateCliff(i);
    for (unsigned int i = 0; i < 8; ++i)
        populateIR(i);
}

void Create3SensorModel::populateBump(unsigned int port)
{

    switch (port)
    {
    case 0:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Bump0, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Bump, port, true));
        break;
    case 1:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Bump1, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Bump, port, true));
        break;
    case 2:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Bump2, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Bump, port, true));
        break;
    case 3:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Bump3, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Bump, port, true));
        break;
    case 4:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Bump4, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Bump, port, true));
        break;
    default:
        break;
    }
}

void Create3SensorModel::populateCliff(unsigned int port)
{

    switch (port)
    {
    case 0:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Cliff0, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Cliff, port, true));
        break;
    case 1:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Cliff1, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Cliff, port, true));
        break;
    case 2:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Cliff2, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Cliff, port, true));
        break;
    case 3:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::Cliff3, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::Cliff, port, true));
        break;

    default:
        break;
    }
}

void Create3SensorModel::populateIR(unsigned int port)
{
    switch (port)
    {
    case 0:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR0, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 1:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR1, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 2:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR2, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 3:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR3, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 4:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR4, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 5:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR5, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    case 6:
        appendRow(QList<QStandardItem *>()
                  << new Create3SensorNameItem(Create3SensorModel::IR6, port)
                  << new Create3SensorValueItem<unsigned short>(Create3SensorModel::IR, port, true));
        break;
    default:
        break;
    }
}
