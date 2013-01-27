#include "KovanSerialBridge.h"

#include "Serial.h"
#include "RootController.h"
#include "ProgramWidget.h"
#include "Program.h"

KovanSerialBridge::KovanSerialBridge()
	: m_device(0)
{
}

void KovanSerialBridge::init(Device *device)
{
	static bool s_inited = false;
	if(s_inited) return;
	m_device = device;
	Serial *serialDBus = new Serial("org.kipr.Serial", "/org/kipr/Serial", QDBusConnection::systemBus(), this);
	connect(serialDBus, SIGNAL(Run(QString)), SLOT(run(QString)));
	s_inited = true;
}

void KovanSerialBridge::run(const QString &path)
{
	ProgramWidget *programWidget = new ProgramWidget(Program::instance(), m_device);
	if(!Program::instance()->start(path)) delete programWidget;
	else RootController::ref().presentWidget(programWidget);
}