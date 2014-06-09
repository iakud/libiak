#include "DataSocket.h"
#include "TcpConnection.h"

DataSocket::DataSocket(std::shared_ptr<TcpConnection> connection)
	: m_connection(connection)
{

}

DataSocket::~DataSocket()
{

}

DataSocketPtr DataSocket::Create(std::shared_ptr<TcpConnection> connection)
{
	return std::make_shared<DataSocket>(connection);
}

void DataSocket::Send(PacketPtr packet)
{
	TcpConnectionPtr connection = m_connection.lock();
	if (connection)
	{
		connection->Send(packet);
	}
}

void DataSocket::OnConnect()
{
	if (m_connectCallback)
	{
		m_connectCallback(shared_from_this());
	}
}

void DataSocket::OnMessage(PacketPtr packet)
{
	if (m_messageCallback)
	{
		m_messageCallback(shared_from_this(), packet);
	}
}

void DataSocket::OnDisconnect()
{
	if (m_disconnectCallback)
	{
		m_disconnectCallback(shared_from_this());
	}
}