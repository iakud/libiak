#ifndef IAK_NET_EPOLLLOOP_H
#define IAK_NET_EPOLLLOOP_H

#include "EventLoop.h"

struct epoll_event;

class EPollLoop : public EventLoop {
public:
	EPollLoop();
	virtual ~EPollLoop();

protected:
	static const int kEventSizeInit = 16;

	void poll(int timeout) override;
	void addWatcher(Watcher* watcher) override;
	void updateWatcher(Watcher* watcher) override;
	void removeWatcher(Watcher* watcher) override;

	int epollfd_;
	int eventsize_;
	struct epoll_event* events_;
};

#endif // IAK_NET_EPOLLLOOP_H
