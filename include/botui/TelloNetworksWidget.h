

#ifndef _TELLONETWORKSWIDGET_H_
#define _TELLONETWORKSWIDGET_H_

#include "Options.h"
#include "StandardWidget.h"
#include <QObject>
#include <QItemSelection>
namespace Ui
{
	class TelloNetworksWidget;
}

class NetworkItemModel;

class TelloNetworksWidget : public StandardWidget
{
Q_OBJECT
public:
	TelloNetworksWidget(Device *device, QWidget *parent = 0);
	~TelloNetworksWidget();
	
public slots:
	void connect();
	void refresh();
	void selectionChanged(const QItemSelection &selection);
	
private:
	Ui::TelloNetworksWidget *ui;
	NetworkItemModel *m_model;
};

#endif


