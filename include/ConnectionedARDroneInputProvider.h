#ifndef _CONNECTIONEDARDRONEINPUTPROVIDER_H_
#define _CONNECTIONEDARDRONEINPUTPROVIDER_H_

#include <kovan/ardrone.hpp>

namespace Camera
{
	class ConnectionedARDroneInputProvider : public ARDroneInputProvider
	{
	public:
		virtual bool open(const int number);
		virtual bool close();
	};
}

#endif
