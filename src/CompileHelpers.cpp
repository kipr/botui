#include "CompileHelpers.h"

#include <QFileInfo>
#include <QRegularExpression>

QString CompileHelpers::postProcess(const QString &output) {
    QString ret = output;
    QRegularExpression rx(
        "([a-zA-Z]:)?(/[a-zA-Z0-9_.-]+)*([a-zA-Z0-9_-]+\\.[a-zA-Z]+)/?:");
    QRegularExpressionMatchIterator i = rx.globalMatch(ret);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString path =
            match.captured(1); // get the first substring that matched rx
        const QString ins = QFileInfo(path).fileName();

        int startOffset = match.capturedStart(1);
        ret.replace(startOffset, path.length(), ins);
    }
    return ret;
}

// int pos = 0;
// 	while((pos = rx.indexIn(ret, pos)) != -1) {
// 		const QString path = ret.mid(pos, rx.matchedLength() - 1);
// 		const QString ins = QFileInfo(path).fileName();
// 		ret.replace(pos, path.length(), ins);
// 		pos += ins.length();
// 	}