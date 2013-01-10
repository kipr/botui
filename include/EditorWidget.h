#ifndef _EDITORWIDGET_H_
#define _EDITORWIDGET_H_

#include <QWidget>
#include <kar.hpp>

class Device;
class MenuBar;

namespace Ui
{
	class EditorWidget;
}

class EditorWidget : public QWidget
{
Q_OBJECT
public:
	EditorWidget(Device *device, QWidget *parent = 0);
	~EditorWidget();
	
	void setSavePath(const QString &savePath);
	const QString &savePath() const;
	
	void setArchive(const Kiss::KarPtr &archive);
	const Kiss::KarPtr &archive() const;
	
private slots:
	void fileChanged(int i);
	void saveAndExit();
	
private:
	Device *m_device;
	
	Ui::EditorWidget *ui;
	MenuBar *m_menuBar;
	
	QString m_savePath;
	Kiss::KarPtr m_archive;
	
	QList<QString> m_lookup;
	
	int m_currentIndex;
};

#endif
