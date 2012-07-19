#ifndef _KOVANPROGRAMSITEMMODEL_H_
#define _KOVANPROGRAMSITEMMODEL_H_

#include "FilesystemProvider.h"

namespace Kovan
{
	class FilesystemProvider;

	class ProgramsItemModel : public ::ProgramsItemModel
	{
	Q_OBJECT
	public:
		ProgramsItemModel(FilesystemProvider *filesystemProvider, QObject *parent = 0);
	
	public slots:
		void programUpdated(const QString& name);
		void programDeleted(const QString& name);
	
	private:
		FilesystemProvider *m_filesystemProvider;
	};
}

#endif
