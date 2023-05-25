#include "CommunicationProvider.h"

CommunicationProvider::CommunicationProvider(Device *device)
    : m_device(device) {}

CommunicationProvider::~CommunicationProvider() {}

void CommunicationProvider::setDevice(Device *device) { m_device = device; }

Device *CommunicationProvider::device() const { return m_device; }