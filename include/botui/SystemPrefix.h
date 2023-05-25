#ifndef _SYSTEM_PREFIX_H_
#define _SYSTEM_PREFIX_H_

#include "Singleton.h"
#include <pcompiler/root_manager.hpp>

class SystemPrefix : public Singleton<SystemPrefix> {
  public:
    SystemPrefix();
    Compiler::RootManager *rootManager();

  private:
    Compiler::RootManager _rootManager;
};

#endif