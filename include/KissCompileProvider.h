#ifndef _KISSCOMPILEPROVIDER_H_
#define _KISSCOMPILEPROVIDER_H_

#include "CompileProvider.h"
#include <TinyArchive.h>

class KissCompileProvider : public CompileProvider
{
public:
	KissCompileProvider(QObject *parent = 0);
	
	CompilationPtr compile(const QString& name, TinyArchive *archive);
};

#endif
