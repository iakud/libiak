#include "EventLoop.h"
#include "EPollLoop.h"
#include "Watcher.h"
#include "Buffer.h"

EventLoop::EventLoop()
	: m_bufferPool(BufferPool::Create(1024*4, 64))
{

}

EventLoop::~EventLoop()
{

}

EventLoop* EventLoop::Create()
{
	return new EPollLoop();
}

void EventLoop::Release(EventLoop* loop)
{
	delete loop;
}

// block until quit loop
void EventLoop::Loop()
{
	m_bQuit = false;
	std::vector<Functor> functors;
	while(!m_bQuit)
	{
		poll(10);	// poll network event
		swapPendingFunctors(functors);
		doPendingFunctors(functors);
		handleActivedWatchers();
		// clear at last (for watchers safe)
		functors.clear();
	}
}

// Quit loop
void EventLoop::Quit()
{
	m_bQuit = true;
}

// do all pending function
void EventLoop::doPendingFunctors(std::vector<Functor>& functors)
{
	for (Functor functor : functors)
	{
		functor();
	}
}

void EventLoop::activeWatcher(Watcher* watcher)
{
	if (watcher->IsActived()) return;
	m_activedWatchers.push_back(watcher);
	watcher->SetActived(true);
}

void EventLoop::handleActivedWatchers()
{
	std::vector<Watcher*> activedWatchers;
	activedWatchers.swap(m_activedWatchers);
	for (Watcher* watcher : activedWatchers)
	{
		watcher->SetActived(false);
		watcher->HandleEvents();
	}
}
