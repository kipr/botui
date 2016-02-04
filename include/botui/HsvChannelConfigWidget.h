#ifndef _HSVCHANNELCONFIGWIDGET_H_
#define _HSVCHANNELCONFIGWIDGET_H_

#include "ChannelConfigWidget.h"

#include <opencv2/core/core.hpp>

namespace Ui
{
	class HsvChannelConfigWidget;
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
	void visual();
	void manual();
	
	void manualEntry(const QString &number);
	
	void visualChanged();
	
	void imagePressed(const cv::Mat &image, const int &x, const int &y);
	
	void done();
	
private:
	
	void blockChildSignals(const bool &block);
	
	Ui::HsvChannelConfigWidget *ui;
	NumpadDialog *m_numpad;
};

#endif
