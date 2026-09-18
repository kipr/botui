#include "SystemPrefix.h"

using namespace Compiler;

SystemPrefix::SystemPrefix()
  : _rootManager("/wallaby")
{
}

RootManager *SystemPrefix::rootManager()
{
  return &_rootManager;
}
