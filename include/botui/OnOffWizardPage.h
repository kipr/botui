#ifndef _ONOFFWIZARDPAGE_H_
#define _ONOFFWIZARDPAGE_H_

#include <QWizardPage>

namespace Ui {
class OnOffWizardPage;
}

class OnOffWizardPage : public QWizardPage {
    Q_OBJECT
  public:
    OnOffWizardPage(QWidget *parent = 0);
    ~OnOffWizardPage();

  private slots:
    void on_allOff_clicked();
    void on_allOn_clicked();

    void clicked();

  signals:
    void on(const quint16 &port);
    void off(const quint16 &port);

  private:
    Ui::OnOffWizardPage *ui;

    QPushButton *m_on[4];
    QPushButton *m_off[4];
};

#endif
