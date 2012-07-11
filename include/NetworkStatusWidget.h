#ifndef _NETWORKSTATUSWIDGET_H_
#define _NETWORKSTATUSWIDGET_H_

#include <QWidget>

class NetworkingProvider;

class NetworkStatusWidget : public QWidget
{
Q_OBJECT
Q_PROPERTY(NetworkingProvider *networkingProvider
	READ networkingProvider
	WRITE setNetworkingProvider)
public:
	NetworkStatusWidget(QWidget *parent = 0);
	NetworkStatusWidget(NetworkingProvider *networkingProvider, QWidget *parent = 0);
	
	void setNetworkingProvider(NetworkingProvider *networkingProvider);
	NetworkingProvider *networkingProvider() const;
	
protected:
	void paintEvent(QPaintEvent *event);
	
private slots:
	void networkStateChanged();
	
private:
	void constrain();
	NetworkingProvider *m_networkingProvider;
};

#endif
