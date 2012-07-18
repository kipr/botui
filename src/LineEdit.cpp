#include "LineEdit.h"
#include <QEvent>
#include "InputProviderDialog.h"
#include "RootController.h"

LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent),
	m_inputProvider(0)
{
	init();
}

LineEdit::LineEdit(InputProviderDialog *inputProvider, QWidget *parent)
	: QLineEdit(parent),
	m_inputProvider(inputProvider)
{
	init();
}

bool LineEdit::event(QEvent *e)
{
	if(!m_inputProvider) {
		e->ignore();
		return false;
	}
	
	if(e->type() == QEvent::MouseButtonPress) {
		int ret = RootController::ref().presentDialog(m_inputProvider);
		if(ret == QDialog::Accepted) setText(m_inputProvider->input());
		e->accept();
		return true;
	}
	
	return QLineEdit::event(e);
}

void LineEdit::setInputProvider(InputProviderDialog *inputProvider)
{
	m_inputProvider = inputProvider;
}

InputProviderDialog *LineEdit::inputProvider() const
{
	return m_inputProvider;
}

void LineEdit::init()
{
	setFocusPolicy(Qt::NoFocus);
}