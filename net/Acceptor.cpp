#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace iak::net;

AcceptorPtr Acceptor::make(EventLoop* loop,
		const struct sockaddr_in& localSockAddr) {
	return std::make_shared<Acceptor>(loop, localSockAddr);
}

Acceptor::Acceptor(EventLoop* loop, const struct sockaddr_in& localSockAddr)
	: loop_(loop)
	, localSockAddr_(localSockAddr)
	, sockFd_(Socket::open())
	, channel_(new Channel(loop, sockFd_))
	, listenning_(false)
	, accept_(false) 
	, idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
	Socket::setReuseAddr(sockFd_, 1);
	Socket::bind(sockFd_, localSockAddr_);
	
	channel_->setReadCallback(std::bind(&Acceptor::onRead, this));
	channel_->enableRead();
}

Acceptor::~Acceptor() {
	Socket::close(sockFd_);
	::close(idleFd_);
}


void Acceptor::listen() {
	if (accept_) {
		return;
	}
	accept_ = true;
	loop_->runInLoop(std::bind(&Acceptor::listenInLoop, shared_from_this()));
}

void Acceptor::close() {
	if (!accept_) {
		return;
	}
	accept_ = false;
	loop_->runInLoop(std::bind(&Acceptor::closeInLoop, shared_from_this()));
}

// listen in loop
void Acceptor::listenInLoop() {
	if (!accept_ || listenning_) {
		return;
	}
	listenning_ = true;
	Socket::listen(sockFd_);
	channel_->open();
}

// close in loop
void Acceptor::closeInLoop() {
	if (accept_) {
		return;
	}
	channel_->close();
}

void Acceptor::onRead() {
	if (!accept_) {
		return;
	}
	while (true) {
		struct sockaddr_in peerSockAddr;
		int sockFd = Socket::accept(sockFd_, &peerSockAddr);
		if (sockFd < 0) {
			int err = errno;// on error
			if (EAGAIN == err) {
				channel_->disableReadable();
				return;
			} else if (EMFILE == err || ENFILE == err) {
				::close(idleFd_);
				idleFd_ = Socket::accept(sockFd_, nullptr);
				::close(idleFd_);
				idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
			} else {
				// FIXME
				return;
			}
		} else {
			// accept successful
			if (acceptCallback_) {
				acceptCallback_(sockFd, peerSockAddr);
			} else {
				Socket::close(sockFd);
			}
		}
	}
}