#ifndef _PROGRAMARGUMENTS_H_
#define _PROGRAMARGUMENTS_H_

#include <kar.hpp>
#include <QStringList>

class ProgramArguments
{
public:
	static void setArguments(const Kiss::KarPtr &archive, const QStringList &arguments);
	static QStringList arguments(const Kiss::KarPtr &archive);
};

#endif
