#include "EditorWidget.h"
#include "ui_EditorWidget.h"

#include "MenuBar.h"

#include "AreYouSureDialog.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "RootController.h"
#include "SystemPrefix.h"

#include <QApplication>
#include <QDebug>

EditorWidget::EditorWidget(Device *device, QWidget *parent)
    : QWidget(parent),
      m_device(device),
      ui(new Ui::EditorWidget),
      m_menuBar(new MenuBar(this)),
      m_currentIndex(0) {
    ui->setupUi(this);
    layout()->setMenuBar(m_menuBar);
    QAction *saveAndExit = m_menuBar->addAction(tr("Save"));
    connect(saveAndExit, SIGNAL(triggered()), SLOT(saveAndExit()));

    QAction *addFile = m_menuBar->addAction(tr("Add File"));
    connect(addFile, SIGNAL(triggered()), SLOT(addFile()));

    QAction *m_button = m_menuBar->addAction("Cancel");
    connect(m_button,
            SIGNAL(triggered()),
            RootController::ptr(),
            SLOT(dismissWidget()));

    QAction *removeFile = m_menuBar->addAction(tr("Delete File"));
    connect(removeFile, SIGNAL(triggered()), SLOT(removeFile()));

    connect(
        ui->files, SIGNAL(currentIndexChanged(int)), SLOT(fileChanged(int)));
}

EditorWidget::~EditorWidget() { delete ui; }

bool EditorWidget::getFileContents(const QString &filepath,
                                   QString &contents) const {
    QFile file(filepath);
    if (!file.exists()) {
        qDebug() << "Missing file: " << filepath;
        return false;
    }

    // load file contents
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        contents = stream.readAll();
        file.close();
    }
    else {
        // TODO: QMessageBox error?
        return false;
    }

    return true;
}

bool EditorWidget::saveFileContents(const QString &filepath,
                                    const QString &contents) const {
    QFile file(filepath);

    // save file contents
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << contents;
        file.flush();
        file.close();
    }
    else {
        // TODO: QMessageBox error?
        return false;
    }

    return true;
}

void EditorWidget::setProjectPath(const QString &projectPath) {
    m_projectPath = projectPath;

    m_lookup.clear();
    ui->files->clear();
    m_currentIndex = -1;

    const QDir projectDir(projectPath);
    const QDir includeDir(projectPath + "/include/");
    const QDir srcDir(projectPath + "/src/");
    const QDir dataDir(projectPath + "/data/");

    // foreach(const QString &file, m_archive->files()) m_lookup << file;

    qDebug() << " checking for source files at " << srcDir.absolutePath();
    foreach (const QString &file,
             srcDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files))
        m_lookup << "src/" + file;

    qDebug() << " checking for include files at " << includeDir.absolutePath();
    foreach (const QString &file,
             includeDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files))
        m_lookup << "include/" + file;

    qDebug() << " checking for data files at " << dataDir.absolutePath();
    foreach (const QString &file,
             dataDir.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files))
        m_lookup << "data/" + file;

    ui->files->addItems(m_lookup);
    if (m_lookup.isEmpty()) {
        fileChanged(-1);
        return;
    }

    // clear text window
    ui->text->clear();

    QString filepath = m_projectPath + m_lookup[0];

    QString contents;
    if (getFileContents(filepath, contents)) {
        ui->text->setPlainText(contents);
    }
}

const QString &EditorWidget::projectPath() const { return m_projectPath; }

void EditorWidget::fileChanged(int i) {
    ui->text->setEnabled(i >= 0);
    if (i < 0) {
        ui->text->setPlainText(tr("No files to edit."));
        m_currentIndex = -1;
    }
    if (i >= m_lookup.size() || i < 0) return;

    // clear text window
    ui->text->clear();

    QString filepath = m_projectPath + m_lookup[i];

    QString contents;
    if (getFileContents(filepath, contents)) {
        ui->text->setPlainText(contents);
    }

    m_currentIndex = i;
}

bool EditorWidget::removeDir(const QString &path) const {
    bool success = true;
    /*
        QDir directory(path);

        QFileInfoList files = directory.entryInfoList(QDir::Files |
       QDir::NoDotAndDotDot); foreach(const QFileInfo &file, files) success &=
       directory.remove(file.fileName());

        QFileInfoList dirs = directory.entryInfoList(QDir::Dirs |
       QDir::NoDotAndDotDot); foreach(const QFileInfo &dir, dirs) success &=
       removeDir(dir.absoluteFilePath());

        success &= directory.rmdir(directory.absolutePath());
    */
    return success;
}

void EditorWidget::save() {
    QString filepath = m_projectPath + m_lookup[m_currentIndex];
    saveFileContents(filepath, ui->text->toPlainText());
}

void EditorWidget::saveAndExit() {
    save();
    RootController::ref().dismissWidget();
}

void EditorWidget::addFile() {
    KeyboardDialog dialog(tr("File Name"));
    if (RootController::ref().presentDialog(&dialog) != QDialog::Accepted)
        return;
    const QString filename = dialog.input();
    m_lookup << filename;
    ui->files->addItem(filename);
    ui->files->setCurrentIndex(m_lookup.size() - 1);
}

void EditorWidget::removeFile() {
    if (m_currentIndex < 0) return;

    AreYouSureDialog dialog;
    int index = ui->files->currentIndex();
    dialog.setDescription(
        tr("You're about to permanently delete \"%1\". Continue?")
            .arg(m_lookup[index]));
    if (RootController::ref().presentDialog(&dialog) != QDialog::Accepted)
        return;
    // FIXME: m_archive->removeFile(m_lookup[index]);

    QString filepath = m_projectPath + m_lookup[m_currentIndex];
    QFile file(filepath);
    file.remove();

    m_lookup.removeAt(index);
    ui->files->removeItem(index);
    if (m_lookup.isEmpty()) return;
    index = ui->files->currentIndex();
    // FIXME: ui->text->setPlainText(m_archive->data(m_lookup[index]));
    m_currentIndex = index;
}
