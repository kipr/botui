#include "ChannelsWidget.h"

#include "ui_ChannelsWidget.h"

#include "ConfigurationPaths.h"

#include <QFileSystemModel>

ChannelsWidget::ChannelsWidget(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::ChannelsWidget)
{
	ui->setupUi(this);
	
	QFileSystemModel *fsModel = new QFileSystemModel(this);
	ui->active->setModel(fsModel);
	
	const QModelIndex rootIndex = fsModel->setRootPath(ConfigurationPaths::ref().channelsPath());
	ui->active->setRootModelIndex(rootIndex);
	
	connect(ui->add, SIGNAL(clicked()), SLOT(add()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->active, SIGNAL(currentIndexChanged(int)),
		SLOT(activeChanged(int)));
}

ChannelsWidget::~ChannelsWidget()
{
	delete ui;
}

void ChannelsWidget::save()
{
	
}

void ChannelsWidget::add()
{
	// Create channel
	
	Channel dummy;
	
}

void ChannelsWidget::remove()
{
	
}

void ChannelsWidget::activeChanged(const int &index)
{
	
}