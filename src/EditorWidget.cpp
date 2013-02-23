#include "EditorWidget.h"
#include "ui_EditorWidget.h"

#include "MenuBar.h"

#include "RootController.h"
#include "Device.h"

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
	QAction *saveAndExit = m_menuBar->addAction(tr("Save and Exit"));
	connect(saveAndExit, SIGNAL(activated()), SLOT(saveAndExit()));
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

void EditorWidget::setArchive(const Kiss::KarPtr &archive)
{
	m_lookup.clear();
	ui->files->clear();
	m_currentIndex = 0;
	m_archive = archive;
	if(m_archive.isNull()) return;
	foreach(const QString &file, m_archive->files()) m_lookup << file;
	ui->files->addItems(m_lookup);
	ui->text->setPlainText(m_archive->data(m_lookup[0]));
}

const Kiss::KarPtr &EditorWidget::archive() const
{
	return m_archive;
}

void EditorWidget::fileChanged(int i)
{
	// Shouldn't happen
	if(i >= m_lookup.size()) return;
	
	m_archive->setFile(m_lookup[m_currentIndex], ui->text->toPlainText().toUtf8());
	ui->text->setPlainText(m_archive->data(m_lookup[i]));
	
	m_currentIndex = i;
}

void EditorWidget::saveAndExit()
{
	// TODO: Error checking?
	m_archive->save(m_savePath);
	RootController::ref().dismissWidget();
}