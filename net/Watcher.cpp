#include "Watcher.h"
#include "EventLoop.h"

Watcher::Watcher(EventLoop* loop, const int fd)
	: m_loop(loop)
	, m_fd(fd)
	, m_watchevents(EV_NONE)
	, m_activeevents(EV_NONE)
	, m_watched(false)
	, m_actived(false)
{
}

Watcher::~Watcher()
{
}

void Watcher::Start()
{
	m_loop->addWatcher(this);
}

void Watcher::Stop()
{
	m_loop->removeWatcher(this);
}

void Watcher::ActiveEvents(int events)
{
	if (events)
	{
		m_activeevents |= events;
		m_loop->activeWatcher(this);
	}
}

void Watcher::ActiveWatcher(int events)
{
	events &= m_watchevents;
	if (events)
	{
		if (events & EV_READ)
			m_readable = true;
		if (events & EV_WRITE)
			m_writeable = true;
		m_activeevents |= events;
		m_loop->activeWatcher(this);
	}
}

void Watcher::HandleEvents()
{
	if (m_activeevents & EV_CLOSE && m_closeCallback)
	{
		m_activeevents &= ~EV_CLOSE;
		m_closeCallback();
	}
	if (m_activeevents & EV_READ && m_readCallback)
	{
		m_activeevents &= ~EV_READ;
		m_readCallback();
	}
	if (m_activeevents & EV_WRITE && m_writeCallback)
	{
		m_activeevents &= ~EV_WRITE;
		m_writeCallback();
	}
}
