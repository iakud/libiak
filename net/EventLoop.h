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

class EventLoop : public NonCopyable {
public:
	static EventLoop* create();
	static void release(EventLoop*);
	
	typedef std::function<void()> Functor;

	std::shared_ptr<BufferPool>& getBufferPool() {
		return bufferPool_;
	}

	void runInLoop(Functor&& functor) {
		MutexGuard lock(mutex_);
		pendingFunctors_.push_back(functor);
	}

	void quit() { quit_ = true; }
	void loop();

protected:
	EventLoop();
	virtual ~EventLoop();

	// actived watchers
	void activeWatcher(Watcher* watcher);
	void handleWatchers();
	virtual void poll(int timeout)=0;
	virtual void addWatcher(Watcher* watcher) = 0;
	virtual void updateWatcher(Watcher* watcher) = 0;
	virtual void removeWatcher(Watcher* watcher) = 0;
	
	bool quit_;
	Mutex mutex_;
	std::vector<Functor> pendingFunctors_;
	std::vector<Watcher*> activedWatchers_;
	std::shared_ptr<BufferPool> bufferPool_; // buffer

	// friend class
	friend class Watcher;
}; // end class EventLoop

} // end namespace iak

#endif // IAK_NET_EVENTLOOP_H
