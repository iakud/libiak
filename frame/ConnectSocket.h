#ifndef IAK_FRAME_CONNECTSOCKET_H
#define IAK_FRAME_CONNECTSOCKET_H

#include "DataSocket.h"

#include <net/InetAddress.h>
#include <net/TcpClient.h>

#include <memory>
#include <functional>

namespace iak {

class ConnectSocket;
typedef std::shared_ptr<ConnectSocket> ConnectSocketPtr;

class ConnectSocket : public std::enable_shared_from_this<ConnectSocket> {
public:
	typedef std::function<bool(DataSocketPtr)> ConnectCallback;
	static ConnectSocketPtr make(const InetAddress& remoteAddr);

public:
	ConnectSocket(const InetAddress& remoteAddr);
	~ConnectSocket();
	// noncopyable
	ConnectSocket(const ConnectSocket&) = delete;
	ConnectSocket& operator=(const ConnectSocket&) = delete;

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void connect();

private:
	void onConnect(TcpConnectionPtr connection);
	void handleConnect(TcpConnectionPtr connection);
	void onClose(DataSocketPtr datasocket);

	TcpClientPtr client_;
	DataSocketPtr datasocket_;

	ConnectCallback connectCallback_;
}; // end class ConnectSocket

} // end namespace iak

#endif // IAK_FRAME_CONNECTSOCKET_H
