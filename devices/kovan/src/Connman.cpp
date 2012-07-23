#include "Connman.h"
#include "Manager.h"
#include "Network.h"
#include "NetworkingProviderDelegate.h"
#include "NetworkingDelegateResponder.h"

#include <QtDBus>
#include <QDebug>

#define SERVICES_KEY "Services"

using namespace Kovan;

#define TECHNOLOGY "wifi"

NetworkItem::NetworkItem(const NetworkPtr& network)
	: QStandardItem(network->name()),
	m_network(network)
{
	setSizeHint(QSize(0, 50));
	qDebug() << "Made Network Item with name" << network->name();
}

NetworkPtr NetworkItem::network() const
{
	return m_network;
}


Kovan::NetworkItemModel::NetworkItemModel()
{
}


void Kovan::NetworkItemModel::scanned(const NetworkPtrList& networks)
{
	clear();
	qDebug() << "Scanned (item model)";
	foreach(const NetworkPtr& network, networks) {
		if(!network->isHidden()) appendRow(new NetworkItem(network));
	}
}

NetworkPtr Kovan::NetworkItemModel::network(const QModelIndex& index)
{
	NetworkItem *item = dynamic_cast<NetworkItem *>(itemFromIndex(index));
	return item ? item->network() : NetworkPtr();
}

void Kovan::NetworkItemModel::beginResetModel()
{
	clear();
}

void Kovan::NetworkItemModel::endResetModel()
{
	refresh();
}

void Kovan::NetworkItemModel::refresh()
{
	
}

Connman::Connman(NetworkingProviderDelegate *delegate)
	: NetworkingProvider(delegate),
	m_type(TECHNOLOGY),
	m_manager(new net::connman::Manager("net.connman", "/", QDBusConnection::systemBus(), this)),
	m_setup(false),
	m_networkItemModel(new Kovan::NetworkItemModel())
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

::NetworkItemModel *Connman::networkItemModel() const
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
	QObject::connect(m_manager, SIGNAL(StateChanged(QString)), SLOT(stateChanged(QString)));
	new NetworkingDelegateResponder(this, delegate());
	m_manager->RegisterAgent(QDBusObjectPath("/"));
}

void Connman::propertyChanged(const QString& name, const QDBusVariant& value)
{
	qDebug() << "Property" << name << "changed.";
	m_properties[name] = value.variant();
	if(name == SERVICES_KEY) emit scanned(list());
	qDebug() << servicesToStringList();
}

void Connman::stateChanged(const QString& name)
{
	qDebug() << "State changed" << name;
}

QStringList Connman::servicesToStringList() const
{
	QStringList ret;
	QList<QDBusObjectPath> list = qdbus_cast<QList<QDBusObjectPath> >(m_properties[SERVICES_KEY]);
	foreach(const QDBusObjectPath& obj, list) ret << obj.path();
	return ret;
}