#ifndef IAK_NET_EVENTLOOP_H
#define IAK_NET_EVENTLOOP_H

#include "Buffer.h"

#include <mutex>
#include <vector>
#include <memory>
#include <functional>

namespace iak {
namespace net {

class Channel;
class EPollPoller;

class EventLoop {
public:
	typedef std::function<void()> Functor;

public:
	EventLoop();
	virtual ~EventLoop();
	// noncopyable
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator=(const EventLoop&) = delete;

	std::shared_ptr<BufferPool>& getBufferPool() {
		return bufferPool_;
	}

	void quit();
	void loop();

	void runInLoop(Functor&& functor);

	void addChannel(Channel* channel);
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
	void addActivedChannel(Channel* channel);

protected:
	void doPendingFunctors();

	// actived channels
	void handleActivedChannels();

	void wakeup();
	void handleWakeup();
	
	bool quit_;
	std::mutex mutex_;
	std::vector<Functor> pendingFunctors_;
	std::vector<Channel*> activedChannels_;
	std::shared_ptr<BufferPool> bufferPool_; // buffer
	std::unique_ptr<EPollPoller> epollPoller_;
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;
}; // end class EventLoop

} // end namespace net
} // end namespace iak

#endif // IAK_NET_EVENTLOOP_H
