#include "FileManagerWidget.h"

#include "ui_FileManagerWidget.h"

#include "MenuBar.h"
#include "RootController.h"
#include "FileUtils.h"
#include "AreYouSureDialog.h"
#include "NotYetImplementedDialog.h"
#include "Clipboard.h"
#include <QFileSystemModel>
#include <QFile>
#include <QDebug>

FileManagerWidget::FileManagerWidget(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::FileManagerWidget),
	m_menuBar(new MenuBar(this)),
	m_fs(new QFileSystemModel(this)),
	m_up(new QAction(tr("Go Up"), this))
{
	ui->setupUi(this);
	m_menuBar->setTitle(tr("File Manager"));
	layout()->setMenuBar(m_menuBar);
	QAction *ex = new QAction(tr("Exit"), this);
	connect(m_up, SIGNAL(activated()), SLOT(up()));
	connect(ex, SIGNAL(activated()), SLOT(exit()));
	m_menuBar->addAction(ex);
	m_menuBar->addAction(m_up);
	
	m_fs->setRootPath("/");
	
	ui->files->setModel(m_fs);
	home();
	
	connect(ui->open, SIGNAL(clicked()), SLOT(open()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->copy, SIGNAL(clicked()), SLOT(copy()));
	connect(ui->paste, SIGNAL(clicked()), SLOT(paste()));
	connect(ui->home, SIGNAL(clicked()), SLOT(home()));
}

FileManagerWidget::~FileManagerWidget()
{
	delete ui;
}

void FileManagerWidget::exit()
{
	RootController::ref().dismissWidget();
}

void FileManagerWidget::up()
{
	QModelIndexList indexes = ui->files->selectionModel()->selection().indexes();
	if(indexes.size() != 1) return;
	ui->files->setRootIndex(m_fs->parent(ui->files->rootIndex()));
	updateOptions();
}

void FileManagerWidget::open()
{
	QModelIndexList indexes = ui->files->selectionModel()->selection().indexes();
	if(indexes.size() != 1) return;
	ui->files->setRootIndex(indexes[0]);
}

void FileManagerWidget::copy()
{
	QModelIndexList indexes = ui->files->selectionModel()->selection().indexes();
	if(indexes.size() != 1) return;
	Clipboard::ref().setContents(m_fs->filePath(indexes[0]),
		m_fs->isDir(indexes[0]) ? Clipboard::Folder : Clipboard::File);
}

void FileManagerWidget::paste()
{
	NotYetImplementedDialog::nyi();
	return;
	
	const QString from = Clipboard::ref().contents().toString();
	const QString to = m_fs->filePath(ui->files->rootIndex());
	const Clipboard::Type &type = Clipboard::ref().type();
	if(type == Clipboard::File || type == Clipboard::Folder) {
		if(QFileInfo(to).dir().exists(QFileInfo(from).fileName())) {
			AreYouSureDialog confirm;
			confirm.setDescription(tr("The directory %1 already exists. Pasting %2 will overwrite %1.")
				.arg(QFileInfo(from).fileName()).arg(from));
			int ret = RootController::ref().presentDialog(&confirm);
			if(ret == QDialog::Rejected) return;
			FileUtils::rm(QFileInfo(to).dir().filePath(QFileInfo(from).fileName()));
		}
		qDebug() << "Copy success?" << FileUtils::copy(from, to);
		return;
	}
	qWarning() << "FileManager doesn't know how to paste this type.";
}

void FileManagerWidget::remove()
{
	QModelIndexList indexes = ui->files->selectionModel()->selection().indexes();
	if(indexes.size() != 1) return;
	const QString to = m_fs->filePath(indexes[0]);
	AreYouSureDialog confirm;
	confirm.setDescription(tr("You are about to permanently delete %1.")
		.arg(QFileInfo(to).fileName()));
	int ret = RootController::ref().presentDialog(&confirm);
	if(ret == QDialog::Rejected) return;
	qDebug() << "Remove success?" << FileUtils::rm(to);
}

void FileManagerWidget::home()
{
	ui->files->setRootIndex(m_fs->index(m_fs->rootPath()));
	updateOptions();
}

void FileManagerWidget::updateOptions()
{
	// m_up->setEnabled(ui->files->rootIndex() != m_fs->index(m_fs->rootPath()));
}