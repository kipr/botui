#ifndef _CVWIDGET_H_
#define _CVWIDGET_H_

#include <QImage>
#include <QWidget>
#include <QMutex>
#include <opencv4/opencv2/opencv.hpp>

class CvWidget : public QWidget
{
Q_OBJECT
Q_PROPERTY(bool invalid READ invalid WRITE setInvalid)
public:
	CvWidget(QWidget *parent = 0);
	~CvWidget();
	
	void setInvalid(bool invalid);
	const bool& invalid() const;
	
public slots:
	void updateImage(const cv::Mat &image);
	
signals:
	void pressed(const cv::Mat &image, const int &x, const int &y);
	
protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	
private:
	void scaleImage();
	
	bool m_invalid;
	cv::Mat m_image;
	unsigned char *m_data;
	QImage m_resizedImage;
	mutable QMutex m_mutex;
};

#endif
