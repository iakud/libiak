#include "TcpClient.h"
#include "EventLoop.h"
#include "Connector.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>

using namespace iak::net;

TcpClientPtr TcpClient::make(EventLoop* loop, const InetAddress& peerAddr) {
	return std::make_shared<TcpClient>(loop, peerAddr);
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& peerAddr)
	: loop_(loop)
	, peerAddr_(peerAddr)
	, connector_(Connector::make(loop, peerAddr_.getSockAddr()))
	, connect_(false)
	, retry_(false) {
	connector_->setConnectCallback(std::bind(&TcpClient::onConnect, this, std::placeholders::_1, std::placeholders::_2));
}

TcpClient::~TcpClient() {
	if (connect_) {
		if (connection_) {
			connection_->destroy();
		} else {
			connector_->close();
		}
	}
}

void TcpClient::connect() {
	if (connect_) {
		return;
	}
	connect_ = true;
	connector_->connect();
}

void TcpClient::onConnect(const int sockFd, const struct sockaddr_in& localSockAddr) {
	InetAddress localAddr(localSockAddr);
	TcpConnectionPtr connection = TcpConnection::make(loop_, sockFd, localAddr, peerAddr_);
	connection_ = connection;
	connection->setCloseCallback(std::bind(&TcpClient::onClose, 
			this, std::placeholders::_1));
	if (connectCallback_) {
		connectCallback_(connection);
	}
}

void TcpClient::onClose(TcpConnectionPtr connection) {
	if (connection_ != connection) {
		return;	// match connection
	}
	
	connection->destroy();// Destroy first
	connection_.reset();// release pointer
	if (!connect_) {
		return;// connect?
	}

	if (retry_) { // retry?
		connector_->connect();	// connect again
	} else {
		connect_ = false;	// disconnect
	}
}
