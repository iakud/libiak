#include "ListenSocket.h"

#include <net/TcpServer.h>
#include <net/TcpConnection.h>

ListenSocket::ListenSocket(const InetAddress& localAddr)
	: server_(TcpServer::create(AsyncNet::getEventLoop(), localAddr)) {
	server_->setEventLoopThreadPool(AsyncNet::getEventLoopThreadPool());
	server_->setConnectCallback(std::bind(&ListenSocket::handleAccept,
			this, std::placeholders::_1));
}

ListenSocket::~ListenSocket() {

}

ListenSocketPtr ListenSocket::create(const InetAddress& localAddr) {
	return std::make_shared<ListenSocket>(localAddr);
}

void ListenSocket::listen() {
	server_->listenAsync();
}

void ListenSocket::onAccept(std::shared_ptr<TcpConnection> connection) {
	AsyncNet::put(std::bind(&ListenSocket::handleAccept, this, connection));
}

void ListenSocket::handleAccept(std::shared_ptr<TcpConnection> connection) {
	std::shared_ptr<DataSocket> datasocket = DataSocket::create(connection);
	if (connectCallback_(datasocket)) {
		connection->establishAsync();
	}
}