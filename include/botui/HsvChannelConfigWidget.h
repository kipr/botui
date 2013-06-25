#ifndef _HSVCHANNELCONFIGWIDGET_H_
#define _HSVCHANNELCONFIGWIDGET_H_

#include "ChannelConfigWidget.h"

namespace Ui
{
	class HsvChannelConfigWidget;
}

namespace Camera
{
	class Device;
}

class NumpadDialog;

class HsvChannelConfigWidget : public ChannelConfigWidget
{
Q_OBJECT
public:
	HsvChannelConfigWidget(const QModelIndex &index, QWidget *parent = 0);
	~HsvChannelConfigWidget();
	
	virtual void refresh();
	
private slots:
	void update();
	void visual();
	void manual();
	
	void manualEntry(const QString &number);
	
	void visualChanged();
	
	void imagePressed(const int &x, const int &y);
	
	void done();
	
private:
	void setSlowUpdate();
	void setFastUpdate();
	
	void blockChildSignals(const bool &block);
	
	Ui::HsvChannelConfigWidget *ui;
	NumpadDialog *m_numpad;
	Camera::Device *m_camera;
};

#endif
