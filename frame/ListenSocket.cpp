#include "ListenSocket.h"
#include "AsyncNet.h"

#include <net/TcpServer.h>
#include <net/TcpConnection.h>

using namespace iak;

ListenSocketPtr ListenSocket::make(const InetAddress& localAddr) {
	return std::make_shared<ListenSocket>(localAddr);
}

ListenSocket::ListenSocket(const InetAddress& localAddr)
	: server_(TcpServer::make(AsyncNet::getEventLoop(), localAddr)) {
	server_->setEventLoopThreadPool(AsyncNet::getEventLoopThreadPool());
	server_->setConnectCallback(std::bind(&ListenSocket::onAccept,
			this, std::placeholders::_1));
}

ListenSocket::~ListenSocket() {
	
}

void ListenSocket::listen() {
	server_->listenAsync();
}

void ListenSocket::onAccept(TcpConnectionPtr connection) {
	AsyncNet::put(std::bind(&ListenSocket::handleAccept, this, connection));
}

void ListenSocket::handleAccept(TcpConnectionPtr connection) {
	DataSocketPtr datasocket = DataSocket::make(connection,
			std::bind(&ListenSocket::onClose, this, std::placeholders::_1));
	if (connectCallback_(datasocket)) {
		connection->establishAsync();
		datasockets_.insert(datasocket);
	} else {
		connection->closeAsync();
	}
}

void ListenSocket::onClose(DataSocketPtr datasocket) {
	datasockets_.erase(datasocket);
}
