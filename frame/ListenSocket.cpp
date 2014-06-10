#include "ListenSocket.h"
#include "AsyncNet.h"

#include <net/TcpServer.h>
#include <net/TcpConnection.h>

using namespace iak;

ListenSocket::ListenSocket(const InetAddress& localAddr)
	: server_(TcpServer::create(AsyncNet::getEventLoop(), localAddr)) {
	server_->setEventLoopThreadPool(AsyncNet::getEventLoopThreadPool());
	server_->setConnectCallback(std::bind(&ListenSocket::onAccept,
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
	std::shared_ptr<DataSocket> datasocket = DataSocket::create(connection,
			std::bind(&ListenSocket::onClose, this, std::placeholders::_1));
	if (connectCallback_(datasocket)) {
		connection->establishAsync();
		datasockets_.insert(datasocket);
	} else {
		connection->closeAsync();
	}
}

void ListenSocket::onClose(std::shared_ptr<DataSocket> datasocket) {
	datasockets_.erase(datasocket);
}
