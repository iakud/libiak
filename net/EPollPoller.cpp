#include "EPollPoller.h"
#include "Channel.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace iak::net;

EPollPoller::EPollPoller()
	: epollFd_(::epoll_create1(EPOLL_CLOEXEC))
	, eventSize_(kEventSizeInit) {
	events_ = static_cast<struct epoll_event*>(
			::malloc(eventSize_ * sizeof(struct epoll_event)));
}

EPollPoller::~EPollPoller() {
	::free(events_);
	::close(epollFd_);
}

void EPollPoller::poll(int timeout) {
	int nfd = ::epoll_wait(epollFd_, events_, eventSize_, timeout);
	if (nfd > 0) {
		for (int i=0; i<nfd; ++i) {
			struct epoll_event& event = events_[i];
			Channel* channel = static_cast<Channel*>(event.data.ptr);
			if (event.events & (EPOLLRDHUP|EPOLLHUP)) {
				channel->onClose();
			}
			if (event.events & (EPOLLERR)) {
				channel->onError();
			}
			if (event.events & (EPOLLIN)) {
				channel->onRead();
			}
			if (event.events & (EPOLLOUT)) {
				channel->onWrite();
			}
		}
		if (nfd == eventSize_) {
			::free(events_);
			eventSize_ = eventSize_ << 1; // eventsize extend
			events_ = static_cast<struct epoll_event*>(
					::malloc(eventSize_ * sizeof(struct epoll_event)));
		}
	} else if (nfd == 0) {

	} else if (errno != EINTR) {
		// error happens
	}
}

void EPollPoller::addChannel(Channel* channel) {
	struct epoll_event event;
	if (channel->isRead()) {
		event.events |= EPOLLIN;
	}
	if (channel->isWrite()) {
		event.events |= EPOLLOUT;
	}
	event.events |= (EPOLLERR|EPOLLRDHUP|EPOLLET);	// edge trigger
	event.data.ptr = channel;
	if (::epoll_ctl(epollFd_, EPOLL_CTL_ADD, channel->getFd(), &event) < 0) {
		// on error
		return;
	}
}

void EPollPoller::updateChannel(Channel* channel) {
	struct epoll_event event;
	if (channel->isRead()) {
		event.events |= EPOLLIN;
	}
	if (channel->isWrite()) {
		event.events |= EPOLLOUT;
	}
	event.events |= (EPOLLERR|EPOLLRDHUP|EPOLLET);	// edge trigger
	event.data.ptr = channel;
	if (::epoll_ctl(epollFd_, EPOLL_CTL_MOD, channel->getFd(), &event) < 0) {
		// on error
		return;
	}
}

void EPollPoller::removeChannel(Channel* channel) {
	if (::epoll_ctl(epollFd_, EPOLL_CTL_DEL, channel->getFd(), nullptr) < 0) {
		// on error
		return;
	}
}
