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
Q_PROPERTY(QString input READ input)
	
public:
	enum InputType
	{
		Integer,
		Decimal
	};

	QNumpadDialog(const QString& text,
					InputType type = Integer,
					const double& min = std::numeric_limits<double>::min(),
					const double& max = std::numeric_limits<double>::max(),
					QWidget *parent = 0);
	~QNumpadDialog();

	QString input();

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

	QKeyButton *makeButton(const char *, const QString&, const QString& = "");
	bool inBounds(const double&) const;
};

#endif // QNUMPADDIALOG_H
