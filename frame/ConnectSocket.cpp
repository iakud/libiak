#include "ConnectSocket.h"
#include "AsyncNet.h"

using namespace iak;

ConnectSocketPtr ConnectSocket::make(const InetAddress& remoteAddr) {
	return std::make_shared<ConnectSocket>(remoteAddr);
}

ConnectSocket::ConnectSocket(const InetAddress& remoteAddr)
	: client_(TcpClient::make(AsyncNet::getEventLoop(), remoteAddr)) {
	client_->setRetry(true);
	client_->setConnectCallback(std::bind(&ConnectSocket::onConnect,
			this, std::placeholders::_1));
}

ConnectSocket::~ConnectSocket() {
	
}

void ConnectSocket::connect() {
	client_->connectAsync();
}

void ConnectSocket::onConnect(TcpConnectionPtr connection) {
	AsyncNet::put(std::bind(&ConnectSocket::handleConnect, this, connection));
}

void ConnectSocket::handleConnect(TcpConnectionPtr connection) {
	DataSocketPtr datasocket = DataSocket::make(connection,
			std::bind(&ConnectSocket::onClose, this, std::placeholders::_1));
	if (connectCallback_(datasocket)) {
		connection->establishAsync();
		datasocket_ = datasocket;
	} else {
		connection->closeAsync();
	}
}

void ConnectSocket::onClose(DataSocketPtr datasocket) {
	datasocket_.reset();
}
