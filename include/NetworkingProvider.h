#ifndef _NETWORKINGPROVIDER_H_
#define _NETWORKINGPROVIDER_H_

#include <QObject>
#include <QString>
#include <QVariant>
#include <QList>
#include <QVariantMap>
#include <QSharedPointer>
#include <QStandardItemModel>

class QAbstractItemModel;

class Network : public QObject
{
Q_OBJECT
public:
	Network(QObject *parent = 0);
	virtual ~Network();
	
	virtual QString path() const = 0;
	virtual QString name() const = 0;
	virtual bool isHidden() const = 0;
	virtual QString state() const = 0;
	virtual QString type() const = 0;
	virtual QStringList security() const = 0;
	virtual QString passphrase() const = 0;
	virtual void setPassphrase(const QString& passphrase) = 0;
	virtual quint8 strength() const = 0;

	virtual void setAutoConnect(const bool& value) = 0;

	virtual QVariantMap ipv4Settings() = 0;
	virtual void setIpv4Settings(const QVariantMap& map) = 0;

public slots:
	virtual void connect() = 0;
	virtual void disconnect() = 0;

signals:
	void propertiesChanged();
	void stateChanged();
	void connected();
	
protected:
	Network();
	Network(const Network& rhs);
	Network& operator=(Network& rhs);
	
private:
	QString m_name;
	QVariant m_internal;
};

typedef QSharedPointer<Network> NetworkPtr;
typedef QList<NetworkPtr> NetworkPtrList;

class NetworkItemModel : public QStandardItemModel
{
public:
	virtual NetworkPtr network(const QModelIndex& index) = 0;
};

class NetworkingProvider : public QObject
{
Q_OBJECT
Q_PROPERTY(NetworkState networkState
	READ networkState
	WRITE setNetworkState
	NOTIFY networkStateChanged)

public:
	virtual ~NetworkingProvider();
	
	enum NetworkState {
		NetworkUnknown,
		NetworkOn,
		NetworkOff
	};
	
	virtual const NetworkingProvider::NetworkState networkState() const = 0;
	virtual void setNetworkState(const NetworkingProvider::NetworkState& state) = 0;
	virtual NetworkItemModel *networkItemModel() const = 0;
	
	virtual const bool scan() = 0;
	virtual NetworkPtrList list() const = 0;
	
	virtual const float networkStrengthMin() const = 0;
	virtual const float networkStrengthMax() const = 0;

signals:
	void networkStateChanged(const NetworkState& state);
	void scanned(const NetworkPtrList& networks);
	void connected(const NetworkPtr& network);
};

#endif
