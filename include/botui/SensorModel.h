#ifndef _SENSORMODEL_H_
#define _SENSORMODEL_H_
/* FIXME
#include <QStandardItemModel>

class SensorModel : public QStandardItemModel
{
Q_OBJECT
public:
	enum SensorType {
		Analog,
		Digital,
		AccelX,
		AccelY,
		AccelZ,
		Other
	};
	
	SensorModel(QObject *parent = 0);
	~SensorModel();

	SensorModel::SensorType type(const QModelIndex &index) const;
  
  void setPullUp(const QModelIndex &index, const bool pullup);
  bool pullUp(const QModelIndex &index) const;

public slots:
	void update();
	
private:
	void populate();
	void populateAnalog(const unsigned char port);
	void populateDigital(const unsigned char port);
	void populateAccel();
};
*/
#endif
