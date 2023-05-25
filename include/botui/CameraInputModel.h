#ifndef _CAMERAINPUTMODEL_H_
#define _CAMERAINPUTMODEL_H_

#include <QStandardItemModel>

class CameraInputModel : public QStandardItemModel {
  public:
    CameraInputModel(QObject *parent = 0);
    ~CameraInputModel();
};

#endif
