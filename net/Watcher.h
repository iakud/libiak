#ifndef TOT_WATCHER_H
#define TOT_WATCHER_H

#include <base/NonCopyable.h>
#include <functional>

#define EV_NONE 0x00
#define EV_READ 0x01
#define EV_WRITE 0x02
#define EV_CLOSE 0x04

class EventLoop;

class Watcher : public NonCopyable {
public:
	Watcher(EventLoop* loop, const int fd);
	virtual ~Watcher();

	typedef std::function<void()> EventCallback;
	// watch fd
	int GetFd() { return m_fd; }
	// callback
	void SetReadCallback(EventCallback&& cb)
	{ m_readCallback = cb; }
	void SetWriteCallback(EventCallback&& cb)
	{ m_writeCallback = cb; }
	void SetCloseCallback(EventCallback&& cb)
	{ m_closeCallback = cb; }
	// watch events
	void SetWatchEvents(int events) { m_watchevents = events; }
	int GetWatchEvents() { return m_watchevents; }
	// watched & actived
	bool IsWatched() { return m_watched; }
	void SetWatched(bool watched) { m_watched = watched; }
	bool IsActived() { return m_actived; }
	void SetActived(bool actived) { m_actived = actived; }
	// readable & writeable
	bool IsReadable() { return m_readable; }
	void SetReadable(bool readable) { m_readable = readable; }
	bool IsWriteable() { return m_writeable; }
	void SetWriteable(bool writeable) { m_writeable = writeable; }

	void Start();
	void Stop();
	void ActiveEvents(int events);
	void ActiveWatcher(int events);
	void HandleEvents();
private:
	EventLoop* m_loop;
	const int m_fd;

	int m_watchevents;
	int m_activeevents;
	bool m_watched;
	bool m_actived;	// in active watchers list?

	bool m_readable;
	bool m_writeable;
	
	EventCallback m_readCallback;
	EventCallback m_writeCallback;
	EventCallback m_closeCallback;
};

#endif // TOT_WATCHER_H
