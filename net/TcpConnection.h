#ifndef TOT_TCPCONNECTION_H
#define TOT_TCPCONNECTION_H

#include "InetAddress.h"
#include "Packet.h"

#include <base/NonCopyable.h>

#include <memory>
#include <functional>

#include <stdint.h>

namespace iak {

class EventLoop;
class Watcher;
class Buffer;
class BufferPool;

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : public NonCopyable, 
		public std::enable_shared_from_this<TcpConnection> {

public:
	// callback typedef
	typedef std::function<void(TcpConnectionPtr)> ConnectCallback;
	typedef std::function<void(TcpConnectionPtr, PacketPtr)> ReceiveCallback;
	//typedef std::function<void(TcpConnectionPtr, uint32_t)> SendCallback;
	typedef std::function<void(TcpConnectionPtr)> DisconnectCallback;

	static TcpConnectionPtr make(EventLoop* loop, int sockFd,
			const InetAddress& localAddr, const InetAddress& peerAddr);

public:
	explicit TcpConnection(EventLoop* loop, int sockFd, 
		const InetAddress& localAddr, const InetAddress& remoteAddr);
	~TcpConnection();	
	
	const InetAddress& getLocalAddress() const { return localAddr_; }
	const InetAddress& getRemoteAddress() const { return remoteAddr_; }
	// IO
	size_t GetReadBufferSize() { return readSize_; }
	bool PeekData(const char* data, const size_t size);
	bool ReadData(const char* data, const size_t size);
	size_t GetWriteBufferSize() { return writeSize_; }
	bool writeData(const char* data, const size_t size);
	// callback function
	void setConnectCallback(ConnectCallback&& cb) { connectCallback_ = cb; }
	void setReceiveCallback(ReceiveCallback&& cb) { receiveCallback_ = cb; }
	//void SetSendCallback(SendCallback&& cb) { sendCallback_ = cb; }
	void setDisconnectCallback(DisconnectCallback&& cb)  { disconnectCallback_ = cb; }
	// user data
	void setUserData(void* userdata) { userdata_ = userdata; }
	void* getUserData() const { return userdata_; }

	void establishAsync();
	void shutdownAsync();
	void closeAsync();
	void sendPack(PacketPtr packet);

private:
	// for tcpserver and tcpclient
	typedef std::function<void(TcpConnectionPtr)> CloseCallback;
	void setCloseCallback(CloseCallback&& cb) { closeCallback_ = cb; }

	// after close or shutdown
	void destroyAsync();
	// in loop
	void establish();
	void shutdown();
	void destroy();
	void send(PacketPtr packet);
	// handle events
	void onRead();
	void onWrite();
	void onClose();

	EventLoop* loop_;
	const int sockFd_;
	InetAddress localAddr_;
	InetAddress remoteAddr_;
	bool establish_;
	bool close_;
	std::unique_ptr<Watcher> watcher_;
	std::shared_ptr<BufferPool> bufferPool_;

	// read write buffer
	Buffer* readHead_;	// read head buffer
	Buffer* readTail_;	// read tail buffer
	size_t readSize_;
	Buffer* writeHead_;// write head buffer
	Buffer* writeTail_;// write tail buffer
	size_t writeSize_;

	// callback
	ConnectCallback connectCallback_;
	ReceiveCallback receiveCallback_;
	//SendCallback sendCallback_;
	DisconnectCallback disconnectCallback_;
	CloseCallback closeCallback_;

	// userdata
	void* userdata_;

	// friend class only TcpServer & TcpClient
	friend class TcpServer;
	friend class TcpClient;
}; // end class TcpConnection

} // end namespace iak

#endif // IAK_NET_TCPCONNECTION_H
