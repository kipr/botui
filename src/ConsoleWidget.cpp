#include "ConsoleWidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

ConsoleWidget::ConsoleWidget(QWidget *parent)
	: QTextEdit(parent),
	m_process(0),
	m_savedPalette(palette())
{
  setProcess(0);
}

ConsoleWidget::~ConsoleWidget()
{
	setProcess(0);
}

void ConsoleWidget::setProcess(QIODevice *process)
{
	if(m_process) m_process->disconnect(this);
	m_process = process;
	setReadOnly(!m_process);
	if(!m_process) return;
	clear();
	
	connect(m_process, SIGNAL(readyRead()), this, SLOT(readStandardOut()));
}

QIODevice *ConsoleWidget::process() const
{
	return m_process;
}

void ConsoleWidget::readStandardOut()
{
	QByteArray array = m_process->readAll();
	int i = array.lastIndexOf('\f');
	if(i >= 0) {
		setPlainText("");
		array = array.mid(i + 1);
	}
	i = array.lastIndexOf('\a');
	if(i >= 0) startBeep();
	array.remove(i, 1);
	
	insertPlainText(array);

        //Output to a debug file
        std::ofstream myfile.open("/home/pi/debugLog.txt", std::ofstream::out | std::ofstream::trunc); //trunc is necessary to prevent filling the file throughout usage
        if (myfile.is_open()){
            //Cycle through array and write to file
            for(int count = 0; count < array.lastIndexOf('/f'); count++){
                myfile << array[count] << " " ;
            }
        myfile.close();
        }

	moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
	update();
}

void ConsoleWidget::startBeep()
{
	QPalette p = palette();
	p.setColor(QPalette::Base, QColor(255, 200, 200));
	setPalette(p);
	QTimer::singleShot(155, this, SLOT(endBeep()));
}

void ConsoleWidget::endBeep()
{
	setPalette(m_savedPalette);
}
