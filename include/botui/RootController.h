#ifndef ROOTCONTROLLER_H
#define ROOTCONTROLLER_H

#include "InputProviderWidget.h"
#include "Singleton.h"

#include <QObject>
#include <QStack>
#include <QUrl>

class QDialog;
class QWidget;
class RootWindow;

class RootController : public QObject, public Singleton<RootController>
{
	Q_OBJECT
	Q_PROPERTY(bool dismissable READ isDismissable WRITE setDismissable)
	Q_PROPERTY(unsigned int depth READ depth)
public:
	RootController();

	void initialize(RootWindow *window, QWidget *rootPage);
	unsigned int depth() const;
	QWidget *currentWidget() const;

	void setDismissable(bool dismissable);
	bool isDismissable() const;

	template <typename T>
	void dismissUntil()
	{
		while (depth() > 1 && !dynamic_cast<T *>(m_stack.top())) {
			const unsigned int previousDepth = depth();
			dismissWidget();
			if (depth() == previousDepth)
				break;
		}
	}

	template <typename T>
	bool containsWidget() const
	{
		for (QWidget *widget : m_stack) {
			if (dynamic_cast<T *>(widget))
				return true;
		}
		return false;
	}

	InputProviderWidget::Result presentInput(InputProviderWidget *inputProvider);

public slots:
	void presentQml(const QUrl &url);
	int presentDialog(QDialog *dialog);
	void presentWidget(QWidget *widget);
	void replaceWidget(QWidget *widget);

	void dismissWidget();
	void dismissAllWidgets();

private:
	bool navigationAllowed(QWidget *newWidget = nullptr) const;

	RootWindow *m_window;
	QStack<QWidget *> m_stack;
	InputProviderWidget *m_activeInput;
	bool m_dismissable;
};

#endif // ROOTCONTROLLER_H
