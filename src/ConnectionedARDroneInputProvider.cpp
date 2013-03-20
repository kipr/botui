#include "ConnectionedARDroneInputProvider.h"

using namespace Camera;

bool ConnectionedARDroneInputProvider::open(const int number)
{
	if(!ARDrone::instance()->connect()) return false;
	bool ret = ARDroneInputProvider::open(number);
	return ret;
}

bool ConnectionedARDroneInputProvider::close()
{
	bool ret = ARDroneInputProvider::close();
	if(ret) ARDrone::instance()->disconnect();
	return ret;
}