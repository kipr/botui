#ifndef _CREATETESTWIZARDPAGE_H_
#define _CREATETESTWIZARDPAGE_H_

#include <QWizardPage>

namespace Ui {
class CreateTestWizardPage;
}

class QTimer;

class CreateTestWizardPage : public QWizardPage {
    Q_OBJECT
  public:
    CreateTestWizardPage(QWidget *parent = 0);
    ~CreateTestWizardPage();

    void initializePage();
    void cleanupPage();

  private slots:
    void createConnect();
    void turn();

  private:
    Ui::CreateTestWizardPage *ui;
    QTimer *m_connectTimer;
};

#endif
