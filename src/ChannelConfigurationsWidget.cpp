#include "ChannelConfigurationsWidget.h"
#include "ui_ChannelConfigurationsWidget.h"

#include "ChannelConfigurationsModel.h"
#include "ChannelsWidget.h"
#include "KeyboardDialog.h"
#include "MenuBar.h"
#include "RootController.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QItemDelegate>
#include <QItemSelection>
#include <QPainter>

#include <kipr/camera/camera.hpp>
#include <kipr/config/config.hpp>

class ConfigItemDelegate : public QItemDelegate {
  public:
    ConfigItemDelegate(ChannelConfigurationsWidget *parent = 0)
        : QItemDelegate(parent),
          m_star(QIcon(":/icons/star.png").pixmap(16, 16)) {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const {
        QItemDelegate::paint(painter, option, index);
        ChannelConfigurationsWidget *w =
            qobject_cast<ChannelConfigurationsWidget *>(parent());
        if (!w->isDefaultPath(index)) return;

        const QPoint right = option.rect.topRight();
        painter->drawPixmap(right.x() - 24,
                            right.y() + option.rect.height() / 2 - 8,
                            16,
                            16,
                            m_star);
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const {
        return QSize(0, 22);
    }

  private:
    QPixmap m_star;
};

ChannelConfigurationsWidget::ChannelConfigurationsWidget(Device *device,
                                                         QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ChannelConfigurationsWidget),
      m_model(new ChannelConfigurationsModel(this)),
      m_import(new QAction(tr("Import"), this)),
      m_defaultPath("") {
    ui->setupUi(this);
    performStandardSetup(tr("Channel Configurations"));
    // menuBar()->addAction(m_import);

    ui->configs->setModel(m_model);
    ui->configs->setItemDelegate(new ConfigItemDelegate(this));

    ui->configs->setRootIndex(m_model->index(m_model->rootPath()));

    connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
    connect(ui->rename, SIGNAL(clicked()), SLOT(rename()));
    connect(ui->default_, SIGNAL(clicked()), SLOT(default_()));
    connect(ui->add, SIGNAL(clicked()), SLOT(add()));
    connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->configs->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            SLOT(currentChanged(QModelIndex)));

    m_defaultPath = m_model->filePath(m_model->defaultConfiguration());

    currentChanged(QModelIndex());
}

ChannelConfigurationsWidget::~ChannelConfigurationsWidget() { delete ui; }

bool ChannelConfigurationsWidget::isDefaultPath(
    const QModelIndex &index) const {
    return m_model->filePath(index) == m_defaultPath;
}

void ChannelConfigurationsWidget::edit() {
    QItemSelection selection = ui->configs->selectionModel()->selection();
    if (selection.indexes().size() != 1) return;
    QModelIndex index = selection.indexes()[0];
    ChannelsWidget *widget = new ChannelsWidget(device());
    widget->setFile(m_model->filePath(index));
    RootController::ref().presentWidget(widget);
}

void ChannelConfigurationsWidget::rename() {
    QItemSelection selection = ui->configs->selectionModel()->selection();
    if (selection.indexes().size() != 1) return;
    QModelIndex index = selection.indexes()[0];

    QFileInfo file = m_model->fileInfo(index);
    KeyboardDialog keyboard(tr("Rename %1").arg(file.fileName()));
    keyboard.setInput(file.baseName());
    RootController::ref().presentDialog(&keyboard);
    if (!QFile::rename(file.filePath(),
                       file.path() + "/" + keyboard.input() + "." +
                           file.completeSuffix()))
    {
        qWarning() << "Failed to change name";
        // TODO: Make this error user visible
        return;
    }
}

void ChannelConfigurationsWidget::default_() {
    QItemSelection selection = ui->configs->selectionModel()->selection();
    if (selection.indexes().size() != 1) return;
    QModelIndex index = selection.indexes()[0];
    m_defaultPath = m_model->filePath(index);
    ui->default_->setEnabled(false);
    kipr::camera::ConfigPath::setDefaultConfigPath(
        m_model->fileInfo(index).baseName().toStdString());
    ui->configs->repaint();

    RootController::ref().dismissWidget();
    RootController::ref().presentWidget(
        new ChannelConfigurationsWidget(device()));
}

void ChannelConfigurationsWidget::add() {
    KeyboardDialog keyboard(tr("Create New Configuration"));
    RootController::ref().presentDialog(&keyboard);
    kipr::config::Config blank;
    std::string savePath =
        kipr::camera::ConfigPath::path(keyboard.input().toStdString());
    QString qSavePath = QString::fromStdString(savePath);
    if (!blank.save(savePath)) {
        qWarning() << "Error saving" << qSavePath;
        return;
    }

    // Select it and set as default if it's the first one
    QDir saves = QDir(
        QFileInfo(qSavePath).path(),
        "*." + QString::fromStdString(kipr::camera::ConfigPath::extension()));
    if (saves.entryList(QDir::Files).size() == 1) {
        QModelIndex index = m_model->index(qSavePath);
        ui->configs->selectionModel()->select(index,
                                              QItemSelectionModel::Select);
        default_();
        currentChanged(index);
    }
}

void ChannelConfigurationsWidget::remove() {
    QItemSelection selection = ui->configs->selectionModel()->selection();
    if (selection.indexes().size() != 1) return;
    QFile::remove(m_model->filePath(selection.indexes()[0]));
    currentChanged(QModelIndex());
}

void ChannelConfigurationsWidget::currentChanged(const QModelIndex &index) {
    const bool enable = index.isValid();
    ui->edit->setEnabled(enable);
    ui->rename->setEnabled(enable);
    ui->default_->setEnabled(enable && !isDefaultPath(index));
    ui->remove->setEnabled(enable);
    // m_import->setEnabled(enable);
}
