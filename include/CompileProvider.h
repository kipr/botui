#ifndef _COMPILEPROVIDER_H_
#define _COMPILEPROVIDER_H_

#include <QObject>
#include <QString>

#include <kiss-compiler/Compilation.h>

class TinyArchive;

class CompileProvider : public QObject
{
Q_OBJECT
public:
	CompileProvider(QObject *parent = 0);
	virtual ~CompileProvider();
	virtual CompilationPtr compile(const QString& name, TinyArchive *archive) = 0;
	
signals:
	void compileFinished(CompileResult result);
	
};

#endif
