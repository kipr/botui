#ifndef _CVWIDGET_H_
#define _CVWIDGET_H_

#include <QImage>
#include <QWidget>
#include <opencv2/opencv.hpp>

class CvWidget : public QWidget
{
Q_OBJECT
Q_PROPERTY(bool invalid READ invalid WRITE setInvalid)
public:
	CvWidget(QWidget *parent = 0);
	~CvWidget();
	
	void setInvalid(bool invalid);
	const bool& invalid() const;
	
	void updateImage(cv::Mat image);
	
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
