#ifndef _SENSORMODEL_H_
#define _SENSORMODEL_H_

#include <QStandardItemModel>

class SensorModel : public QStandardItemModel {
    Q_OBJECT
  public:
    enum SensorType {
        Analog,
        Digital,
        AccelX,
        AccelY,
        AccelZ,
#ifdef WOMBAT
        GyroX,
        GyroY,
        GyroZ,
        MagnetoX,
        MagnetoY,
        MagnetoZ,
        Button,
#endif
        Other
    };

    SensorModel(QObject *parent = 0);
    ~SensorModel();

    SensorModel::SensorType type(const QModelIndex &index) const;

  public slots:
    void update();

  private:
    void populate();
    void populateAnalog(const unsigned char port);
    void populateDigital(const unsigned char port);
    void populateAccel();
#ifdef WOMBAT
    void populateGyro();
    void populateMagneto();
    void populateButtons();
#endif
};

#endif
