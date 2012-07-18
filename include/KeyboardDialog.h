#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include "InputProviderDialog.h"
#include "QKeyButton.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class KeyboardDialog;
}

class KeyboardDialog : public InputProviderDialog
{
Q_OBJECT
Q_PROPERTY(QString input READ input)
	
public:
	enum Mode
	{
		Normal,
		Password
	};

	KeyboardDialog(const QString& text, Mode mode = Normal, QWidget *parent = 0);
	~KeyboardDialog();
	
	QString input() const;
	
private slots:
	void symbolPressed();
	void spacePressed();
	void shiftPressed();
	void delPressed();
	void numPressed();
	
private:
	Ui::KeyboardDialog *ui;
	QWidget *alphaWidget;
	QWidget *numWidget;
	QGridLayout *alphaGrid;
	QGridLayout *numGrid;
	QLabel *label;
	QLineEdit *display;
	
	QKeyButton *alphaButtons[26];
	QKeyButton *numButtons[25];
	QKeyButton *enterButton;
	QKeyButton *spaceButton;
	QKeyButton *shiftButton;
	QKeyButton *delButton;
	QKeyButton *numButton;
	
	bool shifted;
	
	QKeyButton *makeButton(const char *, const QString&, const QString& = "");
	void setupAlphaButtons();
	void setupNumButtons();
	void setupSpecialButtons();
};

#endif // KEYBOARDDIALOG_H
