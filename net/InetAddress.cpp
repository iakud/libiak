#include "InetAddress.h"

#include <string.h>

using namespace iak::net;

InetAddress::InetAddress(uint16_t port, const char* ip) {
	::bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	::inet_pton(AF_INET, ip, &addr_.sin_addr);
}
