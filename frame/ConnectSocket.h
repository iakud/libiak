#ifndef IAK_FRAME_CONNECTSOCKET_H
#define IAK_FRAME_CONNECTSOCKET_H

#include "DataSocket.h"

#include <base/NonCopyable.h>
#include <net/InetAddress.h>

#include <memory>
#include <functional>

namespace iak {

class TcpClient;
class TcpConnection;

class ConnectSocket;
typedef std::shared_ptr<ConnectSocket> ConnectSocketPtr;

class ConnectSocket : public NonCopyable,
	public std::enable_shared_from_this<ConnectSocket> {
public:
	ConnectSocket(const InetAddress& remoteAddr);
	~ConnectSocket();

	typedef std::function<bool(std::shared_ptr<DataSocket>)> ConnectCallback;

	static ConnectSocketPtr create(const InetAddress& remoteAddr);

	void setConnectCallback(ConnectCallback&& cb) { connectCallback_ = cb; }

	void connect();
private:
	void onConnect(std::shared_ptr<TcpConnection> connection);
	void handleConnect(std::shared_ptr<TcpConnection> connection);
	void onClose(std::shared_ptr<DataSocket> datasocket);

	std::shared_ptr<TcpClient> client_;
	std::shared_ptr<DataSocket> datasocket_;

	ConnectCallback connectCallback_;
}; // end class ConnectSocket

} // end namespace iak

#endif // IAK_FRAME_CONNECTSOCKET_H