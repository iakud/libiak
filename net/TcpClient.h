#ifndef TOT_TCPCLIENT_H
#define TOT_TCPCLIENT_H

#include "InetAddress.h"
#include "TcpConnection.h"

#include <base/NonCopyable.h>

#include <memory>
#include <functional>

namespace iak {

class EventLoop;
class Connector;

class TcpClient;
typedef std::shared_ptr<TcpClient> TcpClientPtr;

class TcpClient : public NonCopyable, 
		public std::enable_shared_from_this<TcpClient> {
public:
	explicit TcpClient(EventLoop* loop, const InetAddress& remoteAddr);
	virtual ~TcpClient();

	typedef std::function<void(TcpConnectionPtr)> ConnectCallback;

	static TcpClientPtr create(EventLoop* loop, 
		const InetAddress& remoteAddr);

	void setRetry(bool retry) { retry_ = retry; }

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void connectAsync();
private:
	void handleConnect(const int sockFd,
			const struct sockaddr_in& localSockAddr);
	void handleClose(TcpConnectionPtr connection);

	EventLoop* loop_;
	InetAddress remoteAddr_;
	std::shared_ptr<Connector> connector_;
	bool connect_;
	std::shared_ptr<TcpConnection> connection_;

	bool retry_;
	ConnectCallback connectCallback_;
}; // end class TcpClient

} // end namespace iak

#endif // TOT_TCPCLIENT_H
