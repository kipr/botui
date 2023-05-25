#ifndef _CHANNELCONFIGWIDGETFACTORY_H_
#define _CHANNELCONFIGWIDGETFACTORY_H_

#include <QModelIndex>
#include <QString>

class ChannelConfigWidget;

class ChannelConfigWidgetFactory {
  public:
    ~ChannelConfigWidgetFactory();

    static bool hasConfig(const QString &type);
    static ChannelConfigWidget *create(const QModelIndex &index,
                                       const QString &type);

  private:
    ChannelConfigWidgetFactory();
};

#endif
