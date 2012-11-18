#ifndef _MANAGENETWORKS_H_
#define _MANAGENETWORKS_H_

#include "StandardWidget.h"

namespace Ui
{
	class ManageNetworks;
}

class NetworkItemModel;

class ManageNetworks : public StandardWidget
{
Q_OBJECT
public:
	ManageNetworks(Device *device, QWidget *parent = 0);
	~ManageNetworks();
	
private slots:
	void forget();
	void information();
	
private:
	Ui::ManageNetworks *ui;
	Device *m_device;
	NetworkItemModel *m_model;
};

#endif
