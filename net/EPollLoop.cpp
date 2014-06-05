#include "EPollLoop.h"
#include <sys/epoll.h>
#include <unistd.h>
#include "Watcher.h"

#define INIT_EVENTSIZE 16

EPollLoop::EPollLoop()
	: EventLoop()
	, m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
	, m_eventsize(INIT_EVENTSIZE)
{
	m_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*m_eventsize);
}

EPollLoop::~EPollLoop()
{
	::free(m_events);
	::close(m_epollfd);
}

void EPollLoop::poll(int timeout)
{
	int nfd = ::epoll_wait(m_epollfd,m_events,m_eventsize,timeout);

	if (nfd > 0)
	{
		for (int i=0;i<nfd;++i)
		{
			struct epoll_event& event = m_events[i];
			Watcher* watcher = (Watcher*)event.data.ptr;
			int events = (event.events & (EPOLLRDHUP)?EV_CLOSE:EV_NONE) // close
				| (event.events & (EPOLLIN|EPOLLERR|EPOLLHUP)?EV_READ:EV_NONE) // read
				| (event.events & (EPOLLOUT|EPOLLERR|EPOLLHUP)?EV_WRITE:EV_NONE); // write
			watcher->ActiveWatcher(events);
		}
		if (nfd == m_eventsize)
		{
			::free(m_events);
			m_eventsize = m_eventsize<<1; // eventsize extend
			m_events = (struct epoll_event*)::malloc(sizeof(struct epoll_event)*m_eventsize);
		}
	}
	else if (nfd == 0)
	{

	}
	else if (errno != EINTR)
	{
		// error happens
	}
}

void EPollLoop::addWatcher(Watcher* watcher)
{
	if (watcher->IsWatched()) return;
	struct epoll_event event;
	int events = watcher->GetWatchEvents();
	event.events = EPOLLET | (events&EV_CLOSE?EPOLLRDHUP:0) // close
		| (events&EV_READ?EPOLLIN:0) | (events&EV_WRITE?EPOLLOUT:0); //read & write
	event.data.ptr = watcher;
	if (::epoll_ctl(m_epollfd,EPOLL_CTL_ADD,watcher->GetFd(),&event) < 0)
	{
		// on error
		return;
	}
	watcher->SetWatched(true);
}

void EPollLoop::removeWatcher(Watcher* watcher)
{
	if (!watcher->IsWatched()) return;
	if (::epoll_ctl(m_epollfd,EPOLL_CTL_DEL,watcher->GetFd(),NULL) < 0)
	{
		// on error
		return;
	}
	watcher->SetWatched(false);
}
