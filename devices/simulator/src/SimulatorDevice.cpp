#include "SimulatorDevice.h"

#include "BatteryLevelProvider.h"
#include <QAbstractItemModel>
#include <QStandardItemModel>

#include <QDebug>

namespace Simulator
{
	class BatteryLevelProvider : public ::BatteryLevelProvider
	{
	public:
		virtual const float batteryLevel() const;
		virtual const float batteryLevelMin() const;
		virtual const float batteryLevelMax() const;
		virtual const bool isCharging() const;
	};
}

const float Simulator::BatteryLevelProvider::batteryLevel() const
{
	return 100.0;
}

const float Simulator::BatteryLevelProvider::batteryLevelMin() const
{
	return 0.0;
}

const float Simulator::BatteryLevelProvider::batteryLevelMax() const
{
	return 100.0;
}

const bool Simulator::BatteryLevelProvider::isCharging() const
{
	return true;
}

Simulator::Device::Device()
	: m_batteryLevelProvider(new Simulator::BatteryLevelProvider)
{
	
}

Simulator::Device::~Device()
{
	delete m_batteryLevelProvider;
}

QString Simulator::Device::name() const
{
	return "Sim";
}

QString Simulator::Device::version() const
{
	return "0.1a";
}

QString Simulator::Device::id() const
{
  return QString();
}

bool Simulator::Device::isTouchscreen() const
{
	return false;
}

ArchivesManager *Simulator::Device::archivesManager() const
{
	return 0;
}

CompileProvider *Simulator::Device::compileProvider() const
{
	return 0;
}

NetworkingProvider *Simulator::Device::networkingProvider() const
{
	return 0;
}

BatteryLevelProvider *Simulator::Device::batteryLevelProvider() const
{
	return m_batteryLevelProvider;
}

SettingsProvider *Simulator::Device::settingsProvider() const
{
	return 0;
}

ButtonProvider *Simulator::Device::buttonProvider() const
{
	return 0;
}