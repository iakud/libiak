#include "EventLoop.h"
#include "EPollPoller.h"
#include "Watcher.h"
#include "Buffer.h"

#include <sys/eventfd.h>
#include <unistd.h>

using namespace iak;

namespace {

const int kPollTime = 10000; // ms

int createEventFd() {
	int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (eventfd < 0) {
		abort();
	}
	return eventfd;
}

} // anonymous

EventLoop::EventLoop()
	: quit_(false)
	, mutex_()
	, pendingFunctors_()
	, activedWatchers_()
	, bufferPool_(new BufferPool(1024*4, 64))
	, epollPoller_(new EPollPoller())
	, wakeupfd_(createEventFd())
	, wakeupWatcher_(new Watcher(this, wakeupfd_)) {
	wakeupWatcher_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
	wakeupWatcher_->enableRead();
	wakeupWatcher_->start();
}

EventLoop::~EventLoop() {
	wakeupWatcher_->stop();
	 ::close(wakeupfd_);
}

void EventLoop::quit() {
	quit_ = true;
	wakeup();
}

// blocking until quit
void EventLoop::loop() {
	quit_ = false;
	while(!quit_) {
		activeWatchersWakeup(); // wakeup when has active watchers
		epollPoller_->poll(kPollTime); // poll network event
		std::vector<Functor> functors;
		swapPendingFunctors(functors);
		doPendingFunctors(functors);
		handleWatchers();
		// clear at last (for watchers safe)
		// functors.clear();
	}
}

void EventLoop::addWatcher(Watcher* watcher) {
	epollPoller_->addWatcher(watcher);
}

void EventLoop::updateWatcher(Watcher* watcher) {
	epollPoller_->updateWatcher(watcher);
}

void EventLoop::removeWatcher(Watcher* watcher) {
	epollPoller_->removeWatcher(watcher);
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
	activedWatchers_.push_back(watcher);
}

void EventLoop::handleWatchers() {
	std::vector<Watcher*> watchers;
	watchers.swap(activedWatchers_);
	for (Watcher* watcher : watchers) {
		watcher->handleEvents()
	}
}

void EventLoop::activeWatchersWakeup() {
	if (activedWatchers_.size() > 0) {
		wakeup();
	}
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	::write(wakeupfd_, &one, sizeof one);
}

void EventLoop::handleWakeup() {
	uint64_t one = 1;
	::read(wakeupfd_, &one, sizeof one);
	wakeupWatcher_->disableReadable();
}