#include "TcpClient.h"
#include "EventLoop.h"
#include "Connector.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>

using namespace iak;

TcpClient::TcpClient(EventLoop* loop, const InetAddress& remoteAddr)
	: loop_(loop)
	, remoteAddr_(remoteAddr)
	, connector_(Connector::create(loop, remoteAddr_.getSockAddr()))
	, connect_(false)
	, retry_(false) {
	connector_->setConnectCallback(std::bind(&TcpClient::handleConnect, 
			this, std::placeholders::_1, std::placeholders::_2));
}

TcpClient::~TcpClient() {
	if (connect_) {
		if (connection_) {
			connection_->destroyAsync();
		} else {
			connector_->closeAsync();	// SAFE
		}
	}
}

TcpClientPtr TcpClient::create(EventLoop* loop, 
		const InetAddress& remoteAddr) {
	return std::make_shared<TcpClient>(loop, remoteAddr);
}

void TcpClient::connectAsync() {
	if (connect_) {
		return;
	}
	connect_ = true;
	connector_->connectAsync();
}

void TcpClient::handleConnect(const int sockFd, 
		const struct sockaddr_in& localSockAddr) {
	InetAddress localAddr(localSockAddr);
	TcpConnectionPtr connection = TcpConnection::create(loop_, 
			sockFd, localAddr, remoteAddr_);
	connection_ = connection;
	connection->setCloseCallback(std::bind(&TcpClient::handleClose, 
			this, std::placeholders::_1));
	if (connectCallback_) {
		connectCallback_(connection);
	}
}

void TcpClient::handleClose(TcpConnectionPtr connection) {
	if (connection_ != connection) {
		return;	// match connection
	}
	
	connection->destroyAsync();// Destroy first
	connection_.reset();// release pointer
	if (!connect_) {
		return;// connect?
	}

	if (retry_) { // retry?
		connector_->connectAsync();	// connect again
	} else {
		connect_ = false;	// disconnect
	}
}
