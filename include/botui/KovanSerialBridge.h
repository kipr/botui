#ifndef _KOVANSERIALBRIDGE_H_
#define _KOVANSERIALBRIDGE_H_

#include <QObject>

#include "Singleton.h"

class Device;

class KovanSerialBridge : public QObject, public Singleton<KovanSerialBridge> {
    Q_OBJECT
  public:
    KovanSerialBridge();

    void init(Device *device);

  private slots:
    void newConnection();
    void run(const QString &program);

  private:
    Device *m_device;
};

#endif