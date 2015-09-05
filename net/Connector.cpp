#include "Connector.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

#include <errno.h>

using namespace iak::net;

ConnectorPtr Connector::make(EventLoop* loop, const struct sockaddr_in& peerSockAddr) {
	return std::make_shared<Connector>(loop, peerSockAddr);
}

Connector::Connector(EventLoop* loop, const struct sockaddr_in& peerSockAddr)
	: loop_(loop)
	, peerSockAddr_(peerSockAddr)
	, connect_(false)
	, connecting_(false) {
}

Connector::~Connector() {
}

void Connector::connect() {
	connect_ = true;
	loop_->runInLoop(std::bind(&Connector::connectInLoop, shared_from_this()));
}

void Connector::close() {
	connect_ = false;
	loop_->runInLoop(std::bind(&Connector::closeInLoop, shared_from_this()));
}

void Connector::connectInLoop() {
	if (!connect_ || connecting_) {
		return;
	}

	int sockFd = Socket::open();
	int ret = Socket::connect(sockFd, peerSockAddr_);
	if (ret == 0) {
		struct sockaddr_in localSockAddr;
		Socket::getSockName(sockFd, &localSockAddr);
		if (connectCallback_) {
			connectCallback_(sockFd, localSockAddr);
		}
		return;
	}
	
	int err = errno;
	if (err == EINPROGRESS || err == EINTR || err == EISCONN) {
		connecting_ = true;
		channel_.reset(new Channel(loop_, sockFd));
		channel_->setWriteCallback(std::bind(&Connector::onWrite, this));
		channel_->enableWrite();
		channel_->open();
	} else {
		Socket::close(sockFd); // close first
		// retry
		if (err == EAGAIN || err == EADDRINUSE || err == EADDRNOTAVAIL || err == ECONNREFUSED || err == ENETUNREACH) {
			loop_->runInLoop(std::bind(&Connector::connectInLoop, shared_from_this()));
		}
	}
}

void Connector::closeInLoop() {
	if (!connecting_) {
		return;
	}

	int sockFd = channel_->getFd();
	channel_->close();
	Socket::close(sockFd);
}

void Connector::resetChannel() {
	channel_.reset();
}

void Connector::onWrite() {
	if (!connect_ || !connecting_) {
		return;
	}
	int sockFd = channel_->getFd();
	channel_->close();
	// inside Channel::handleEvent
	loop_->runInLoop(std::bind(&Connector::resetChannel, shared_from_this()));

	connecting_ = false;

	int optval;
	if (Socket::getError(sockFd, &optval) == 0 && optval == 0) {
		struct sockaddr_in localSockAddr;
		Socket::getSockName(sockFd, &localSockAddr);
		if (connectCallback_) {
			connectCallback_(sockFd, localSockAddr);
		} else {
			Socket::close(sockFd);
		}
	} else {
		Socket::close(sockFd); // close first
		// retry
		loop_->runInLoop(std::bind(&Connector::connectInLoop, shared_from_this()));
	}
}
