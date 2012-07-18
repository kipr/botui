#ifndef _LINEEDIT_H_
#define _LINEEDIT_H_

#include <QLineEdit>

class InputProviderDialog;

class LineEdit : public QLineEdit
{
Q_OBJECT
Q_PROPERTY(InputProviderDialog *dialog READ dialog WRITE setDialog)
public:
	LineEdit(QWidget *parent = 0);
	LineEdit(InputProviderDialog *dialog, QWidget *parent = 0);
	virtual bool event(QEvent *e);
	
	void setDialog(InputProviderDialog *dialog);
	InputProviderDialog *dialog() const;
private:
	void init();
	
	InputProviderDialog *m_dialog;
};

#endif
