#ifndef IAK_NET_WATCHER_H
#define IAK_NET_WATCHER_H

#include "Event.h"
#include <base/NonCopyable.h>

#include <functional>

class EventLoop;

class Watcher : public NonCopyable {
public:
	Watcher(EventLoop* loop, const int fd)
		: loop_(loop)
		, fd_(fd)
		, events_(EV_NONE)
		, revents_(EV_NONE)
		, started_(false)
		, actived_(false)
		, closed_(false) {
	}

	~Watcher() {
	}

	typedef std::function<void()> EventCallback;

	// watch fd
	int fd() { return m_fd; }
	// watch events
	int events() { return events_; }

	// callback
	void setReadCallback(EventCallback&& cb) { readCallback_ = cb; }
	void setWriteCallback(EventCallback&& cb) { writeCallback_ = cb; }
	void setCloseCallback(EventCallback&& cb) { closeCallback_ = cb; }
	void enableRead() { wevents_ |= EV_READ; }
	void disableRead() { wevents_ &= ~EV_READ; }
	void enableWrite() { wevents_ |= EV_WRITE; }
	void disableWrite() { wevents_ &= ~EV_WRITE; }
	void enableClose() { wevents_ |= EV_CLOSE; }
	void disableClose() { wevents_ &= ~EV_CLOSE; }

	// watched & actived
	bool isActived() { return actived_; }
	void setActived(bool actived) { actived_ = actived; }
	// readable & writeable
	bool isReadable() { return readable_; }
	void setReadable(bool readable) { readable_ = readable; }
	bool isWriteable() { return writeable_; }
	void setWriteable(bool writeable) { writeable_ = writeable; }
	bool isClosed() { return closed_; }
	void setClosed(bool closed) { closed_ = closed; }

	void start();
	void stop();

	void active(int events);
	void handleEvents();

private:
	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;
	bool started_;
	bool actived_;	// in active list
	bool readable_;
	bool writeable_;
	bool closed_;
	EventCallback closeCallback_;
	EventCallback readCallback_;
	EventCallback writeCallback_;
};

#endif // IAK_NET_WATCHER_H
