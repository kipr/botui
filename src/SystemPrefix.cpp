#include "SystemPrefix.h"

using namespace Compiler;

SystemPrefix::SystemPrefix() : _rootManager("/kovan") {}

RootManager *SystemPrefix::rootManager() { return &_rootManager; }
