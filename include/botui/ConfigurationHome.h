#ifndef CONFIGURATIONHOME_H
#define CONFIGURATIONHOME_H

#include <QDir>

#include <pwd.h>
#include <unistd.h>

namespace ConfigurationHome
{
	inline QString path()
	{
		if (geteuid() == 0) {
			bool valid = false;
			const uint userId = qgetenv("SUDO_UID").toUInt(&valid);
			if (valid) {
				const passwd *user = getpwuid(userId);
				if (user && user->pw_dir)
					return QString::fromLocal8Bit(user->pw_dir);
			}
		}
		return QDir::homePath();
	}
}

#endif
