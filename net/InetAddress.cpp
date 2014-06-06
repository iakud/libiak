#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, const char* ip) {
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	::inet_pton(AF_INET, ip, &addr_.sin_addr);
}
