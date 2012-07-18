#ifndef _ROOTCONTROLLER_H_
#define _ROOTCONTROLLER_H_

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QStack>
#include <QUrl>
#include <QMap>

#include "Singleton.h"

class RootController : public QObject, public Singleton<RootController>
{
Q_OBJECT
Q_PROPERTY(bool dismissable READ isDismissable WRITE setDismissable)
Q_PROPERTY(unsigned int depth READ depth)
public:
	RootController();
	
	void presentQml(const QUrl& url);
	int presentDialog(QDialog *dialog);
	void presentWidget(QWidget *widget, bool owns = true);
	const unsigned int depth() const;
	
	void setDismissable(bool dismissable);
	bool isDismissable() const;
	
public slots:
	void dismissWidget();
	void dismissAllWidgets();
	
private:
	void constrain(QWidget *widget);
	void present(QWidget *widget);
	
	QMap<QWidget *, bool> m_ownership;
	QStack<QWidget *> m_stack;
	bool m_dismissable;
};

#endif
