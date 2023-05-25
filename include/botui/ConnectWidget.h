

#ifndef _CONNECTWIDGET_H_
#define _CONNECTWIDGET_H_

#include "Options.h"
#include "StandardWidget.h"
#include <QItemSelection>
#include <QObject>
namespace Ui {
class ConnectWidget;
}

class NetworkItemModel;

class ConnectWidget : public StandardWidget {
    Q_OBJECT
  public:
    ConnectWidget(Device *device, QWidget *parent = 0);
    ~ConnectWidget();

  public slots:
    void connect();
    void other();
    void refresh();
    void selectionChanged(const QItemSelection &selection);

  private:
    Ui::ConnectWidget *ui;
    NetworkItemModel *m_model;
};

#endif
