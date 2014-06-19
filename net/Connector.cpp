#include "Connector.h"
#include "EventLoop.h"
#include "Watcher.h"

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace iak;

ConnectorPtr Connector::make(EventLoop* loop,
		const struct sockaddr_in& remoteSockAddr) {
	return std::make_shared<Connector>(loop, remoteSockAddr);
}

Connector::Connector(EventLoop* loop,
		const struct sockaddr_in& remoteSockAddr)
	: loop_(loop)
	, remoteSockAddr_(remoteSockAddr)
	, connect_(false)
	, connecting_(false) {
}

Connector::~Connector() {
}

void Connector::connectAsync() {
	connect_ = true;
	loop_->runInLoop(std::bind(&Connector::connect, shared_from_this()));
}

void Connector::closeAsync() {
	connect_ = false;
	loop_->runInLoop(std::bind(&Connector::close, shared_from_this()));
}

void Connector::connect() {
	if (!connect_ || connecting_) {
		return;
	}

	int sockFd = ::socket(AF_INET, 
			SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);
	int ret = ::connect(sockFd, 
			reinterpret_cast<const struct sockaddr*>(&remoteSockAddr_), 
		static_cast<socklen_t>(sizeof remoteSockAddr_));
	if (ret == 0) {
		//connecting(sockFd);
		//connected(sockFd); //not here?
		struct sockaddr_in localSockAddr;
		socklen_t localSockAddrLen = static_cast<socklen_t>(sizeof localSockAddr);
		::getsockname(sockFd, 
				reinterpret_cast<struct sockaddr*>(&localSockAddr), 
				&localSockAddrLen);
		if (connectCallback_) {
			connectCallback_(sockFd, localSockAddr);
		}
		return;
	}
	
	int err = errno;
	if (err == EINPROGRESS || err == EINTR || err == EISCONN) {
		connecting_ = true;
		watcher_.reset(new Watcher(loop_, sockFd));
		watcher_->enableWrite();
		watcher_->setWriteCallback(std::bind(&Connector::onWrite, this));
		watcher_->start();
	} else {
		::close(sockFd); // close first
		// retry
		if (err == EAGAIN || err == EADDRINUSE || err == EADDRNOTAVAIL 
				|| err == ECONNREFUSED || err == ENETUNREACH) {
			loop_->runInLoop(std::bind(&Connector::connect, shared_from_this()));
		}
	}
}

void Connector::close() {
	if (!connecting_) {
		return;
	}

	int sockFd = watcher_->getFd();
	watcher_->stop();
	::close(sockFd);
}

void Connector::onWrite() {
	if (!connect_ || !connecting_) {
		return;
	}

	int sockFd = watcher_->getFd();
	watcher_->stop();
	//watcher_->disableWrite();
	watcher_.reset();
	connecting_ = false;

	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);
	if (::getsockopt(sockFd, SOL_SOCKET, SO_ERROR, 
				&optval, &optlen) == 0 && optval == 0) {
		struct sockaddr_in localSockAddr;
		socklen_t localSockAddrLen = static_cast<socklen_t>(sizeof localSockAddr);
		::getsockname(sockFd, reinterpret_cast<struct sockaddr*>(&localSockAddr), 
				&localSockAddrLen);
		if (connectCallback_) {
			connectCallback_(sockFd, localSockAddr);
		}
	} else {
		::close(sockFd); // close first
		// retry
		loop_->runInLoop(std::bind(&Connector::connect, shared_from_this()));
	}
}
