#include "ConsoleWidget.h"

#include <QApplication>
#include <QKeyEvent>

ConsoleWidget::ConsoleWidget(QWidget *parent)
	: QTextEdit(parent), m_process(0)
{
	cmdStr = "";
	curCursorLoc = this->textCursor();
	inputCharCount = 0;
	histLocation = -1;
	tempCmd = "";
	setProcess(0);
}

ConsoleWidget::~ConsoleWidget()
{
	setProcess(0);
}

void ConsoleWidget::setProcess(QProcess *process)
{
	if(m_process) m_process->disconnect(this);
	m_process = process;
	setReadOnly(!m_process);
	if(!m_process) return;
	clear();
	m_process->setProcessChannelMode(QProcess::MergedChannels);
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOut()));
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardErr()));
}

QProcess *ConsoleWidget::process() const
{
	return m_process;
}

void ConsoleWidget::readStandardOut()
{
	insertPlainText(m_process->readAllStandardOutput());
	moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
}

void ConsoleWidget::readStandardErr()
{
	insertPlainText(m_process->readAllStandardError());
	moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
}

void ConsoleWidget::changeDir(const QString & dir)
{
	QString theDir = QString(dir);
	theDir = theDir.replace(QChar('/'), "\\");
	m_process->write("cd ", 3);
	m_process->write(theDir.toAscii().data(), theDir.length());
	m_process->write("\r\n", 2);
}

void ConsoleWidget::keyPressEvent(QKeyEvent * event)
{
	int key = event->key();

	setTextCursor(curCursorLoc);

	if (key != Qt::Key_Backspace) {
		if (key == Qt::Key_Return || key == Qt::Key_Enter) {
			inputCharCount = 0;
		} else if (key == Qt::Key_Up) {
			if (cmdHistory.size()) {
				if (histLocation == -1) {
					histLocation = cmdHistory.size() - 1;
					tempCmd = cmdStr;
				} else if (histLocation == 0) {
					QApplication::beep();
					event->ignore();
					return;
				} else {
					--histLocation;
				}

				for (int i = 0; i < inputCharCount; ++i) {
					QTextEdit::keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
				}

				inputCharCount = cmdHistory.at(histLocation).length();
				this->insertPlainText(cmdHistory.at(histLocation));
				cmdStr = cmdHistory.at(histLocation);
			}

			event->ignore();
			return;
		} else if (key == Qt::Key_Down) {
			QString str = "";
			if (histLocation == -1) {
				QApplication::beep();
				event->ignore();
				return;
			} else if (histLocation == cmdHistory.size() - 1) {
				histLocation = -1;
				str = tempCmd;
			} else {
				++histLocation;
				str = cmdHistory.at(histLocation);
			}

			for (int i = 0; i < inputCharCount; ++i) {
				QTextEdit::keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
			}

			inputCharCount = str.length();
			this->insertPlainText(str);
			cmdStr = str;
		} else if (key == Qt::Key_Left) {
			if (inputCharCount) {
				--inputCharCount;
				QTextEdit::keyPressEvent(event);
			} else QApplication::beep();
		} else if (key == Qt::Key_Right) {
			QTextCursor cursor = this->textCursor();
			if (cursor.movePosition(QTextCursor::Right)) {
				++inputCharCount;
				this->setTextCursor(cursor);
			} else QApplication::beep();
		} else if (key == Qt::Key_Tab) {
		} else {
			QString text = event->text();
			for (int i = 0; i < text.length(); ++i) {
				if (text.at(i).isPrint()) ++inputCharCount;
			}
			QTextEdit::keyPressEvent(event);
		}
	} else {
		if (inputCharCount) {
			--inputCharCount;
			QTextEdit::keyPressEvent(event);
			cmdStr.remove(inputCharCount, 1);
		} else QApplication::beep();
	}

	if (key == Qt::Key_Return || key == Qt::Key_Enter) {
		moveCursor(QTextCursor::End);
		m_process->write(cmdStr.toAscii().data(), cmdStr.length());
		m_process->write("\r\n", 2);
		QTextEdit::keyPressEvent(event);
		cmdHistory.push_back(cmdStr);
		histLocation = -1;
		cmdStr = "";
		tempCmd = "";
	} else {
		QString input = event->text();
		for (int i = 0; i < input.length(); ++i) {
			if (input.at(i).isPrint()) cmdStr.insert(inputCharCount - 1, input.at(i));
		}
	}
	curCursorLoc = textCursor();
}