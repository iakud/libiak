#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>

using namespace iak;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& localAddr)
	: loop_(loop)
	, localAddr_(localAddr)
	, acceptor_(Acceptor::create(loop, localAddr_.getSockAddr()))
	, listen_(false)
	, loopThreadPool_(NULL)
	, indexLoop_(0) {
	acceptor_->setAcceptCallback(std::bind(&TcpServer::handleAccept, 
			this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
	if (listen_) {
		acceptor_->closeAsync();	// close first
		// destroy connections
		for (TcpConnectionPtr connection : connections_) {
			connection->destroyAsync();
			//connection.reset();
		}
	}
	if (loopThreadPool_) {
		delete loopThreadPool_;
		loopThreadPool_ = NULL;
	}
}

TcpServerPtr TcpServer::create(EventLoop* loop, 
		const InetAddress& localAddr) {
	return std::make_shared<TcpServer>(loop, localAddr);
}

void TcpServer::listenAsync() {
	if (listen_) {
		return;
	}

	listen_ = true;
	acceptor_->listenAsync();
}

void TcpServer::handleAccept(const int sockFd,
		const struct sockaddr_in& remoteSockAddr) {
	InetAddress remoteAddr(remoteSockAddr);
	EventLoop* loop;
	if (loopThreadPool_) {
		++indexLoop_;
		indexLoop_ %= loopThreadPool_->getCount();
		loopThreadPool_->getLoop(indexLoop_);
	} else {
		loop = loop_;
	}
	
	TcpConnectionPtr connection = TcpConnection::create(loop, 
			sockFd, localAddr_, remoteAddr);
	connections_.insert(connection);
	connection->setCloseCallback(std::bind(&TcpServer::handleClose,
			this, std::placeholders::_1));
	if (connectCallback_) {
		connectCallback_(connection);
	}
}

void TcpServer::handleClose(TcpConnectionPtr connection) {
	//std::set<TcpConnectionPtr>::iterator itConnection = m_connections.find(connection);
	//m_connections.erase(itConnection);
	// add lock !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (connections_.erase(connection)) {
		connection->destroyAsync();
	}
}
