#include "ChannelConfigurationsWidget.h"
#include "ui_ChannelConfigurationsWidget.h"

#include "KeyboardDialog.h"
#include "RootController.h"
#include "ChannelsWidget.h"

#include <QItemSelection>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QDebug>

#include <kovan/camera.hpp>
#include <kovan/config.hpp>

ChannelConfigurationsWidget::ChannelConfigurationsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ChannelConfigurationsWidget),
	m_model(new QFileSystemModel(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Channel Configurations"));
	
	ui->configs->setModel(m_model);
	
	const QString configPath = QString::fromStdString(Camera::ConfigPath::path());
	ui->configs->setRootIndex(m_model->setRootPath(configPath));

	connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
	connect(ui->rename, SIGNAL(clicked()), SLOT(rename()));
	connect(ui->add, SIGNAL(clicked()), SLOT(add()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->configs->selectionModel(),
		SIGNAL(currentChanged(QModelIndex, QModelIndex)),
		SLOT(currentChanged(QModelIndex)));
	
	currentChanged(QModelIndex());
}

ChannelConfigurationsWidget::~ChannelConfigurationsWidget()
{
	delete ui;
}

void ChannelConfigurationsWidget::edit()
{
	QItemSelection selection = ui->configs->selectionModel()->selection();
	if(selection.indexes().size() != 1) return;
	const QModelIndex &index = selection.indexes()[0];
	
	RootController::ref().presentWidget(new ChannelsWidget(device()));
}

void ChannelConfigurationsWidget::rename()
{
	QItemSelection selection = ui->configs->selectionModel()->selection();
	if(selection.indexes().size() != 1) return;
	const QModelIndex &index = selection.indexes()[0];
	
	QFileInfo file = m_model->fileInfo(index);
	KeyboardDialog keyboard(tr("Rename %1").arg(file.fileName()));
	keyboard.setInput(file.baseName());
	RootController::ref().presentDialog(&keyboard);
	if(!QFile::rename(file.filePath(),
		file.path() + "/" + keyboard.input() + "." + file.completeSuffix())) {
		qWarning() << "Failed to change name";
		// TODO: Make this error user visible
		return;
	}
}

void ChannelConfigurationsWidget::add()
{
	KeyboardDialog keyboard(tr("Create New Configuration"));
	RootController::ref().presentDialog(&keyboard);
	Config blank;
	std::string savePath = Camera::ConfigPath::path(keyboard.input().toStdString());
	if(!blank.save(savePath)) {
		qWarning() << "Error saving" << QString::fromStdString(savePath);
		return;
	}
}

void ChannelConfigurationsWidget::remove()
{
	QItemSelection selection = ui->configs->selectionModel()->selection();
	if(selection.indexes().size() != 1) return;
}

void ChannelConfigurationsWidget::currentChanged(const QModelIndex &index)
{
	const bool enable = index.isValid();
	ui->edit->setEnabled(enable);
	ui->rename->setEnabled(enable);
	ui->remove->setEnabled(enable);
}
