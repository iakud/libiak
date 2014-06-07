#include "Watcher.h"
#include "EventLoop.h"

using namespace iak;

void Watcher::start() {
	loop_->addWatcher(this);
}

void Watcher::stop() {
	loop_->removeWatcher(this);
}

//void Watcher::ActiveEvents(int events)
//{
//	if (events)
//	{
//		m_activeevents |= events;
//		m_loop->activeWatcher(this);
//	}
//}
void Watcher::activeRead() {
	revents_ |= EV_READ;
	loop_->activeWatcher(this);
}

void Watcher::activeWrite() {
	revents_ |= EV_WRITE;
	loop_->activeWatcher(this);
}

void Watcher::active(int revents) {
	revents &= events_;
	if (revents & EV_READ && !readable_) {
		readable_ = true;
		revents_ |= EV_READ;
		loop_->activeWatcher(this);
	}
	if (revents & EV_WRITE && ! writeable_) {
		writeable_ = true;
		revents_ |= EV_WRITE;
		loop_->activeWatcher(this);
	}
	if (revents & EV_CLOSE && closed_) {
		closed_ = true;
		revents_ |= EV_CLOSE;
		loop_->activeWatcher(this);
	}
}

void Watcher::handleEvents() {
	if (revents_ & EV_CLOSE) {
		revents_ &= ~EV_CLOSE;
		if (closeCallback_) {
			closeCallback_();
		}
	}
	if (revents_ & EV_READ) {
		revents_ &= ~EV_READ;
		if (readCallback_) {
			readCallback_();
		}
	}
	if (revents_ & EV_WRITE) {
		revents_ &= ~EV_WRITE;
		if (writeCallback_) {
			writeCallback_();
		}
	}
}
