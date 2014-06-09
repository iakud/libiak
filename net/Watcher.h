#ifndef IAK_NET_WATCHER_H
#define IAK_NET_WATCHER_H

#include <base/NonCopyable.h>

#include <functional>

namespace iak {

class EventLoop;

class Watcher : public NonCopyable {
public:
	Watcher(EventLoop* loop, const int fd);
	~Watcher();

	typedef std::function<void()> EventCallback;

	// watch fd
	int getFd() { return fd_; }
	// watch events
	int events() { return events_; }

	// callback
	void setReadCallback(EventCallback&& cb) { readCallback_ = cb; }
	void setWriteCallback(EventCallback&& cb) { writeCallback_ = cb; }
	void setCloseCallback(EventCallback&& cb) { closeCallback_ = cb; }
	
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

	void enableRead();
	void disableRead();
	void enableWrite();
	void disableWrite();
	void enableClose();
	void disableClose();

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
