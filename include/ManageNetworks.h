#ifndef _MANAGENETWORKS_H_
#define _MANAGENETWORKS_H_

#include "StandardWidget.h"

namespace Ui
{
	class ManageNetworks;
}

class ManageNetworks : public StandardWidget
{
public:
	ManageNetworks(Device *device, QWidget *parent = 0);
	~ManageNetworks();
	
private:
	Ui::ManageNetworks *ui;
	Device *m_device;
};

#endif
