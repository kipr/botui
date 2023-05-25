#ifndef _CONFIGURATIONPATHS_H_
#define _CONFIGURATIONPATHS_H_

#include "Singleton.h"

#include <QString>

class ConfigurationPaths : public Singleton<ConfigurationPaths> {
  public:
    ConfigurationPaths();
};

#endif
