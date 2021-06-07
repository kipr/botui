#ifndef _NOTYETIMPLEMENTEDDIALOG_H_
#define _NOTYETIMPLEMENTEDDIALOG_H_

#include <QDialog>
#include <QQuickWidget>

namespace Ui
{
	class NotYetImplementedDialog;
}

class NotYetImplementedDialog : public QDialog
{
public:
	NotYetImplementedDialog(QQuickWidget *parent = 0);
	~NotYetImplementedDialog();
	
	static void nyi();
	
private:
	Ui::NotYetImplementedDialog *ui;
};

#endif
