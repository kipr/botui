#include "Options.h"

#ifdef NETWORK_ENABLED

#include "ManageNetworksWidget.h"

#include "ui_ManageNetworks.h"

#include "NetworkItemModel.h"
#include "NetworkManager.h"

#include <QItemSelectionModel>

ManageNetworksWidget::ManageNetworksWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ManageNetworks),
      m_device(device),
      m_model(new NetworkItemModel(this)) {
    ui->setupUi(this);
    performStandardSetup(tr("Manage Networks"));

    m_model->connect(&NetworkManager::ref(),
                     SIGNAL(networkAdded(Network)),
                     SLOT(addNetwork(Network)));

    m_model->connect(&NetworkManager::ref(),
                     SIGNAL(networkForgotten(Network)),
                     SLOT(removeNetwork(Network)));

    ui->networks->setModel(m_model);

    m_model->setNetworks(NetworkManager::ref().networks());

    connect(ui->forget, SIGNAL(clicked()), SLOT(forget()));
}

ManageNetworksWidget::~ManageNetworksWidget() { delete ui; }

void ManageNetworksWidget::forget() {
    QItemSelectionModel *selectionModel = ui->networks->selectionModel();
    QItemSelection selection = selectionModel->selection();
    if (selection.indexes().size() != 1) return;

    NetworkManager::ref().forgetNetwork(
        m_model->indexToNetwork(selection.indexes()[0]));
}

#endif