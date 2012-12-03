#ifndef _HSVWIDGET_H_
#define _HSVWIDGET_H_

#include <QWidget>
#include <QImage>
#include <opencv2/core/core.hpp>

class HsvWidget : public QWidget {
public:
	HsvWidget(QWidget *parent = 0);
	
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent *event);
	
private:
	void updateImage();
	inline quint16 x2sat(const int &x) const;
	inline quint16 x2val(const int &x) const;
	inline quint16 y2hue(const int &y) const;
	inline int sat2x(const int &sat) const;
	inline int val2x(const int &val) const;
	inline int hue2y(const int &hue) const;

	int m_hueOffset;
	QImage m_image;
};

#endif
