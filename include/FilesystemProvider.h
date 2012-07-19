#ifndef _FILESYSTEMPROVIDER_H_
#define _FILESYSTEMPROVIDER_H_

#include <TinyArchive.h>
#include <QString>
#include <QStandardItemModel>
#include <QStringList>

class QAbstractItemModel;

class ProgramsItemModel : public QStandardItemModel
{
public:
	ProgramsItemModel(QObject *parent = 0);
	
	QString program(const QModelIndex& index) const;
};

class FilesystemProvider : public QObject
{
Q_OBJECT
public:
	virtual ~FilesystemProvider();
	
	virtual const bool setProgram(const QString& name, const TinyArchive *archive) = 0;
	virtual const bool deleteProgram(const QString& name) = 0;
	virtual TinyArchive *program(const QString& name) const = 0;
	virtual const QStringList programs() const = 0;
	virtual ProgramsItemModel *programsItemModel() const = 0;
	
signals:
	void programUpdated(const QString& name, const TinyArchive *archive);
	void programDeleted(const QString& name);
	
private:
	
};

#endif
