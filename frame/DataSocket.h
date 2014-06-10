#ifndef IAK_FRAME_DATASOCKET_H
#define IAK_FRAME_DATASOCKET_H

#include <base/NonCopyable.h>
#include <net/InetAddress.h>
#include <net/Packet.h>

#include <memory>

namespace iak {

class TcpConnection;

class DataSocket;
typedef std::shared_ptr<DataSocket> DataSocketPtr;

class DataSocket : public NonCopyable,
	public std::enable_shared_from_this<DataSocket> {
public:
	typedef std::function<void(DataSocketPtr)> ConnectCallback;
	typedef std::function<void(DataSocketPtr, PacketPtr)> MessageCallback;
	typedef std::function<void(DataSocketPtr)> DisconnectCallback;
	typedef std::function<void(DataSocketPtr)> CloseCallback;

public:
	DataSocket(std::shared_ptr<TcpConnection> connection,
			CloseCallback& cb);
	~DataSocket();

	static DataSocketPtr create(std::shared_ptr<TcpConnection> connection,
			CloseCallback&& cb);

	void setConnectCallback(ConnectCallback&& cb) { 
		connectCallback_ = cb;
	}

	void setMessageCallback(MessageCallback&& cb) {
		messageCallback_ = cb;
	}

	void setDisconnectCallback(DisconnectCallback&& cb) {
		disconnectCallback_ = cb;
	}

	void sendPack(PacketPtr packet);
private:
	void onConnect(std::shared_ptr<TcpConnection> connection);
	void onMessage(std::shared_ptr<TcpConnection> connection,
			PacketPtr packet);
	void onDisconnect(std::shared_ptr<TcpConnection> connection);
	void handleConnect(std::shared_ptr<TcpConnection> connection);
	void handleMessage(std::shared_ptr<TcpConnection> connection,
			PacketPtr packet);
	void handleDisconnect(std::shared_ptr<TcpConnection> connection);

	std::shared_ptr<TcpConnection> connection_;

	ConnectCallback connectCallback_;
	MessageCallback messageCallback_;
	DisconnectCallback disconnectCallback_;
	CloseCallback closeCallback_;
}; // end class DataSocket

} // end namespace iak
\
#endif // IAK_FRAME_DATASOCKET_H
