#ifndef IAK_NET_CONNECTOR_H
#define IAK_NET_CONNECTOR_H

#include "InetAddress.h"

#include <base/NonCopyable.h>

#include <arpa/inet.h>
#include <memory>
#include <functional>

namespace iak {

class EventLoop;
class Watcher;

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class Connector : public NonCopyable, 
	public std::enable_shared_from_this<Connector> {
public:
	explicit Connector(
		EventLoop* loop, const struct sockaddr_in& remoteSockAddr);

	virtual ~Connector();

	typedef std::function<void(int sockFd, 
		const struct sockaddr_in& localSockAddr)> ConnectCallback;

	static ConnectorPtr create(EventLoop* loop, 
		const struct sockaddr_in& remoteSockAddr);

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void connectAsync();
	void closeAsync();
private:
	void connect();	// connect in loop
	void close();	// close in loop
	void handleWrite();// write event active

	EventLoop* loop_;
	const struct sockaddr_in& remoteSockAddr_;
	bool connect_;
	bool connecting_;
	std::unique_ptr<Watcher> watcher_;

	ConnectCallback connectCallback_;
}; // end class Connector

} // end namespace iak

#endif // IAK_NET_CONNECTOR_H
