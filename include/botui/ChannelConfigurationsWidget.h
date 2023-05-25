#ifndef _CHANNELCONFIGURATIONSWIDGET_H_
#define _CHANNELCONFIGURATIONSWIDGET_H_

#include "StandardWidget.h"

#include <QModelIndex>

namespace Ui {
class ChannelConfigurationsWidget;
}

class ChannelConfigurationsModel;
class QAction;

class ChannelConfigurationsWidget : public StandardWidget {
    Q_OBJECT
  public:
    ChannelConfigurationsWidget(Device *device, QWidget *parent = 0);
    ~ChannelConfigurationsWidget();

    bool isDefaultPath(const QModelIndex &index) const;

  private slots:
    void edit();
    void rename();
    void default_();
    void add();
    void remove();

    void currentChanged(const QModelIndex &index);

  private:
    Ui::ChannelConfigurationsWidget *ui;
    ChannelConfigurationsModel *m_model;
    QAction *m_import;
    QString m_defaultPath;
};

#endif
