#include "SensorModel.h"

SensorModel::SensorModel(QObject *parent)
	: QStandardItemModel(parent)
{
}

SensorModel::~SensorModel()
{
}

SensorModel::SensorType SensorModel::type(const QModelIndex &index) const
{
	return SensorModel::Other;
}

void SensorModel::update()
{
	
}