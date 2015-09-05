#ifndef IAK_NET_CHANNEL_H
#define IAK_NET_CHANNEL_H

#include <functional>

namespace iak {
namespace net {

class EventLoop;

class Channel {
public:
	typedef std::function<void()> EventCallback;

public:
	Channel(EventLoop* loop, const int fd);
	~Channel();
	// noncopyable
	Channel(const Channel&) = delete;
	Channel& operator=(const Channel&) = delete;

	// fd
	int getFd() { return fd_; }
	// events
	bool isRead() { return read_; }
	void enableRead() { read_ = true; }
	void disableRead() { read_ = false; }
	bool isWrite() { return write_; }
	void enableWrite() { write_ = true; }
	void disableWrite() { write_ = false; }
	void enableAll() { read_ = write_ = true; }
	void disableAll() { read_ = write_ = false; }
	// callback
	void setCloseCallback(EventCallback&& cb) { closeCallback_ = cb; }
	void setErrorCallback(EventCallback&& cb) { errorCallback_ = cb; }
	void setReadCallback(EventCallback&& cb) { readCallback_ = cb; }
	void setWriteCallback(EventCallback&& cb) { writeCallback_ = cb; }
	// readable & writeable
	bool isReadable() { return readable_; }
	void disableReadable() { readable_ = false; }
	bool isWriteable() { return writeable_; }
	void disableWriteable() { writeable_ = false; }

	void open();
	void update();
	void close();

	void onClose();
	void onError();
	void onRead();
	void onWrite();
	void handleEvents();

private:
	void addActivedChannel();

	EventLoop* loop_;
	const int fd_;
	// events
	bool read_;
	bool write_;
	// revents
	bool rclose_;
	bool rerror_;
	bool rread_;
	bool rwrite_;
	// events callback
	EventCallback closeCallback_;
	EventCallback errorCallback_;
	EventCallback readCallback_;
	EventCallback writeCallback_;

	bool started_;	// started
	bool actived_;	// in active list
	bool closed_;
	bool readable_;
	bool writeable_;
}; // end class Channel

} // end namespace net
} // end namespace iak

#endif // IAK_NET_CHANNEL_H
