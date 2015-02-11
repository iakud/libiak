#include "Watcher.h"
#include "EventLoop.h"

using namespace iak;

Watcher::Watcher(EventLoop* loop, const int fd)
	: loop_(loop)
	, fd_(fd)
	, sync_(false)
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
		if (sync_) {
			handleRead();
		} else {
			rread_ = true;
			if (!actived_) {
				loop_->activeWatcher(this);
				actived_ = true;
			}
		}
	}
}

void Watcher::activeWrite() {
	if (write_) {
		if (sync_) {
			handleWrite();
		} else {
			rwrite_ = true;
			if (!actived_) {
				loop_->activeWatcher(this);
				actived_ = true;
			}
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
		readable_ = true;
		if (sync_) {
			handleRead();
		} else {
			rread_ = true;
			if (!actived_) {
				loop_->activeWatcher(this);
				actived_ = true;
			}
		}
	}
}

void Watcher::onWrite() {
	if (write_ && !writeable_) {
		writeable_ = true;
		if (sync_) {
			handleWrite();
		} else {
			rwrite_ = true;
			if (!actived_) {
				loop_->activeWatcher(this);
				actived_ = true;
			}
		}
	}
}

void Watcher::onClose() {
	if (close_ && !closed_) {
		closed_ = true;
		if (sync_) {
			handleClose();
		} else {
			rclose_ = true;
			if (!actived_) {
				loop_->activeWatcher(this);
				actived_ = true;
			}
		}
	}
}

void Watcher::handleEvents() {
	actived_ = false;
	if (rread_) {
		rread_ = false;
		handleRead();
	}
	if (rwrite_) {
		rwrite_ = false;
		handleWrite();
	}
	if (rclose_) {
		rclose_ = false;
		handleClose();
	}
}

void Watcher::handleRead() {
	if (readCallback_) {
		readCallback_();
	}
}

void Watcher::handleWrite() {
	if (writeCallback_) {
		writeCallback_();
	}
}

void Watcher::handleClose() {
	if (closeCallback_) {
		closeCallback_();
	}
}