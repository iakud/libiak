#ifndef IAK_FRAME_LISTENSOCKET_H
#define IAK_FRAME_LISTENSOCKET_H

#include "DataSocket.h"

#include <net/InetAddress.h>
#include <net/TcpServer.h>

#include <memory>
#include <functional>
#include <set>

namespace iak {

class ListenSocket;
typedef std::shared_ptr<ListenSocket> ListenSocketPtr;

class ListenSocket : public std::enable_shared_from_this<ListenSocket> {

public:
	typedef std::function<bool(DataSocketPtr)> ConnectCallback;
	static ListenSocketPtr make(const InetAddress& localAddr);

public:
	explicit ListenSocket(const InetAddress& localAddr);
	~ListenSocket();
	// noncopyable
	ListenSocket(const ListenSocket&) = delete;
	ListenSocket& operator=(const ListenSocket&) = delete;

	void setConnectCallback(ConnectCallback&& cb) {
		connectCallback_ = cb;
	}

	void listen();

private:
	void onAccept(TcpConnectionPtr connection);
	void handleAccept(TcpConnectionPtr connection);
	void onClose(DataSocketPtr datasocket);

	TcpServerPtr server_;
	std::set<DataSocketPtr> datasockets_;

	ConnectCallback connectCallback_;
}; // end class ListenSocket

} // end namespace iak

#endif // IAK_FRAME_LISTENSOCKET_H
