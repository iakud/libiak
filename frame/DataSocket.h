#ifndef TOT_DATASOCKET_H
#define TOT_DATASOCKET_H

#include <memory>
#include "NonCopyable.h"
#include "Packet.h"

class TcpConnection;

class DataSocket;
typedef std::shared_ptr<DataSocket> DataSocketPtr;

class DataSocket : public NonCopyable,
	public std::enable_shared_from_this<DataSocket>
{
public:
	DataSocket(std::shared_ptr<TcpConnection> connection);
	~DataSocket();
	friend class TcpConnection;

	typedef std::function<void(DataSocketPtr)> ConnectCallback;
	typedef std::function<void(DataSocketPtr, PacketPtr)> MessageCallback;
	typedef std::function<void(DataSocketPtr)> DisconnectCallback;

	static DataSocketPtr Create(std::shared_ptr<TcpConnection> connection);
	//void SetConnection(std::shared_ptr<TcpConnection> connection)
	//{ m_connection = connection; }

	void SetConnectCallback(ConnectCallback&& connectCallback)
	{ m_connectCallback = connectCallback; }
	void SetMessageCallback(MessageCallback&& messageCallback)
	{ m_messageCallback = messageCallback; }
	void SetDisconnectCallback(DisconnectCallback&& disconnectCallback)
	{ m_disconnectCallback = disconnectCallback; }

	void Send(PacketPtr packet);

	void OnConnect();
	void OnMessage(PacketPtr packet);
	void OnDisconnect();
private:
	std::weak_ptr<TcpConnection> m_connection;

	ConnectCallback m_connectCallback;
	MessageCallback m_messageCallback;
	DisconnectCallback m_disconnectCallback;
};

#endif // TOT_DATASOCKET_H