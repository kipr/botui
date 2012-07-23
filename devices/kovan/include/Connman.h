#ifndef _CONNMAN_H_
#define _CONNMAN_H_

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QStringList>
#include <QList>
#include <QStandardItemModel>
#include <QDBusVariant>

#include "NetworkingProvider.h"
#include "Manager.h"

class QDBusPendingCallWatcher;
class Network;

namespace Kovan
{
	class NetworkItem : public QStandardItem
	{
	public:
		NetworkItem(const NetworkPtr& network);
		
		NetworkPtr network() const;
	private:
		NetworkPtr m_network;
	};

	class NetworkItemModel : public ::NetworkItemModel
	{
	Q_OBJECT
	public:
		NetworkItemModel();
	
		NetworkPtr network(const QModelIndex& index);
	
	public slots:
		void scanned(const NetworkPtrList& networks);
	
	protected:
		virtual void beginResetModel();
		virtual void endResetModel();
	
	private:
		void refresh();
	};

	class Connman : public ::NetworkingProvider
	{
	Q_OBJECT
	public:
		Connman(NetworkingProviderDelegate *delegate);
		~Connman();
	
		void setup();
	
		virtual const NetworkingProvider::NetworkState networkState() const;
		virtual void setNetworkState(const NetworkingProvider::NetworkState& state);
	
	 	virtual NetworkPtrList list() const;
		virtual const bool scan();
	
		virtual const float networkStrengthMin() const;
		virtual const float networkStrengthMax() const;
		
		virtual ::NetworkItemModel *networkItemModel() const;
	
	private slots:
		void finishSetup(QDBusPendingCallWatcher *watcher);
		void propertyChanged(const QString& name, const QDBusVariant& value);
		void stateChanged(const QString& name);
	
	signals:
		void scanned(const NetworkPtrList& list);
	
	private:
		QStringList servicesToStringList() const;
	
		QString m_type;
		net::connman::Manager *m_manager;
		bool m_setup;
		QVariantMap m_properties;
		Kovan::NetworkItemModel *m_networkItemModel;
	};
}

#endif
