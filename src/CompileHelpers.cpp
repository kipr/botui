#include "CompileHelpers.h"

#include <QFileInfo>
#include <QRegExp>

QString CompileHelpers::postProcess(const QString &output)
{
	QString ret = output;
	QRegExp rx("([a-zA-Z]:)?(/[a-zA-Z0-9_.-]+)*([a-zA-Z0-9_-]+\\.[a-zA-Z]+)/?:");
	int pos = 0;
	while((pos = rx.indexIn(ret, pos)) != -1) {
		const QString path = ret.mid(pos, rx.matchedLength() - 1);
		const QString ins = QFileInfo(path).fileName();
		ret.replace(pos, path.length(), ins);
		pos += ins.length();
	}
	return ret;
}
