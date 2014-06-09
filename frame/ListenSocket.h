#ifndef IAK_FRAME_LISTENSOCKET_H
#define IAK_FRAME_LISTENSOCKET_H

#include "DataSocket.h"

#include <base/NonCopyable.h>

#include <memory>

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

	void listen();
private:
	void onAccept(std::shared_ptr<TcpConnection> connection);
	void handleAccept(std::shared_ptr<TcpConnection> connection);

	std::shared_ptr<TcpServer> server_;

	ConnectCallback connectCallback_;
};

#endif // IAK_FRAME_LISTENSOCKET_H