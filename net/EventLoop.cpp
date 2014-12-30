#include "EventLoop.h"
#include "EPollLoop.h"
#include "Watcher.h"
#include "Buffer.h"

using namespace iak;

namespace {

const int kPollTime = 10000; // ms

int createEventFd() {
	int eventFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (eventFd < 0) {
		abort();
	}
	return eventFd;
}

} // anonymous

EventLoop::EventLoop()
	: quit_(false)
	, mutex_()
	, pendingFunctors_()
	, activedWatchers_()
	, bufferPool_(new BufferPool(1024*4, 64))
	, wakeupFd_(createEventFd())
	, wakeupChannel_(new Watcher(this, wakeupFd_)) {
	wakeupWatcher_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
	wakeupWatcher_->enableRead();
}

EventLoop::~EventLoop() {
	 ::close(wakeupFd_);
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
		poll(kPollTime); // poll network event
		std::vector<Functor> functors;
		swapPendingFunctors(functors);
		doPendingFunctors(functors);
		handleWatchers();
		// clear at last (for watchers safe)
		// functors.clear();
	}
}

void EventLoop::runInLoop(Functor&& functor) {
	{
		MutexGuard lock(mutex_);
		pendingFunctors_.push_back(functor);
	}
	wakeup();
}

void EventLoop::swapPendingFunctors(std::vector<Functor>& functors) {
	MutexGuard lock(mutex_);
	functors.swap(pendingFunctors_);
}

void EventLoop::doPendingFunctors(std::vector<Functor>& functors) {
	for (Functor functor : functors) {
		functor();
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

void EventLoop::wakeup() {
	uint64_t one = 1;
	::write(wakeupFd_, &one, sizeof one);
}

void EventLoop::handleWakeup() {
	uint64_t one = 1;
	::read(wakeupFd_, &one, sizeof one);
}