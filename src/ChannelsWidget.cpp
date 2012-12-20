#include "ChannelsWidget.h"

#include "ui_ChannelsWidget.h"

#include "CameraConfigModel.h"
#include "RootController.h"
#include "ChannelConfigWidget.h"
#include "ChannelConfigWidgetFactory.h"

#include <QFileSystemModel>
#include <QStyleOptionViewItem>
#include <QItemDelegate>
#include <QPainter>

#include <kovan/camera.hpp>

class ChannelItemDelegate : public QItemDelegate
{
public:
	ChannelItemDelegate(CameraConfigModel *model, QObject *parent = 0)
		: QItemDelegate(parent),
		m_model(model),
		m_hsv(QIcon(":/icons/color_wheel.png").pixmap(16, 16)),
		m_qr(QIcon(":/icons/qrcode.png").pixmap(16, 16))
	{
	}
	
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		QItemDelegate::paint(painter, option, index);

		const QString &type = m_model->channelType(index);

		QPixmap pixmap;
		if(type == CAMERA_CHANNEL_TYPE_HSV_KEY) pixmap = m_hsv;
		else if(type == CAMERA_CHANNEL_TYPE_QR_KEY) pixmap = m_qr;

		const QPoint right = option.rect.topRight();
		painter->drawPixmap(right.x() - 24, right.y() + option.rect.height() / 2 - 8,
			16, 16, pixmap);
	}
	
private:
	CameraConfigModel *m_model;
	QPixmap m_hsv;
	QPixmap m_qr;
};

ChannelsWidget::ChannelsWidget(Device *device, QWidget *parent)
	: StandardWidget(device, parent),
	ui(new Ui::ChannelsWidget),
	m_model(new CameraConfigModel(this))
{
	ui->setupUi(this);
	performStandardSetup(tr("Channels"));
	ui->channels->setModel(m_model);
	ui->channels->setItemDelegate(new ChannelItemDelegate(m_model, this));
	
	connect(ui->add, SIGNAL(clicked()), SLOT(add()));
	connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
	connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
}

ChannelsWidget::~ChannelsWidget()
{
	delete ui;
}


void ChannelsWidget::save()
{
	
}

void ChannelsWidget::edit()
{
	const QModelIndexList &indexes = ui->channels->selectionModel()
		->selection().indexes();
	if(indexes.size() != 1) return;
	ChannelConfigWidget *widget = ChannelConfigWidgetFactory::create(
		m_model->channelType(indexes[0]));
	if(!widget) return;
	RootController::ref().presentWidget(widget);
}

void ChannelsWidget::up()
{
	
}

void ChannelsWidget::down()
{
	
}

void ChannelsWidget::add()
{
	m_model->addChannel();
}

void ChannelsWidget::remove()
{
	const QModelIndexList &indexes = ui->channels->selectionModel()
		->selection().indexes();
	if(indexes.size() != 1) return;
	m_model->removeChannel(indexes[0].row());
}
