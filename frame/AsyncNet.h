#ifndef IAK_FRAME_ASYNCNET_H
#define IAK_FRAME_ASYNCNET_H

#include "DataSocket.h"
#include <list>

class EventLoopThreadPool;

class AsyncNet {
public:
	static void init(uint32_t count);
	static void destroy();

	typedef std::function<void()> Functor;

	void put(Functor&& functor) {
		MutexGuard lock(mutex_);
		s_pendingFunctors_.push_back(functor);
	}

	// run in main thread, per loop
	static void dispatch();

	static EventLoop* getEventLoop() {
		assert(s_loopThreadPool_);
		if (s_loopThreadPool_) {
			++s_indexLoop_;
			s_indexLoop_ %= s_loopThreadPool_->getCount();
			return s_loopThreadPool_->getLoop(s_indexLoop_);
		}
		return NULL;
	}

	static EventLoopThreadPool* getEventLoopThreadPool() {
		return s_loopThreadPool_;
	}

private:
	void onEstablish(TcpConnectionPtr connection, EstablishCallback callback);
	void onConnect(TcpConnectionPtr connection);
	void onMessage(TcpConnectionPtr connection, PacketPtr packet);
	void onDisconnect(TcpConnectionPtr connection);

	void onProcessEstablish(TcpConnectionPtr connection, EstablishCallback callback);
	void onProcessMessage(DataSocketPtr datasock, PacketPtr packet);
	void onProcessConnect(DataSocketPtr datasock);
	void onProcessDisconnect(DataSocketPtr datasock);

	std::list<TcpServerPtr> m_servers;
	std::list<TcpClientPtr> m_clients;

	static std::vector<Functor> s_pendingFunctors_;
	static Mutex s_mutex_;
	static EventLoopThreadPool* s_loopThreadPool_;
	static uint32_t s_indexLoop_;
};

#endif // IAK_FRAME_ASYNCNET_H