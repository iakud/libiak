#include "Acceptor.h"
#include "EventLoop.h"
#include "Watcher.h"

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace iak;

Acceptor::Acceptor(EventLoop* loop, 
		const struct sockaddr_in& localSockAddr)
	: loop_(loop)
	, localSockAddr_(localSockAddr)
	, sockFd_(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP))
	, watcher_(new Watcher(loop, sockFd_))
	, listenning_(false)
	, accept_(false) {
	::bind(sockFd_, reinterpret_cast<const struct sockaddr*>(&localSockAddr_),
		static_cast<socklen_t>(sizeof localSockAddr_));
	watcher_->setReadCallback(std::bind(&Acceptor::onRead, this));
	watcher_->enableRead();
}

Acceptor::~Acceptor() {
	::close(sockFd_);
}

AcceptorPtr Acceptor::create(EventLoop* loop,
		const struct sockaddr_in& localSockAddr) {
	return std::make_shared<Acceptor>(loop, localSockAddr);
}

void Acceptor::listenAsync() {
	if (accept_) {
		return;
	}
	accept_ = true;
	loop_->runInLoop(std::bind(&Acceptor::listen, shared_from_this()));
}

void Acceptor::closeAsync() {
	if (!accept_) {
		return;
	}
	accept_ = false;
	loop_->runInLoop(std::bind(&Acceptor::close, shared_from_this()));
}

// listen in loop
void Acceptor::listen() {
	if (!accept_ || listenning_) {
		return;
	}
	listenning_ = true;
	::listen(sockFd_, SOMAXCONN);
	watcher_->start();
}

// close in loop
void Acceptor::close() {
	if (accept_) {
		return;
	}
	watcher_->stop();
}

void Acceptor::onRead() {
	if (!accept_) {
		return;
	}

	struct sockaddr_in remoteSockAddr;
	socklen_t remoteSockAddrLen = static_cast<socklen_t>(sizeof remoteSockAddr);
	int sockFd = ::accept4(sockFd_,
		reinterpret_cast<struct sockaddr*>(&remoteSockAddr),
		&remoteSockAddrLen, SOCK_NONBLOCK|SOCK_CLOEXEC);
	if (sockFd < 0) {
		int err = errno;// on error
		if (err == EMFILE || err == ENFILE) {

		}
		watcher_->setReadable(false);
		return;
	}
	// accept successful
	if (acceptCallback_) {
		acceptCallback_(sockFd, remoteSockAddr);
	} else {
		::close(sockFd);
	}
	// continue accept in next loop
	watcher_->activeRead();
}
