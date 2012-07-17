#include "NetworkingProvider.h"

Network::Network(QObject *parent) : QObject(parent) {}
Network::~Network() {}

Network::Network() {}
Network::Network(const Network&) {}
Network& Network::operator=(Network&) { return *this; }

NetworkingProvider::~NetworkingProvider() {}