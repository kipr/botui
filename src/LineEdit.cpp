#include "LineEdit.h"

#include <QEvent>
#include <QPainter>

#include "InputProviderDialog.h"
#include "RootController.h"

#include <QDebug>

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
	if(isReadOnly() || !m_inputProvider) {
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
	qDebug() << "Input" << m_inputProvider;
	m_inputProvider = inputProvider;
	setReadOnly(!m_inputProvider);
}

InputProviderDialog *LineEdit::inputProvider() const
{
	return m_inputProvider;
}

void LineEdit::paintEvent(QPaintEvent *e)
{
	QLineEdit::paintEvent(e);
	if(isReadOnly() || !m_inputProvider) return;
	QPainter p(this);
	p.setPen(QColor(127, 127, 127));
	p.setFont(font());
	static const int offset = 5;
	const bool right = alignment() == Qt::AlignRight;
	p.drawText(QRect(right ? offset : 0, 0, width() - offset, height()),
		tr("Tap to Edit..."),
		QTextOption(Qt::AlignAbsolute | (right ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter));
}

void LineEdit::init()
{
	setReadOnly(!m_inputProvider);
	setFocusPolicy(Qt::NoFocus);
}