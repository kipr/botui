#include "EditorWidget.h"
#include "ui_EditorWidget.h"

#include "MenuBar.h"

#include "RootController.h"
#include "Device.h"
#include "KeyboardDialog.h"
#include "AreYouSureDialog.h"

#include <QApplication>

EditorWidget::EditorWidget(Device *device, QWidget *parent)
	: QWidget(parent),
	m_device(device),
	ui(new Ui::EditorWidget),
	m_menuBar(new MenuBar(this)),
	m_currentIndex(0)
{
	ui->setupUi(this);
	layout()->setMenuBar(m_menuBar);
	QAction *saveAndExit = m_menuBar->addAction(tr("Save"));
	connect(saveAndExit, SIGNAL(triggered()), SLOT(saveAndExit()));
	
	QAction *addFile = m_menuBar->addAction(tr("Add File"));
	connect(addFile, SIGNAL(triggered()), SLOT(addFile()));
	
	QAction *removeFile = m_menuBar->addAction(tr("Delete File"));
	connect(removeFile, SIGNAL(triggered()), SLOT(removeFile()));
	
	connect(ui->files, SIGNAL(currentIndexChanged(int)), SLOT(fileChanged(int)));
}

EditorWidget::~EditorWidget()
{
	delete ui;
}

void EditorWidget::setSavePath(const QString &savePath)
{
	m_savePath = savePath;
}

const QString &EditorWidget::savePath() const
{
	return m_savePath;
}

void EditorWidget::setArchive(const kiss::KarPtr &archive)
{
	m_lookup.clear();
	ui->files->clear();
	m_currentIndex = -1;
	m_archive = archive;
	if(m_archive.isNull()) return;
	foreach(const QString &file, m_archive->files()) m_lookup << file;
	ui->files->addItems(m_lookup);
	if(m_lookup.isEmpty()) {
		fileChanged(-1);
		return;
	}
	ui->text->setPlainText(m_archive->data(m_lookup[0]));
}

const kiss::KarPtr &EditorWidget::archive() const
{
	return m_archive;
}

void EditorWidget::fileChanged(int i)
{
	ui->text->setEnabled(i >= 0);
	if(i < 0) {
		ui->text->setPlainText(tr("No files to edit."));
		m_currentIndex = -1;
	}
	if(i >= m_lookup.size() || i < 0) return;
	
	if(m_currentIndex >= 0) m_archive->setFile(m_lookup[m_currentIndex], ui->text->toPlainText().toUtf8());
	ui->text->setPlainText(m_archive->data(m_lookup[i]));
	
	m_currentIndex = i;
}

void EditorWidget::saveAndExit()
{
	// TODO: Error checking?
	fileChanged(m_currentIndex); // Save current file
	m_archive->save(m_savePath);
	RootController::ref().dismissWidget();
}

void EditorWidget::addFile()
{
	KeyboardDialog dialog(tr("File Name"));
	if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	const QString filename = dialog.input();
	m_lookup << filename;
	ui->files->addItem(filename);
	ui->files->setCurrentIndex(m_lookup.size() - 1);
}

void EditorWidget::removeFile()
{
	if(m_currentIndex < 0) return;
	
	AreYouSureDialog dialog;
	int index = ui->files->currentIndex();
	dialog.setDescription(tr("You're about to permanently delete \"%1\". Continue?").arg(m_lookup[index]));
	if(RootController::ref().presentDialog(&dialog) != QDialog::Accepted) return;
	m_archive->removeFile(m_lookup[index]);
	m_lookup.removeAt(index);
	ui->files->removeItem(index);
	if(m_lookup.isEmpty()) return;
	index = ui->files->currentIndex();
	ui->text->setPlainText(m_archive->data(m_lookup[index]));
	m_currentIndex = index;
}