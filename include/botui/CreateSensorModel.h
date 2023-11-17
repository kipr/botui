#ifndef _CREATESENSORMODEL_H_
#define _CREATESENSORMODEL_H_

#include <QStandardItemModel>

class CreateSensorModel : public QStandardItemModel
{
Q_OBJECT
public:

	enum SensorType {
		Bump0,
        Bump1,
        Bump2,
        Bump3,
        Bump4,
		Cliff0,
        Cliff1,
        Cliff2,
        Cliff3,
		IR0,
        IR1,
        IR2,
        IR3,
        IR4,
        IR5,
        IR6,
		Other
	};
	
    enum SensorMethod {
        Bump,
        Cliff,
        IR
    };
	CreateSensorModel(QObject *parent = 0);
	~CreateSensorModel();

	CreateSensorModel::SensorType type(const QModelIndex &index) const;

public slots:
	void update();
	
private:
	void populate();
	void populateBump(const unsigned char port);
	// void populateCliff(const unsigned char port);
    // void populateIR(const unsigned char port);
};

#endif
