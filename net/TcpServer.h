#ifndef IAK_NET_TCPSERVER_H
#define IAK_NET_TCPSERVER_H

#include "InetAddress.h"
#include "TcpConnection.h"

#include <base/NonCopyable.h>

#include <memory>
#include <functional>
#include <map>

namespace iak {

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer;
typedef std::shared_ptr<TcpServer> TcpServerPtr;

class TcpServer : public NonCopyable,
	public std::enable_shared_from_this<TcpServer> {

public:
	typedef std::function<void(TcpConnectionPtr)> ConnectCallback;
	static TcpServerPtr make(EventLoop* loop,
			const InetAddress& localAddr);

public:
	explicit TcpServer(EventLoop* loop, const InetAddress& localAddr);
	~TcpServer();

	void setEventLoopThreadPool(EventLoopThreadPool* loopThreadPool) {
		loopThreadPool_ = loopThreadPool;
	}

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void listenAsync();

private:
	void onAccept(const int sockFd,
			const struct sockaddr_in& remoteSockAddr);
	void onClose(const int sockFd, TcpConnectionPtr connection);

	EventLoop* loop_;
	InetAddress localAddr_;
	std::shared_ptr<Acceptor> acceptor_;
	bool listen_;
	std::map<int, TcpConnectionPtr> connections_;

	EventLoopThreadPool* loopThreadPool_;
	uint32_t indexLoop_;
	ConnectCallback connectCallback_;
}; // end class TcpServer

} // end namespace iak

#endif // IAK_NET_TCPSERVER_H
