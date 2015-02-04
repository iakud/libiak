#include "EPollPoller.h"
#include "Watcher.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace iak;

EPollPoller::EPollPoller()
	: epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	, eventsize_(kEventSizeInit) {
	events_ = static_cast<struct epoll_event*>(
			::malloc(eventsize_ * sizeof(struct epoll_event)));
}

EPollPoller::~EPollPoller() {
	::free(events_);
	::close(epollfd_);
}

void EPollPoller::poll(int timeout) {
	int nfd = ::epoll_wait(epollfd_, events_, eventsize_, timeout);
	if (nfd > 0) {
		for (int i=0; i<nfd; ++i) {
			struct epoll_event& event = events_[i];
			Watcher* watcher = static_cast<Watcher*>(event.data.ptr);
			
			if (event.events & (EPOLLIN|EPOLLERR|EPOLLHUP)) {
				watcher->onRead();
			}
			if (event.events & (EPOLLOUT|EPOLLERR|EPOLLHUP)) {
				watcher->onWrite();
			}
			if (event.events & (EPOLLRDHUP)) {
				watcher->onClose();
			}
		}
		if (nfd == eventsize_) {
			::free(events_);
			eventsize_ = eventsize_ << 1; // eventsize extend
			events_ = static_cast<struct epoll_event*>(
					::malloc(eventsize_ * sizeof(struct epoll_event)));
		}
	}
	else if (nfd == 0) {

	}
	else if (errno != EINTR) {
		// error happens
	}
}

void EPollPoller::addWatcher(Watcher* watcher) {
	struct epoll_event event;
	event.events = EPOLLET;	// edge trigger
	if (watcher->isRead()) {
		event.events |= EPOLLIN;
	}
	if (watcher->isWrite()) {
		event.events |= EPOLLOUT;
	}
	if (watcher->isClose()) {
		event.events |= EPOLLRDHUP;
	}
	event.data.ptr = watcher;
	if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, watcher->getFd(), &event) < 0) {
		// on error
		return;
	}
}

void EPollPoller::updateWatcher(Watcher* watcher) {

}

void EPollPoller::removeWatcher(Watcher* watcher) {
	if (::epoll_ctl(epollfd_, EPOLL_CTL_DEL, watcher->getFd(),NULL) < 0) {
		// on error
		return;
	}
}
