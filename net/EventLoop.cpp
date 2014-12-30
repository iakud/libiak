#include "EventLoop.h"
#include "EPollLoop.h"
#include "Watcher.h"
#include "Buffer.h"

using namespace iak;

EventLoop::EventLoop()
	: quit_(false)
	, mutex_()
	, pendingFunctors_()
	, activedWatchers_()
	, bufferPool_(new BufferPool(1024*4, 64)) {
}

EventLoop::~EventLoop() {
}

EventLoop* EventLoop::create() {
	return new EPollLoop();
}

void EventLoop::release(EventLoop* loop) {
	delete loop;
}

// blocking until quit
void EventLoop::loop() {
	quit_ = false;
	while(!quit_) {
		poll(10);	// poll network event
		std::vector<Functor> functors;
		{
			MutexGuard lock(mutex_);
			functors.swap(pendingFunctors_);
		}
		for (Functor functor : functors) {
			functor();
		}
		handleWatchers();
		// clear at last (for watchers safe)
		// functors.clear();
	}
}

void EventLoop::activeWatcher(Watcher* watcher) {
	if (watcher->isActived()) {
		return;
	}
	activedWatchers_.push_back(watcher);
	watcher->setActived(true);
}

void EventLoop::handleWatchers() {
	std::vector<Watcher*> watchers;
	watchers.swap(activedWatchers_);
	for (Watcher* watch : watchers) {
		watch->setActived(false);
		watch->handleEvents();
	}
}
