#ifndef IAK_NET_CONNECTOR_H
#define IAK_NET_CONNECTOR_H

#include "InetAddress.h"

#include <arpa/inet.h>
#include <memory>
#include <functional>

namespace iak {
namespace net {

class EventLoop;
class Watcher;

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class Connector : public std::enable_shared_from_this<Connector> {

public:
	typedef std::function<void(int sockFd,
			const struct sockaddr_in& localSockAddr)> ConnectCallback;
	static ConnectorPtr make(EventLoop* loop,
			const struct sockaddr_in& remoteSockAddr);

public:
	explicit Connector(EventLoop* loop,
			const struct sockaddr_in& remoteSockAddr);
	~Connector();
	// noncopyable
	Connector(const Connector&) = delete;
	Connector& operator=(const Connector&) = delete;

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void connectAsync();
	void closeAsync();

private:
	void connect();	// connect in loop
	void resetWatcher();	// reset
	void close();	// close in loop
	void onWrite();// write event active

	EventLoop* loop_;
	const struct sockaddr_in& remoteSockAddr_;
	bool connect_;
	bool connecting_;
	std::unique_ptr<Watcher> watcher_;

	ConnectCallback connectCallback_;
}; // end class Connector

} // end namespace net
} // end namespace iak

#endif // IAK_NET_CONNECTOR_H
