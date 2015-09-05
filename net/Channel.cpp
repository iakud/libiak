#include "Channel.h"
#include "EventLoop.h"

using namespace iak::net;

Channel::Channel(EventLoop* loop, const int fd)
	: loop_(loop)
	, fd_(fd)
	, read_(false)
	, write_(false)
	, rclose_(false)
	, rerror_(false)
	, rread_(false)
	, rwrite_(false)
	, started_(false)
	, actived_(false)
	, closed_(false)
	, readable_(false)
	, writeable_(false) {
}

Channel::~Channel() {
}

void Channel::open() {
	if (!started_) {
		loop_->addChannel(this);
		started_ = true;
	}
}

void Channel::update() {
	if (started_) {
		loop_->updateChannel(this);
	}
}

void Channel::close() {
	if (started_) {
		loop_->removeChannel(this);
		started_ = false;
	}
}

void Channel::onClose() {
	if (!closed_ && !rclose_) {
		closed_ = rclose_ = true;
		addActivedChannel();
	}
}

void Channel::onError() {
	if (!rerror_) {
		rerror_ = true;
		addActivedChannel();
	}
}

void Channel::onRead() {
	if (read_ && !readable_ && !rread_) {
		readable_ = rread_ = true;
		addActivedChannel();
	}
}

void Channel::onWrite() {
	if (write_ && !writeable_ && !rwrite_) {
		writeable_ = rwrite_ = true;
		addActivedChannel();
	}
}

void Channel::handleEvents() {
	actived_ = false;
	if (rclose_) {
		rclose_ = false;
		if (closeCallback_) {
			closeCallback_();
		}
	}
	if (rerror_) {
		rerror_ = false;
		if (errorCallback_) {
			errorCallback_();
		}
	}
	if (rread_) {
		rread_ = false;
		if (readCallback_) {
			readCallback_();
		}
	}
	if (rwrite_) {
		rwrite_ = false;
		if (writeCallback_) {
			writeCallback_();
		}
	}
}

void Channel::addActivedChannel() {
	if (!actived_) {
		loop_->addActivedChannel(this);
		actived_ = true;
	}
}