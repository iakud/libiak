#ifndef IAK_FRAME_LISTENSOCKET_H
#define IAK_FRAME_LISTENSOCKET_H

#include "DataSocket.h"

#include <base/NonCopyable.h>
#include <net/InetAddress.h>

#include <memory>
#include <functional>
#include <set>

namespace iak {

class TcpServer;
class TcpConnection;

class ListenSocket;
typedef std::shared_ptr<ListenSocket> ListenSocketPtr;

class ListenSocket : public NonCopyable,
	public std::enable_shared_from_this<ListenSocket> {
public:
	ListenSocket(const InetAddress& localAddr);
	~ListenSocket();

	typedef std::function<bool(std::shared_ptr<DataSocket>)> ConnectCallback;

	static ListenSocketPtr create(const InetAddress& localAddr);

	void setConnectCallback(ConnectCallback&& cb) { connectCallback_ = cb; }

	void listen();
private:
	void onAccept(std::shared_ptr<TcpConnection> connection);
	void handleAccept(std::shared_ptr<TcpConnection> connection);
	void onClose(std::shared_ptr<DataSocket> datasocket);

	std::shared_ptr<TcpServer> server_;
	std::set<DataSocketPtr> datasockets_;

	ConnectCallback connectCallback_;
}; // end class ListenSocket

} // end namespace iak

#endif // IAK_FRAME_LISTENSOCKET_H