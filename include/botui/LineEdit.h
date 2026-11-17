#ifndef _LINEEDIT_H_
#define _LINEEDIT_H_

#include <QLineEdit>
#include <InputProviderWidget.h>

class InputProviderWidget;

class LineEdit : public QLineEdit
{
Q_OBJECT
Q_PROPERTY(InputProviderWidget *inputProvider READ inputProvider WRITE setInputProvider)
public:
	LineEdit(QWidget *parent = 0);
	LineEdit(InputProviderWidget *inputProvider, QWidget *parent = 0);
	virtual bool event(QEvent *e);
	
	void setInputProvider(InputProviderWidget *inputProvider);
	InputProviderWidget *inputProvider() const;
protected:
	void paintEvent(QPaintEvent *e);
	
private:
	void init();
	
	InputProviderWidget *m_inputProvider;
};

#endif
