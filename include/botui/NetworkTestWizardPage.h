#include "Options.h"

#ifdef NETWORK_ENABLED

#ifndef _NETWORKTESTWIZARDPAGE_H_
#define _NETWORKTESTWIZARDPAGE_H_

#include <QWizardPage>

namespace Ui {
class NetworkTestWizardPage;
}

class NetworkItemModel;

class NetworkTestWizardPage : public QWizardPage {
  public:
    NetworkTestWizardPage(QWidget *parent = 0);
    ~NetworkTestWizardPage();

    void initializePage();
    void cleanupPage();

  private:
    Ui::NetworkTestWizardPage *ui;
    NetworkItemModel *m_model;
};

#endif

#endif