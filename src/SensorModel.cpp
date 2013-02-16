#include "SensorModel.h"

#include <kovan/analog.hpp>
#include <kovan/digital.hpp>
#include <kovan/accel.hpp>
#include <kovan/sensor_logic.hpp>
#include <kovan/general.h>
#include <math.h>

class SensorNameItem : public QStandardItem
{
public:
	SensorNameItem(SensorModel::SensorType type, const int port = -1)
		: m_type(type)
		, m_typeName(typeName(type))
		, m_optionName(optionName(type))
	{
		setText(m_typeName + (port >= 0 ? QObject::tr(" %1").arg(port) : QString()));
	}
	
	SensorModel::SensorType sensorType() const
	{
		return m_type;
	}
	
	template<typename T>
	static SensorNameItem *cast(T *t)
	{
		return dynamic_cast<SensorNameItem *>(t);
	}
	
private:
	static QString typeName(SensorModel::SensorType type)
	{
		switch(type) {
		case SensorModel::Analog: return QObject::tr("Analog Sensor");
		case SensorModel::Digital: return QObject::tr("Digital Sensor");
		case SensorModel::AccelX: return QObject::tr("Accelerometer X");
		case SensorModel::AccelY: return QObject::tr("Accelerometer Y");
		case SensorModel::AccelZ: return QObject::tr("Accelerometer Z");
		default: break;
		}
		return QObject::tr("Unknown Sensor");
	}
	
	static QString optionName(SensorModel::SensorType type)
	{
		switch(type) {
		case SensorModel::Analog: return QObject::tr("Pullup");
		default:
			return QString();
		}
		return QString();
	}
	
	SensorModel::SensorType m_type;
	QString m_typeName;
	QString m_optionName;
};

class Updateable
{
public:
	virtual void update() = 0;
	
	template<typename T>
	static Updateable *cast(T *t)
	{
		return dynamic_cast<Updateable *>(t);
	}
};

template<typename T>
class SensorValueItem : public QStandardItem, public Updateable
{
public:
	SensorValueItem(Sensor<T> *const sensor, const bool owns)
		: m_sensor(sensor)
		, m_owns(owns)
	{
	}
	
	~SensorValueItem()
	{
		if(!m_owns) return;
		delete m_sensor;
	}
	
	virtual void update()
	{
		setText(QString("%1").arg(m_sensor->value()));
	}
	
	template<typename L>
	static SensorValueItem *cast(L *t)
	{
		return dynamic_cast<SensorValueItem *>(t);
	}
	
private:
	Sensor<T> *const m_sensor;
	bool m_owns;
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

SensorModel::SensorModel(QObject *parent)
	: QStandardItemModel(parent)
{
	populate();
}

SensorModel::~SensorModel()
{
}

SensorModel::SensorType SensorModel::type(const QModelIndex &index) const
{
	QModelIndex nameIndex = index.sibling(0, 0);
	SensorNameItem *item = SensorNameItem::cast(itemFromIndex(nameIndex));
	return item ? item->sensorType() : SensorModel::Other;
}

void SensorModel::update()
{
	publish();
	for(int i = 0; i < rowCount(); ++i) {
		Updateable *updateable = Updateable::cast(item(i, 1));
		if(!updateable) continue;
		updateable->update();
	}
}

void SensorModel::populate()
{
	unsigned char i = 0;
	for(; i < 8; ++i) populateAnalog(i);
	for(; i < 16; ++i) populateDigital(i);
	populateAccel();
}

void SensorModel::populateAnalog(const unsigned char port)
{
	appendRow(QList<QStandardItem *>()
		<< new SensorNameItem(SensorModel::Analog, port)
		<< new SensorValueItem<unsigned short>(new ::Analog(port), true));
}

void SensorModel::populateDigital(const unsigned char port)
{
	appendRow(QList<QStandardItem *>()
		<< new SensorNameItem(SensorModel::Digital, port)
		<< new SensorValueItem<bool>(new SensorLogic::Not(new ::Digital(port), true), true));
}

void SensorModel::populateAccel()
{
	appendRow(QList<QStandardItem *>()
		<< new SensorNameItem(SensorModel::AccelX)
		<< new SensorValueItem<short>(new ::AccelX(), true));
	appendRow(QList<QStandardItem *>()
		<< new SensorNameItem(SensorModel::AccelX)
		<< new SensorValueItem<short>(new ::AccelY(), true));
	appendRow(QList<QStandardItem *>()
		<< new SensorNameItem(SensorModel::AccelX)
		<< new SensorValueItem<short>(new ::AccelZ(), true));
}