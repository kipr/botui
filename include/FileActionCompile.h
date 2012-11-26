#ifndef _FILEACTIONCOMPILE_H_
#define _FILEACTIONCOMPILE_H_

#include "FileActions.h"

#include <QObject>
#include <QString>
#include <pcompiler/output.hpp>

class Device;
class ConcurrentCompile;

class FileActionCompile : public QObject, public FileAction
{
Q_OBJECT
public:
	FileActionCompile();
	virtual bool act(const QString &path, Device *device) const;
	
public slots:
	void compileStarted(const QString &name, ConcurrentCompile *compiler);
	void compileFinished(const Compiler::OutputList &output, ConcurrentCompile *compiler);

private:
	QString postProcess(const QString &output) const;
};

#endif
