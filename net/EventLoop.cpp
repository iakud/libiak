#include "EventLoop.h"
#include "EPollPoller.h"
#include "Channel.h"
#include "Buffer.h"

#include <sys/eventfd.h>
#include <unistd.h>

using namespace iak::net;

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
	, activedChannels_()
	, bufferPool_(new BufferPool(1024*4, 64))
	, epollPoller_(new EPollPoller())
	, wakeupFd_(createEventFd())
	, wakeupChannel_(new Channel(this, wakeupFd_)) {
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
	wakeupChannel_->enableRead();
	wakeupChannel_->open();
}

EventLoop::~EventLoop() {
	wakeupChannel_->close();
	::close(wakeupFd_);
}

void EventLoop::quit() {
	quit_ = true;
	wakeup();
}

// blocking until quit
void EventLoop::loop() {
	quit_ = false;
	while(!quit_) {
		epollPoller_->poll(kPollTime); // poll network event
		handleActivedChannels();
		doPendingFunctors();
	}
}

void EventLoop::runInLoop(Functor&& functor) {
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.push_back(functor);
	}
	wakeup();
}

void EventLoop::addChannel(Channel* channel) {
	epollPoller_->addChannel(channel);
}

void EventLoop::updateChannel(Channel* channel) {
	epollPoller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
	epollPoller_->removeChannel(channel);
}

void EventLoop::addActivedChannel(Channel* channel) {
	activedChannels_.push_back(channel);
}

void EventLoop::doPendingFunctors() {
	std::vector<Functor> functors;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for (Functor functor : functors) {
		functor();
	}
}

void EventLoop::handleActivedChannels() {
	for (Channel* channel : activedChannels_) {
		channel->handleEvents();
	}
	activedChannels_.clear();
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	::write(wakeupFd_, &one, sizeof one);
}

void EventLoop::handleWakeup() {
	uint64_t one = 1;
	::read(wakeupFd_, &one, sizeof one);
	wakeupChannel_->disableReadable();
}