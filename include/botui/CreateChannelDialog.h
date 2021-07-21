#ifndef _CREATECHANNELDIALOG_H_
#define _CREATECHANNELDIALOG_H_

#include <QDialog>
#include <QQuickWidget>

namespace Ui
{
	class CreateChannelDialog;
}

class CreateChannelDialog : public QDialog
{
public:
	CreateChannelDialog(QQuickWidget *parent = 0);
	~CreateChannelDialog();
	
	QString type() const;
	
private:
	Ui::CreateChannelDialog *ui;
};

#endif
