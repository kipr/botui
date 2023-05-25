

#include "ConnectWidget.h"
#include "Device.h"
#include "MenuBar.h"
#include "NetworkItemModel.h"
#include "NetworkManager.h"
#include "Options.h"
#include "OtherNetworkWidget.h"
#include "RootController.h"
#include "StatusBar.h"
#include "ui_ConnectWidget.h"
#include <QDebug>

ConnectWidget::ConnectWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ConnectWidget),
      m_model(new NetworkItemModel(this)) {

    ui->setupUi(this);
    performStandardSetup(tr("Connect"));

    m_model->setNetworks(NetworkManager::ref().accessPoints());

    ui->networks->setModel(m_model);
    ui->connect->setEnabled(true);
    m_model->connect(&NetworkManager::ref(),
                     SIGNAL(accessPointAdded(Network)),
                     SLOT(addNetwork(Network)));
    m_model->connect(&NetworkManager::ref(),
                     SIGNAL(accessPointRemoved(Network)),
                     SLOT(removeNetwork(Network)));

    QObject::connect(ui->refresh, SIGNAL(clicked()), SLOT(refresh()));
    QObject::connect(ui->other, SIGNAL(clicked()), SLOT(other()));
    QObject::connect(ui->connect, SIGNAL(clicked()), SLOT(connect()));
    QObject::connect(ui->networks->selectionModel(),
                     SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                     SLOT(selectionChanged(QItemSelection)));
    selectionChanged(QItemSelection());
}

ConnectWidget::~ConnectWidget() { delete ui; }

void ConnectWidget::connect() {
    QModelIndexList selection =
        ui->networks->selectionModel()->selectedIndexes();
    if (selection.size() != 1) return;
    Network network = m_model->indexToNetwork(selection[0]);
    OtherNetworkWidget *other = new OtherNetworkWidget(device());
    other->fillNetworkInfo(network);
    RootController::ref().presentWidget(other);
}

void ConnectWidget::other() {
    RootController::ref().presentWidget(new OtherNetworkWidget(device()));
}

void ConnectWidget::refresh() { NetworkManager::ref().requestScan(); }

void ConnectWidget::selectionChanged(const QItemSelection &selection) {
    ui->connect->setEnabled(selection.indexes().size());
}
