#ifndef IAK_NET_ACCEPTOR_H
#define IAK_NET_ACCEPTOR_H

#include <memory>
#include <functional>

#include <arpa/inet.h>

namespace iak {
namespace net {

class EventLoop;
class Channel;

class Acceptor;
typedef std::shared_ptr<Acceptor> AcceptorPtr;

class Acceptor : public std::enable_shared_from_this<Acceptor> {

public:
	typedef std::function<void(int sockFd,
			const struct sockaddr_in& remoteSockAddr)> AcceptCallback;
	static AcceptorPtr make(EventLoop* loop,
			const struct sockaddr_in& localSockAddr);

public:
	explicit Acceptor(EventLoop* loop,
			const struct sockaddr_in& localSockAddr);
	~Acceptor();
	// noncopyable
	Acceptor(const Acceptor&) = delete;
	Acceptor& operator=(const Acceptor&) = delete;

	void setAcceptCallback(AcceptCallback&& acceptCallback) {
		acceptCallback_ = acceptCallback;
	}

	bool isListenning() const { return listenning_; }

	void listen();
	void close();

private:
	void listenInLoop();		// listen in loop
	void closeInLoop();		// close in loop
	void onRead();	// read event active

	EventLoop* loop_;
	const struct sockaddr_in localSockAddr_;
	const int sockFd_;
	std::unique_ptr<Channel> channel_;
	bool listenning_;	// is listenning
	bool accept_;	// is accept new connection?
	int idleFd_;

	AcceptCallback acceptCallback_;
}; // end class Acceptor

} // end namespace net
} // end namespace iak

#endif // IAK_NET_ACCEPTOR_H
