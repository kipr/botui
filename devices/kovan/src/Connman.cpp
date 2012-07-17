#include "Connman.h"
#include "Manager.h"
#include "Network.h"

#include <QtDBus>
#include <QDebug>

#define SERVICES_KEY "Services"

using namespace Kovan;

#define TECHNOLOGY "wifi"

NetworkItem::NetworkItem(const NetworkPtr& network)
	: QStandardItem(network->name())
{
	setSizeHint(QSize(0, 50));
	qDebug() << "Made Network Item with name" << network->name();
}


NetworkItemModel::NetworkItemModel()
{
}

void NetworkItemModel::scanned(const NetworkPtrList& networks)
{
	clear();
	qDebug() << "Scanned (item model)";
	foreach(const NetworkPtr& network, networks) appendRow(new NetworkItem(network));
}

void NetworkItemModel::beginResetModel()
{
	clear();
}

void NetworkItemModel::endResetModel()
{
	refresh();
}

void NetworkItemModel::refresh()
{
	
}

Connman::Connman()
	: m_type(TECHNOLOGY),
	m_manager(new net::connman::Manager("net.conmann.Manager", "/", QDBusConnection::systemBus(), this)),
	m_setup(false),
	m_networkItemModel(new NetworkItemModel())
{
	m_networkItemModel->connect(this, SIGNAL(scanned(NetworkPtrList)), SLOT(scanned(NetworkPtrList)));
}

Connman::~Connman()
{
	delete m_networkItemModel;
	delete m_manager;
}

void Connman::setup()
{
	qDebug() << "Setup...";
	if(m_setup) return;
	QDBusPendingReply<QVariantMap> reply = m_manager->GetProperties();
	QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
	connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)), SLOT(finishSetup(QDBusPendingCallWatcher *)));
	m_setup = true;
}

const NetworkingProvider::NetworkState Connman::networkState() const
{
	return NetworkOff;
}

void Connman::setNetworkState(const NetworkingProvider::NetworkState& state)
{
	
}

NetworkPtrList Connman::list() const
{
	QStringList services = servicesToStringList();
	NetworkPtrList ret;
	foreach(const QString& service, services) ret << NetworkPtr(new Kovan::Network(service));
	return ret;
}

const bool Connman::scan()
{
	qDebug() << "Requesting Scan...";
	QDBusPendingReply<> reply = m_manager->RequestScan(m_type);
	reply.waitForFinished();
	if(reply.isError()) qCritical() << reply.error();
	return !reply.isError();
}

const float Connman::networkStrengthMin() const
{
	return 0.0;
}

const float Connman::networkStrengthMax() const
{
	return 255.0;
}

QAbstractItemModel *Connman::networkItemModel() const
{
	return m_networkItemModel;
}

void Connman::finishSetup(QDBusPendingCallWatcher *watcher)
{
	qDebug() << "finishSetup";
	QDBusPendingReply<QVariantMap> reply = *watcher;
	m_properties = reply.value();
	qDebug() << m_properties;
	QObject::connect(m_manager, SIGNAL(PropertyChanged(QString, QDBusVariant)), SLOT(propertyChanged(QString, QDBusVariant)));
}

void Connman::propertyChanged(const QString& name, const QDBusVariant& value)
{
	m_properties[name] = value.variant();
	if(name == SERVICES_KEY) emit scanned(list());
	qDebug() << servicesToStringList();
}

QStringList Connman::servicesToStringList() const
{
	QStringList ret;
	QList<QDBusObjectPath> list = qdbus_cast<QList<QDBusObjectPath> >(m_properties[SERVICES_KEY]);
	foreach(const QDBusObjectPath& obj, list) ret << obj.path();
	return ret;
}