#include "ConnectSocket.h"
#include "AsyncNet.h"

#include <net/TcpClient.h>
#include <net/TcpConnection.h>

using namespace iak;

ConnectSocket::ConnectSocket(const InetAddress& remoteAddr)
	: client_(TcpClient::create(AsyncNet::getEventLoop(), remoteAddr)) {
	client_->setRetry(true);
	client_->setConnectCallback(std::bind(&ConnectSocket::onConnect,
			this, std::placeholders::_1));
}

ConnectSocket::~ConnectSocket() {
	
}

ConnectSocketPtr ConnectSocket::create(const InetAddress& remoteAddr) {
	return std::make_shared<ConnectSocket>(remoteAddr);
}

void ConnectSocket::connect() {
	client_->connectAsync();
}

void ConnectSocket::onConnect(std::shared_ptr<TcpConnection> connection) {
	AsyncNet::put(std::bind(&ConnectSocket::handleConnect, this, connection));
}

void ConnectSocket::handleConnect(std::shared_ptr<TcpConnection> connection) {
	std::shared_ptr<DataSocket> datasocket = DataSocket::create(connection,
			std::bind(&ConnectSocket::onClose, this, std::placeholders::_1));
	if (connectCallback_(datasocket)) {
		connection->establishAsync();
		datasocket_ = datasocket;
	} else {
		connection->closeAsync();
	}
}

void ConnectSocket::onClose(std::shared_ptr<DataSocket> datasocket) {
	datasocket_.reset();
}
