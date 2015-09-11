#ifndef IAK_NET_SOCKET_H
#define IAK_NET_SOCKET_H

#include <memory>

#include <arpa/inet.h>

namespace iak {
namespace net {

class Socket {
public:
	static int open();
	static int close(int sockFd);	
	static int bind(int sockFd, const struct sockaddr_in &addr);
	static int listen(int sockFd);
	static int accept(int sockFd, struct sockaddr_in *addr);
	static int connect(int sockFd, const struct sockaddr_in &addr);
	static int shutdownWrite(int sockFd);

	static int setReuseAddr(int sockFd, bool reuseaddr);
	static int setTcpNoDelay(int sockFd, bool nodelay);
	static int setKeepAlive(int sockFd, bool keepalive);
	static int setKeepIdle(int sockFd, int optval);

	static int getError(int sockFd, int *optval);
	static int getSockName(int sockFd, struct sockaddr_in *addr);
	static int getPeerName(int sockFd, struct sockaddr_in *addr);
}; // end class Socket

} // end namespace net
} // end namespace iak

#endif // IAK_NET_SOCKET_H