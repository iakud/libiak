#ifndef TOT_EVENTLOOP_H
#define TOT_EVENTLOOP_H

#include <vector>
#include <functional>
#include "NonCopyable.h"
#include "Thread.h"
#include "BufferPool.h"

class Watcher;

class EventLoop : public NonCopyable
{
protected:
	EventLoop();
	~EventLoop();
public:
	// friend class
	friend class Watcher;
	friend class TcpAcceptor;
	friend class TcpConnector;
	friend class TcpConnection;
	friend class ReadBuffer;
	friend class WriteBuffer;

	typedef std::function<void()> Functor;

	static EventLoop* Create();
	static void Release(EventLoop*);
	
	void Loop();
	void Quit();

protected:
	void runInLoop(Functor&& functor)
	{ MutexGuard lock(m_mutex); m_pendingFunctors.push_back(functor); }
	void swapPendingFunctors(std::vector<Functor>& functors)
	{ MutexGuard lock(m_mutex); functors.swap(m_pendingFunctors); }
	void doPendingFunctors(std::vector<Functor>& functors);
	// actived watchers
	void activeWatcher(Watcher* watcher);
	void handleActivedWatchers();
	virtual void addWatcher(Watcher* watcher)=0;
	virtual void removeWatcher(Watcher* watcher)=0;
	virtual void poll(int timeout)=0;
	
	bool m_bQuit;
	std::vector<Functor> m_pendingFunctors;
	std::vector<Watcher*> m_activedWatchers;
	Mutex m_mutex;
	// buffer
	BufferPoolPtr m_bufferPool;
};

#endif // TOT_EVENTLOOP_H
