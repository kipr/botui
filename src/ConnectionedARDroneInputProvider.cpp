/* FIXME
#include "ConnectionedARDroneInputProvider.h"

using namespace Camera;

ConnectionedARDroneInputProvider::ConnectionedARDroneInputProvider()
    : m_open(false)
{
}

bool ConnectionedARDroneInputProvider::open(const int number)
{
    if(m_open) return false;
    if(!ARDrone::instance()->connect()) return false;
    m_open = ARDroneInputProvider::open(number);
    return m_open;
}

bool ConnectionedARDroneInputProvider::close()
{
    if(!m_open) return false;
    bool ret = ARDroneInputProvider::close();
    if(ret) ARDrone::instance()->disconnect();
    m_open = false;
    return ret;
}
*/
