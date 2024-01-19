#ifndef _CREATE3SENSORMODEL_H_
#define _CREATE3SENSORMODEL_H_

#include <QStandardItemModel>

class Create3SensorModel : public QStandardItemModel
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
	Create3SensorModel(QObject *parent = 0);
	~Create3SensorModel();

	Create3SensorModel::SensorType type(const QModelIndex &index) const;

public slots:
	void update();
	
private:
	void populate();
	void populateBump(unsigned int port);
	void populateCliff(unsigned int port);
    void populateIR(unsigned int port);
};

#endif
