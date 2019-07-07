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
	enum class Orientation {
		Normal,
		Inverted
	};

	RootController();
	
	const unsigned int depth() const;
	
	void setDismissable(bool dismissable);
	bool isDismissable() const;
  
  void setFullscreen(const bool fullscreen);
  bool isFullscreen() const;

	void setOrientation(const Orientation orientation);
	Orientation orientation() const;
	
	template<typename T>
	void dismissUntil()
	{
		while(depth() > 1 && !dynamic_cast<T *>(m_stack.top())) m_stack.pop();
	}
	
	template<typename T>
	bool containsWidget()
	{
		QStack<QWidget *>::ConstIterator it = m_stack.begin();
		for(; it != m_stack.end(); ++it) {
			if(dynamic_cast<T *>(*it)) return true;
		}
		return false;
	}
	
public slots:
	void presentQml(const QUrl& url);
	int presentDialog(QDialog *dialog);
	void presentWidget(QWidget *widget, bool owns = true);
	
	void dismissWidget();
	void dismissAllWidgets();
  
  void minimize();

private slots:
	void orient();
	
private:
	void constrain(QWidget *widget);
	void present(QWidget *widget);
	void setOrientation_(double rot);
	
	QMap<QWidget *, bool> m_ownership;
	QStack<QWidget *> m_stack;
	bool m_dismissable;
  bool m_fullscreen;
	Orientation m_orientation;

	QTimer *m_orientationTimer;
	
};

#endif
