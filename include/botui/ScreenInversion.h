#ifndef SCREENINVERSION_H
#define SCREENINVERSION_H

#include <QString>

namespace ScreenInversion
{
	void showPage();
	bool recoverPending(QString *errorMessage);
	bool synchronizeConfiguredOutput(QString *errorMessage);
}

#endif
