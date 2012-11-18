#ifndef _COMPILEPROVIDER_H_
#define _COMPILEPROVIDER_H_

#include <QObject>
#include <QString>

#include <kar.hpp>
#include <pcompiler/output.hpp>

class TinyArchive;

class CompileProvider : public QObject
{
Q_OBJECT
public:
	CompileProvider(QObject *parent = 0);
	virtual ~CompileProvider();
	virtual Compiler::OutputList compile(const QString& name, const Kiss::KarPtr& program) = 0;
	
	virtual QString executableFor(const QString& name) const = 0;
	
signals:
	void compileFinished(const Compiler::OutputList& output);
};

#endif
