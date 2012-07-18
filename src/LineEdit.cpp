#include "LineEdit.h"
#include <QEvent>
#include "InputProviderDialog.h"
#include "RootController.h"

LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent),
	m_dialog(0)
{
	init();
}

LineEdit::LineEdit(InputProviderDialog *dialog, QWidget *parent)
	: QLineEdit(parent),
	m_dialog(dialog)
{
	init();
}

bool LineEdit::event(QEvent *e)
{
	if(!m_dialog) {
		e->ignore();
		return false;
	}
	
	if(e->type() == QEvent::MouseButtonPress) {
		int ret = RootController::ref().presentDialog(m_dialog);
		if(ret == QDialog::Accepted) setText(m_dialog->input());
		e->accept();
		return true;
	}
	
	return QLineEdit::event(e);
}

void LineEdit::setDialog(InputProviderDialog *dialog)
{
	m_dialog = dialog;
}

InputProviderDialog *LineEdit::dialog() const
{
	return m_dialog;
}

void LineEdit::init()
{
	setFocusPolicy(Qt::NoFocus);
}