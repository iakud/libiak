#ifndef IAK_NET_INETADDRESS_H
#define IAK_NET_INETADDRESS_H

#include <arpa/inet.h>
#include <stdint.h>

namespace iak {
namespace net {

class InetAddress {
public:
	InetAddress() {
	}

	InetAddress(uint16_t port, const char* ip);

	InetAddress(const struct sockaddr_in& addr)
		: addr_(addr) {
	}

	const struct sockaddr_in& getSockAddr() const {
		return addr_;
	}

	void setSockAddr(const struct sockaddr_in& addr) {
		addr_ = addr;
	}

private:
	struct sockaddr_in addr_;
}; // end class InetAddress

} // end namespace net
} // end namespace iak

#endif // IAK_NET_INETADDRESS_H