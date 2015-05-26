#include "Watcher.h"
#include "EventLoop.h"

using namespace iak::net;

Watcher::Watcher(EventLoop* loop, const int fd)
	: loop_(loop)
	, fd_(fd)
	, read_(false)
	, write_(false)
	, close_(false)
	, rread_(false)
	, rwrite_(false)
	, rclose_(false)
	, started_(false)
	, actived_(false)
	, readable_(false)
	, writeable_(false)
	, closed_(false) {
}

Watcher::~Watcher() {
}

void Watcher::activeRead() {
	if (read_) {
		rread_ = true;
		if (!actived_) {
			loop_->activeWatcher(this);
			actived_ = true;
		}
	}
}

void Watcher::activeWrite() {
	if (write_) {
		rwrite_ = true;
		if (!actived_) {
			loop_->activeWatcher(this);
			actived_ = true;
		}
	}
}

void Watcher::start() {
	loop_->addWatcher(this);
}

void Watcher::stop() {
	loop_->removeWatcher(this);
}

void Watcher::onRead() {
	if (read_ && !readable_) {
		readable_ = rread_ = true;
		if (!actived_) {
			loop_->activeWatcher(this);
			actived_ = true;
		}
	}
}

void Watcher::onWrite() {
	if (write_ && !writeable_) {
		writeable_ = rwrite_ = true;
		if (!actived_) {
			loop_->activeWatcher(this);
			actived_ = true;
		}
	}
}

void Watcher::onClose() {
	if (close_ && !closed_) {
		closed_ = rclose_ = true;
		if (!actived_) {
			loop_->activeWatcher(this);
			actived_ = true;
		}
	}
}

void Watcher::handleEvents() {
	actived_ = false;
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
	if (rclose_) {
		rclose_ = false;
		if (closeCallback_) {
			closeCallback_();
		}
	}
}