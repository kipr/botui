#ifndef QNUMPADDIALOG_H
#define QNUMPADDIALOG_H

#include <QDialog>
#include "QKeyButton.h"

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <limits>

namespace Ui
{
	class QNumpadDialog;
}

class QNumpadDialog : public QDialog
{
Q_OBJECT
	
public:
	enum InputType
	{
		Integer,
		Decimal
	};

	QNumpadDialog(QString text,
					InputType type = Integer,
					double min = std::numeric_limits<double>::min(),
					double max = std::numeric_limits<double>::max(),
					QWidget *parent = 0);
	~QNumpadDialog();

	QString getInput();

private slots:
	void displayChanged(QString);
	void numberPressed();
	void delPressed();
	void clearPressed();
	void decimalPressed();
	void signPressed();
	
private:
	Ui::QNumpadDialog *ui;
	QGridLayout *grid;
	QLabel *label;
	QLineEdit *display;

	bool m_decimalExists;
	bool m_isEmpty;
	double m_min;
	double m_max;

	QKeyButton *makeButton(const char*, QString, QString = "");
	void appendDisplay(const QString&);
	bool inBounds(double);
};

#endif // QNUMPADDIALOG_H
