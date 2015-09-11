#ifndef TOT_TCPCONNECTION_H
#define TOT_TCPCONNECTION_H

#include "InetAddress.h"

#include <memory>
#include <functional>
#include <string>

#include <stdint.h>

namespace iak {
namespace net {

class EventLoop;
class Channel;
class Buffer;
class BufferPool;

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {

public:
	// callback typedef
	typedef std::function<void(TcpConnectionPtr)> ConnectCallback;
	typedef std::function<void(TcpConnectionPtr, uint32_t)> RecvCallback;
	typedef std::function<void(TcpConnectionPtr, uint32_t)> SendCallback;
	typedef std::function<void(TcpConnectionPtr)> DisconnectCallback;

	static TcpConnectionPtr make(EventLoop* loop, int sockFd,
			const InetAddress& localAddr, const InetAddress& peerAddr);
private:
	// for tcpserver and tcpclient
	typedef std::function<void(TcpConnectionPtr)> CloseCallback;

public:
	explicit TcpConnection(EventLoop* loop, int sockFd, 
		const InetAddress& localAddr, const InetAddress& remoteAddr);
	~TcpConnection();
	// noncopyable
	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;
	
	// callback function
	void setConnectCallback(ConnectCallback&& cb) { connectCallback_ = cb; }
	void setRecvCallback(RecvCallback&& cb) { recvCallback_ = cb; }
	void setSendCallback(SendCallback&& cb) { sendCallback_ = cb; }
	void setDisconnectCallback(DisconnectCallback&& cb)  { disconnectCallback_ = cb; }

	void setTcpNoDelay(bool val);
	void setKeepAlive(bool val);

	const InetAddress& getLocalAddress() const { return localAddr_; }
	const InetAddress& getRemoteAddress() const { return remoteAddr_; }
	// IO
	size_t GetReadBufferSize() { return readSize_; }
	bool PeekData(const char* data, size_t size);
	bool ReadData(const char* data, size_t size);
	size_t GetWriteBufferSize() { return writeSize_; }
	bool writeData(const char* data, size_t size);
	
	// user data
	void setUserData(void* userdata) { userdata_ = userdata; }
	void* getUserData() const { return userdata_; }

	void establish();
	void shutdown();
	void close();
	void sendPack(const char* data, size_t size);//PacketPtr packet);

private:
	void setCloseCallback(CloseCallback&& cb) { closeCallback_ = cb; }

	// after close or shutdown
	void destroy();
	// in loop
	void establishInLoop();
	void shutdownInLoop();
	void destroyInLoop();
	void send(std::string &data);
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
	std::unique_ptr<Channel> channel_;
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
	RecvCallback recvCallback_;
	SendCallback sendCallback_;
	DisconnectCallback disconnectCallback_;
	CloseCallback closeCallback_;

	// userdata
	void* userdata_;

	// friend class only TcpServer & TcpClient
	friend class TcpServer;
	friend class TcpClient;
}; // end class TcpConnection

} // end namespace net
} // end namespace iak

#endif // IAK_NET_TCPCONNECTION_H
