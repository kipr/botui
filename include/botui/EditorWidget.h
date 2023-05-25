#ifndef _EDITORWIDGET_H_
#define _EDITORWIDGET_H_

#include <QWidget>
//#include <kar/kar.hpp>

class Device;
class MenuBar;

namespace Ui {
class EditorWidget;
}

class EditorWidget : public QWidget {
    Q_OBJECT
  public:
    EditorWidget(Device *device, QWidget *parent = 0);
    ~EditorWidget();

    void setProjectPath(const QString &projectPath);
    const QString &projectPath() const;

  private slots:
    void fileChanged(int i);
    void save();
    void saveAndExit();
    void addFile();
    void removeFile();

  private:
    bool removeDir(const QString &path) const;
    bool getFileContents(const QString &filepath, QString &contents) const;
    bool saveFileContents(const QString &filepath,
                          const QString &contents) const;

    Device *m_device;

    Ui::EditorWidget *ui;
    MenuBar *m_menuBar;

    QString m_projectPath;

    QList<QString> m_lookup;

    int m_currentIndex;
};

#endif
