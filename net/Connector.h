#ifndef IAK_NET_CONNECTOR_H
#define IAK_NET_CONNECTOR_H

#include "InetAddress.h"

#include <memory>
#include <functional>

#include <arpa/inet.h>

namespace iak {
namespace net {

class EventLoop;
class Channel;

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class Connector : public std::enable_shared_from_this<Connector> {

public:
	typedef std::function<void(int sockFd, const struct sockaddr_in& localSockAddr)> ConnectCallback;
	static ConnectorPtr make(EventLoop* loop, const struct sockaddr_in& peerSockAddr);

public:
	explicit Connector(EventLoop* loop, const struct sockaddr_in& peerSockAddr);
	~Connector();
	// noncopyable
	Connector(const Connector&) = delete;
	Connector& operator=(const Connector&) = delete;

	void setConnectCallback(ConnectCallback&& cb) { connectCallback_ = cb; }

	void connect();
	void close();

private:
	void connectInLoop();	// connect in loop
	void closeInLoop();	// close in loop
	void resetChannel();	// reset
	void onWrite();// write event active

	EventLoop* loop_;
	const struct sockaddr_in& peerSockAddr_;
	bool connect_;
	bool connecting_;
	std::unique_ptr<Channel> channel_;

	ConnectCallback connectCallback_;
}; // end class Connector

} // end namespace net
} // end namespace iak

#endif // IAK_NET_CONNECTOR_H
