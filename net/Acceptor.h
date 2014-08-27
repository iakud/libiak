#ifndef IAK_NET_ACCEPTOR_H
#define IAK_NET_ACCEPTOR_H

#include <base/NonCopyable.h>

#include <memory>
#include <functional>

#include <arpa/inet.h>

namespace iak {

class EventLoop;
class Watcher;

class Acceptor;
typedef std::shared_ptr<Acceptor> AcceptorPtr;

class Acceptor : public NonCopyable,
	public std::enable_shared_from_this<Acceptor> {

public:
	typedef std::function<void(int sockFd,
			const struct sockaddr_in& remoteSockAddr)> AcceptCallback;
	static AcceptorPtr make(EventLoop* loop,
			const struct sockaddr_in& localSockAddr);

public:
	explicit Acceptor(EventLoop* loop,
			const struct sockaddr_in& localSockAddr);
	~Acceptor();

	void setAcceptCallback(AcceptCallback&& acceptCallback) {
		acceptCallback_ = acceptCallback;
	}

	bool isListenning() const { return listenning_; }

	void listenAsync();
	void closeAsync();

private:
	void listen();		// listen in loop
	void close();		// close in loop
	void onRead();	// read event active

	EventLoop* loop_;
	const struct sockaddr_in localSockAddr_;
	const int sockFd_;
	std::unique_ptr<Watcher> watcher_;
	bool listenning_;	// is listenning
	bool accept_;	// is accept new connection?
	int idleFd_;

	AcceptCallback acceptCallback_;
}; // end class Acceptor

} // end namespace iak

#endif // IAK_NET_ACCEPTOR_H
