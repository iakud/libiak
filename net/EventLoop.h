#ifndef IAK_NET_EVENTLOOP_H
#define IAK_NET_EVENTLOOP_H

#include "Buffer.h"
#include <base/NonCopyable.h>
#include <base/Mutex.h>

#include <vector>
#include <memory>
#include <functional>

namespace iak {

class Watcher;
class EPollPoller;

class EventLoop : public NonCopyable {
public:
	typedef std::function<void()> Functor;

public:
	EventLoop();
	virtual ~EventLoop();

	std::shared_ptr<BufferPool>& getBufferPool() {
		return bufferPool_;
	}

	void quit();
	void loop();

	void runInLoop(Functor&& functor);

protected:

	void addWatcher(Watcher* watcher);
	void updateWatcher(Watcher* watcher);
	void removeWatcher(Watcher* watcher);

	void swapPendingFunctors(std::vector<Functor>&);
	void doPendingFunctors(std::vector<Functor>&);

	// actived watchers
	void activeWatcher(Watcher* watcher);
	void handleWatchers();
	void activeWatchersWakeup();

	void wakeup();
	void handleWakeup();
	
	bool quit_;
	Mutex mutex_;
	std::vector<Functor> pendingFunctors_;
	std::vector<Watcher*> activedWatchers_;
	std::shared_ptr<BufferPool> bufferPool_; // buffer
	std::unique_ptr<EPollPoller> epollPoller_;
	int wakeupfd_;
	std::unique_ptr<Watcher> wakeupWatcher_;

	// friend class
	friend class Watcher;
}; // end class EventLoop

} // end namespace iak

#endif // IAK_NET_EVENTLOOP_H
