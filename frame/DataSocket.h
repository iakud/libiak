#ifndef IAK_FRAME_DATASOCKET_H
#define IAK_FRAME_DATASOCKET_H

#include <base/NonCopyable.h>
#include <net/InetAddress.h>
#include <net/Packet.h>
#include <net/TcpConnection.h>

#include <memory>

namespace iak {

class DataSocket;
typedef std::shared_ptr<DataSocket> DataSocketPtr;

class DataSocket : public NonCopyable,
	public std::enable_shared_from_this<DataSocket> {

public:
	typedef std::function<void(DataSocketPtr)> ConnectCallback;
	typedef std::function<void(DataSocketPtr, PacketPtr)> MessageCallback;
	typedef std::function<void(DataSocketPtr)> DisconnectCallback;
	typedef std::function<void(DataSocketPtr)> CloseCallback;
	static DataSocketPtr make(TcpConnectionPtr connection, CloseCallback&& cb);

public:
	DataSocket(TcpConnectionPtr connection, CloseCallback&& cb);
	~DataSocket();

	void setConnectCallback(ConnectCallback&& cb) { 
		connectCallback_ = cb;
	}

	void setMessageCallback(MessageCallback&& cb) {
		messageCallback_ = cb;
	}

	void setDisconnectCallback(DisconnectCallback&& cb) {
		disconnectCallback_ = cb;
	}

	void setUserData(std::shared_ptr<void> userdata) {
		userdata_ = userdata;
	}

	std::shared_ptr<void> getUserData() {
		return userdata_;
	}

	void close();
	void sendPack(PacketPtr packet);

private:
	void onConnect(TcpConnectionPtr connection);
	void onMessage(TcpConnectionPtr connection, PacketPtr packet);
	void onDisconnect(TcpConnectionPtr connection);

	void handleConnect(TcpConnectionPtr connection);
	void handleMessage(TcpConnectionPtr connection, PacketPtr packet);
	void handleDisconnect(TcpConnectionPtr connection);

	TcpConnectionPtr connection_;

	ConnectCallback connectCallback_;
	MessageCallback messageCallback_;
	DisconnectCallback disconnectCallback_;
	CloseCallback closeCallback_;

	std::shared_ptr<void> userdata_;
}; // end class DataSocket

} // end namespace iak

#endif // IAK_FRAME_DATASOCKET_H
