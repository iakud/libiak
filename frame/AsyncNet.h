#ifndef TOT_ASYNCNET_H
#define TOT_ASYNCNET_H

#include <list>
#include "NonCopyable.h"
#include "tot.h"
#include "DataSocket.h"

class AsyncNet : public NonCopyable
{
public:
	explicit AsyncNet(uint32_t count);
	virtual ~AsyncNet();

	typedef std::function<bool(DataSocketPtr)> EstablishCallback;

	TcpServerPtr Listen(const InetAddress& localAddr, EstablishCallback callback);
	TcpClientPtr Connect(const InetAddress& remoteAddr, EstablishCallback callback);
	// run in main thread, per loop
	void NetProcess();
private:
	typedef std::function<void()> Functor;

	void runInProcess(Functor&& functor)
	{ MutexGuard lock(m_mutex); m_pendingFunctors.push_back(functor); }
	void swapPendingFunctors(std::vector<Functor>& functors)
	{ MutexGuard lock(m_mutex); functors.swap(m_pendingFunctors); }

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

	std::vector<Functor> m_pendingFunctors;
	Mutex m_mutex;
	EventLoopThreadPool* m_loopThreadPool;
	uint32_t m_indexLoop;
};

#endif // TOT_ASYNCNET_H