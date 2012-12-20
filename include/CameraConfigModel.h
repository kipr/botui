#ifndef _CAMERACONFIGMODEL_H_
#define _CAMERACONFIGMODEL_H_

#include <QObject>
#include <QStandardItemModel>
#include <kovan/config.hpp>
#include <kovan/camera.hpp>

class CameraConfigModel : public QStandardItemModel
{
Q_OBJECT
public:
	CameraConfigModel(QObject *parent = 0);
	~CameraConfigModel();
	
	const QString &channelType(const QModelIndex &index) const;
	
	Config channelConfig(const QModelIndex &index) const;
	void setChannelConfig(const QModelIndex &index, const Config &config);
	
	void setConfig(const Config &config);
	const Config &config() const;
	
public slots:
	void addChannel();
	void removeChannel(const int &i);
	
private:
	void updateConfig();
	
	Config m_config;
	QString m_blank;
};

#endif
