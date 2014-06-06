#include "EPollLoop.h"
#include "Watcher.h"
#include "Event.h"

#include <sys/epoll.h>
#include <unistd.h>

EPollLoop::EPollLoop()
	: EventLoop()
	, epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	, eventsize_(kEventSizeInit) {
	events_ = (struct epoll_event*)::malloc(eventsize_ * sizeof struct epoll_event);
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
			Watcher* watcher = (Watcher*)event.data.ptr;
			int events = (event.events & (EPOLLRDHUP)?EV_CLOSE:EV_NONE) // close
				| (event.events & (EPOLLIN|EPOLLERR|EPOLLHUP)?EV_READ:EV_NONE) // read
				| (event.events & (EPOLLOUT|EPOLLERR|EPOLLHUP)?EV_WRITE:EV_NONE); // write
			watcher->active(events);
		}
		if (nfd == eventsize_) {
			::free(events_);
			eventsize_ = eventsize_ << 1; // eventsize extend
			events_ = (struct epoll_event*)::malloc(eventsize_ * sizeof struct epoll_event);
		}
	}
	else if (nfd == 0) {

	}
	else if (errno != EINTR) {
		// error happens
	}
}

void EPollLoop::addWatcher(Watcher* watch) {
	struct epoll_event event;
	int events = watch->watchEvents();
	event.events = EPOLLET;	// edge trigger
		| (events & EV_CLOSE ? EPOLLRDHUP : 0)// close
		| (events & EV_READ ? EPOLLIN : 0)	// read
		| (events & EV_WRITE ? EPOLLOUT : 0);	// write
	event.data.ptr = watch;
	if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, watch->fd(), &event) < 0) {
		// on error
		return;
	}
}

void EPollLoop::updateWatcher(Watcher* watcher) {

}

void EPollLoop::removeWatcher(Watcher* watch) {
	if (::epoll_ctl(epollfd_, EPOLL_CTL_DEL, watch->fd(),NULL) < 0) {
		// on error
		return;
	}
}
