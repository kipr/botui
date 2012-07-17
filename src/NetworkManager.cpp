#include "NetworkManager.h"

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QMessageBox>
#include <QDebug>

NetworkManager::NetworkManager()
{
	// Set Internet Access Point
	QNetworkConfigurationManager manager;
	const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
	// Is there default access point, use it
	QNetworkConfiguration cfg = manager.defaultConfiguration();
	if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
		QMessageBox::information(0, QObject::tr("Network"), QObject::tr("No Access Point found."));
		return;
	}
	
	QList<QNetworkConfiguration> configs = manager.allConfigurations();
	
	foreach(QNetworkConfiguration config, configs) {
		qDebug() << "Config:";
		qDebug() << "\t" << config.bearerName();
		qDebug() << "\t" << config.name();
		qDebug() << "\t" << config.identifier();
		qDebug() << "\t" << config.isRoamingAvailable();
		qDebug() << "\tState:" << config.state();
		foreach(QNetworkConfiguration config, config.children()) {
			qDebug() << "\tConfig:";
			qDebug() << "\t\t" << config.bearerName();
			qDebug() << "\t\t" << config.name();
			qDebug() << "\t\t" << config.identifier();
			qDebug() << "\t\t" << config.isRoamingAvailable();
			qDebug() << "\t\tState:" << config.state();
		}
	}

	QNetworkSession *session = new QNetworkSession(cfg);
	session->open();
	session->waitForOpened(-1);
}

NetworkManager::~NetworkManager()
{
	
}