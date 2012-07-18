#ifndef _CVWIDGET_H_
#define _CVWIDGET_H_

#include <QImage>
#include <QWidget>
#include <opencv2/opencv.hpp>

class CvWidget : public QWidget
{
public:
	CvWidget(QWidget *parent = 0);
	~CvWidget();
	
	void updateImage(IplImage *image);
	
protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	
private:
	void scaleImage();
	
	bool m_invalid;
	QImage m_image;
	QImage m_resizedImage;
};

#endif
