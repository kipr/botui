#ifndef _LOGDIALOG_H_
#define _LOGDIALOG_H_

#include <QDialog>
#include <QQuickWidget>

namespace Ui
{
	class LogDialog;
}

class LogDialog : public QDialog
{
Q_OBJECT
public:
	LogDialog(QQuickWidget *parent = 0);
	
public slots:
	void setDismissable(const bool &dismissable);
	void setStatus(const QString &status);
	void appendText(const QString &text);
	void clearText();
	
private:
	Ui::LogDialog *ui;
};

#endif
