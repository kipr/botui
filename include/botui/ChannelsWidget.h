#ifndef _CHANNELSWIDGET_H_
#define _CHANNELSWIDGET_H_

#include "StandardWidget.h"

#include <QPixmap>

namespace Ui {
class ChannelsWidget;
}

class CameraConfigModel;

class ChannelsWidget : public StandardWidget {
    Q_OBJECT
  public:
    ChannelsWidget(Device *device, QWidget *parent = 0);
    ~ChannelsWidget();

    void setFile(const QString &path);
    const QString &file() const;

  private slots:
    void configure();
    // void options();
    void up();
    void down();
    void add();
    void remove();

    void updateOptions();

  private:
    bool save() const;

    bool isFull();

    Ui::ChannelsWidget *ui;
    CameraConfigModel *m_model;
    QString m_path;
};

#endif
