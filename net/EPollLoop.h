#ifndef TOT_EPOLLLOOP_H
#define TOT_EPOLLLOOP_H

#include "EventLoop.h"

struct epoll_event;

class EPollLoop : public EventLoop
{
public:
	EPollLoop();
	virtual ~EPollLoop();

protected:
	void poll(int timeout) override;

	void addWatcher(Watcher* watcher) override;
	void removeWatcher(Watcher* watcher) override;

	int m_epollfd;
	int m_eventsize;
	struct epoll_event* m_events;
};

#endif // TOT_EPOLLLOOP_H
