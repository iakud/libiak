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
	bool isRead() { return read_; }
	void enableRead() { read_ = true; }
	void disableRead() { read_ = false; }
	bool isWrite() { return write_; }
	void enableWrite() { write_ = true; }
	void disableWrite() { write_ = false; }
	bool isClose() { return closed_; }
	void enableClose() { closed_ = true; }
	void disableClose() { closed_ = false; }
	// callback
	void setReadCallback(EventCallback&& cb) { readCallback_ = cb; }
	void setWriteCallback(EventCallback&& cb) { writeCallback_ = cb; }
	void setCloseCallback(EventCallback&& cb) { closeCallback_ = cb; }
	// readable & writeable
	bool isReadable() { return readable_; }
	void disableReadable() { readable_ = false; }
	bool isWriteable() { return writeable_; }
	void disableWriteable() { writeable_ = false; }
	// active readable & writeable
	void activeRead();
	void activeWrite();

	void start();
	void stop();

	void onRead();
	void onWrite();
	void onClose();
	void handleEvents();

private:
	EventLoop* loop_;
	const int fd_;
	// watch events
	bool read_;
	bool write_;
	bool close_;
	// watch revents
	bool rread_;
	bool rwrite_;
	bool rclose_;
	// events callback
	EventCallback closeCallback_;
	EventCallback readCallback_;
	EventCallback writeCallback_;

	bool started_;	// started
	bool actived_;	// in active list
	bool readable_;
	bool writeable_;
	bool closed_;
}; // end class Watcher

} // end namespace iak

#endif // IAK_NET_WATCHER_H
