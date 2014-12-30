#include "EPollLoop.h"
#include "Watcher.h"
#include "Event.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace iak;

EPollLoop::EPollLoop()
	: EventLoop()
	, epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	, eventsize_(kEventSizeInit) {
	events_ = static_cast<struct epoll_event*>(
			::malloc(eventsize_ * sizeof(struct epoll_event)));
}

EPollLoop::~EPollLoop() {
	::free(events_);
	::close(epollfd_);
}

void EPollLoop::poll(int timeout) {
	int nfd = ::epoll_wait(epollfd_, events_, eventsize_, timeout);
	if (nfd > 0) {
		for (int i=0; i<nfd; ++i) {
			struct epoll_event& event = events_[i];
			Watcher* watcher = static_cast<Watcher*>(event.data.ptr);
			int events = (event.events & (EPOLLRDHUP)?EV_CLOSE:EV_NONE) // close
				| (event.events & (EPOLLIN|EPOLLERR|EPOLLHUP)?EV_READ:EV_NONE) // read
				| (event.events & (EPOLLOUT|EPOLLERR|EPOLLHUP)?EV_WRITE:EV_NONE); // write
			watcher->active(events);
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

void EPollLoop::addWatcher(Watcher* watcher) {
	struct epoll_event event;
	int events = watcher->events();
	event.events = EPOLLET;	// edge trigger
	if (events & EV_CLOSE) {
		event.events |= EPOLLRDHUP;
	}
	if (events & EV_READ) {
		event.events |= EPOLLIN;
	}
	if (events & EV_WRITE) {
		event.events |= EPOLLOUT;
	}
	event.data.ptr = watcher;
	if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, watcher->getFd(), &event) < 0) {
		// on error
		return;
	}
}

void EPollLoop::updateWatcher(Watcher* watcher) {

}

void EPollLoop::removeWatcher(Watcher* watcher) {
	if (::epoll_ctl(epollfd_, EPOLL_CTL_DEL, watcher->getFd(),NULL) < 0) {
		// on error
		return;
	}
}
