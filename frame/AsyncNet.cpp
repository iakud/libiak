#include "AsyncNet.h"
#include "Thread.h"
#include "EventLoopThreadPool.h"

std::vector<Functor> AsyncNet::s_pendingFunctors_;
Mutex AsyncNet::s_mutex_;
EventLoopThreadPool* AsyncNet::s_loopThreadPool_ = NULL;
uint32_t AsyncNet::s_indexLoop_ = 0;

AsyncNet::init(uint32_t count) {
	s_loopThreadPool_ = new EventLoopThreadPool(count);
}

AsyncNet::destroy() {
	delete s_loopThreadPool_;
}

TcpServerPtr AsyncNet::Listen(const InetAddress& localAddr, EstablishCallback callback)
{
	TcpServerPtr server = TcpServer::Create(m_loopThreadPool->GetNextLoop(m_indexLoop), localAddr);
	server->SetConnectCallback(std::bind(&AsyncNet::onEstablish, this, std::placeholders::_1, callback));
	server->ListenAsync();
	server->SetEventLoopThreadPool(m_loopThreadPool);
	m_servers.push_back(server);
	return server;
}

TcpClientPtr AsyncNet::Connect(const InetAddress& remoteAddr, EstablishCallback callback)
{
	TcpClientPtr client = TcpClient::Create(m_loopThreadPool->GetNextLoop(m_indexLoop), remoteAddr);
	client->SetConnectCallback(std::bind(&AsyncNet::onEstablish, this, std::placeholders::_1, callback));
	client->ConnectAsync();
	client->SetRetry(true);
	m_clients.push_back(client);
	return client;
}

void AsyncNet::dispatch() {
	std::vector<Functor> functors;
	{
		MutexGuard lock(s_mutex_);
		functors.swap(s_pendingFunctors_);
	}
	for (Functor functor : functors) {
		functor();
	}
}

#include <iostream>
using namespace std;
void AsyncNet::onEstablish(TcpConnectionPtr connection, EstablishCallback callback)
{
	connection->SetConnectCallback(std::bind(&AsyncNet::onConnect, this, std::placeholders::_1));
	connection->SetReceiveCallback(std::bind(&AsyncNet::onMessage, this, std::placeholders::_1, std::placeholders::_2));
	connection->SetDisconnectCallback(std::bind(&AsyncNet::onDisconnect, this, std::placeholders::_1));
	DataSocketPtr datasock = DataSocket::Create(connection);
	connection->SetDataSocket(datasock);
	cout<<"AsyncNetonEstablish"<<endl;
	runInProcess(std::bind(&AsyncNet::onProcessEstablish, this, connection, callback));
}

void AsyncNet::onConnect(TcpConnectionPtr connection)
{
	runInProcess(std::bind(&AsyncNet::onProcessConnect, this, connection->GetDataSocket()));
}

void AsyncNet::onMessage(TcpConnectionPtr connection, PacketPtr packet)
{
	DataSocketPtr datasock = connection->GetDataSocket();
	runInProcess(std::bind(&AsyncNet::onProcessMessage, this, datasock, packet));
}

void AsyncNet::onDisconnect(TcpConnectionPtr connection)
{
	DataSocketPtr datasock = connection->GetDataSocket();
	connection->SetDataSocket(NULL);
	runInProcess(std::bind(&AsyncNet::onProcessDisconnect, this, datasock));
}

//////////////////////////////////////////////////////////////////////////

void AsyncNet::onProcessEstablish(TcpConnectionPtr connection, EstablishCallback callback)
{
	if (callback(connection->GetDataSocket()))
	{
		connection->EstablishAsync();
	}
	else
	{
		connection->CloseAsync();
	}
}

void AsyncNet::onProcessConnect(DataSocketPtr datasock)
{
	datasock->OnConnect();
}

void AsyncNet::onProcessMessage(DataSocketPtr datasock, PacketPtr packet)
{
	datasock->OnMessage(packet);
}

void AsyncNet::onProcessDisconnect(DataSocketPtr datasock)
{
	datasock->OnDisconnect();
}

