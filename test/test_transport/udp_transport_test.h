#ifndef udp_server_h
#define udp_server_h

#include <pump/service.h>
#include <pump/time/timer.h>
#include <pump/transport/udp_transport.h>
#include <stdio.h>

namespace pump {
using namespace transport;
}

using namespace pump;

extern void start_udp_server(const std::string &ip, uint16_t port);

extern void start_udp_client(const std::string &ip, uint16_t port);

#endif