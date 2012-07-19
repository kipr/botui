#include "FilesystemProvider.h"

ProgramsItemModel::ProgramsItemModel(QObject *parent)
	: QStandardItemModel(parent)
{
	
}

QString ProgramsItemModel::program(const QModelIndex& index) const
{
	return itemFromIndex(index)->text();
}

FilesystemProvider::~FilesystemProvider() {}