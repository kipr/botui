#ifndef _ROOTCONTROLLER_H_
#define _ROOTCONTROLLER_H_

#include <QObject>
#include <QWidget>
#include <QStack>
#include <QUrl>
#include <QMap>

#include "Singleton.h"

class RootController : public QObject, public Singleton<RootController>
{
Q_OBJECT
public:
	RootController();
	
	void presentQml(const QUrl& url);
	void presentWidget(QWidget *widget, bool owns = true);
	const unsigned int depth() const;
	
public slots:
	void dismissWidget();
	void dismissAllWidgets();
	
private:
	void constrain(QWidget *widget);
	void present(QWidget *widget);
	
	QMap<QWidget *, bool> m_ownership;
	QStack<QWidget *> m_stack;
};

#endif
