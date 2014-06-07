#ifndef IAK_NET_WATCHER_H
#define IAK_NET_WATCHER_H

#include "Event.h"
#include <base/NonCopyable.h>

#include <functional>

namespace iak {

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
	int getFd() { return fd_; }
	// watch events
	int events() { return events_; }

	// callback
	void setReadCallback(EventCallback&& cb) { readCallback_ = cb; }
	void setWriteCallback(EventCallback&& cb) { writeCallback_ = cb; }
	void setCloseCallback(EventCallback&& cb) { closeCallback_ = cb; }
	void enableRead() { events_ |= EV_READ; }
	void disableRead() { events_ &= ~EV_READ; }
	void enableWrite() { events_ |= EV_WRITE; }
	void disableWrite() { events_ &= ~EV_WRITE; }
	void enableClose() { events_ |= EV_CLOSE; }
	void disableClose() { events_ &= ~EV_CLOSE; }

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

	void activeRead();
	void activeWrite();
	void active(int revents);
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
}; // end class Watcher

} // end namespace iak

#endif // IAK_NET_WATCHER_H
