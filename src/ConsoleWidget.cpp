#include "ConsoleWidget.h"

#include <fstream>

ConsoleWidget::ConsoleWidget(QWidget *parent)
	: QTextEdit(parent),
	m_process(0)
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
	
        //Delete the log file to generate a new log
        std::ofstream myfile;
        myfile.open("/home/pi/debugLog.txt", std::fstream::out | std::fstream::trunc);
        myfile << "Log Cleared." << std::endl;
        myfile.close();

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
	array.replace("\a", "");

        //Output to a debug file
        std::ofstream myfile;
        myfile.open("/home/pi/debugLog.txt", std::fstream::out | std::fstream::app);
        if (myfile.is_open()){
            for(int i = 0; i<array.size(); i++){
                myfile << array[i];
            }
            myfile << std::endl;
            myfile.close();
        }


        insertPlainText(array);



	moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
	update();
}
