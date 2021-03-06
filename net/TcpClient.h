#ifndef TOT_TCPCLIENT_H
#define TOT_TCPCLIENT_H

#include "InetAddress.h"
#include "TcpConnection.h"

#include <memory>
#include <functional>

namespace iak {
namespace net {

class EventLoop;
class Connector;

class TcpClient;
typedef std::shared_ptr<TcpClient> TcpClientPtr;

class TcpClient : public std::enable_shared_from_this<TcpClient> {

public:
	typedef std::function<void(TcpConnectionPtr)> ConnectCallback;
	static TcpClientPtr make(EventLoop* loop,
			const InetAddress& peerAddr);

public:
	explicit TcpClient(EventLoop* loop,
			const InetAddress& peerAddr);
	~TcpClient();
	// noncopyable
	TcpClient(const TcpClient&) = delete;
	TcpClient& operator=(const TcpClient&) = delete;

	void setRetry(bool retry) { retry_ = retry; }

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void connect();
	
private:
	void onConnect(const int sockFd,
			const struct sockaddr_in& localSockAddr);
	void onClose(TcpConnectionPtr connection);

	EventLoop* loop_;
	InetAddress peerAddr_;
	std::shared_ptr<Connector> connector_;
	bool connect_;
	std::shared_ptr<TcpConnection> connection_;

	bool retry_;
	ConnectCallback connectCallback_;
}; // end class TcpClient

} // end namespace net
} // end namespace iak

#endif // TOT_TCPCLIENT_H
