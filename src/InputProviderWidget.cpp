#include "InputProviderWidget.h"

InputProviderWidget::InputProviderWidget(QWidget *parent)
	: QWidget(parent),
	  m_result(Rejected)
{
	// Unlike QDialog, a child QWidget becomes visible with its parent unless it
	// has been explicitly hidden.
	hide();
}

InputProviderWidget::Result InputProviderWidget::result() const
{
	return m_result;
}

void InputProviderWidget::resetResult()
{
	m_result = Rejected;
}

void InputProviderWidget::accept()
{
	finish(Accepted);
}

void InputProviderWidget::reject()
{
	finish(Rejected);
}

void InputProviderWidget::finish(Result result)
{
	m_result = result;
	emit finished(result);
}
