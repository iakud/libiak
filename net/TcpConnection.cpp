#include "TcpConnection.h"
#include "EventLoop.h"
#include "Watcher.h"
#include "Buffer.h"

#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace iak;

TcpConnection::TcpConnection(EventLoop* loop, int sockFd, 
		const InetAddress& localAddr, const InetAddress& remoteAddr)
	: loop_(loop)
	, sockFd_(sockFd)
	, localAddr_(localAddr)
	, remoteAddr_(remoteAddr)
	, establish_(false)
	, close_(false)
	, watcher_(new Watcher(loop, sockFd))
	, bufferPool_(loop->getBufferPool()) {
	int optval = 1;
	::setsockopt(sockFd_, SOL_SOCKET, SO_KEEPALIVE, &optval, 
			static_cast<socklen_t>(sizeof optval));
	watcher_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
	watcher_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
	watcher_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	watcher_->enableRead();
	watcher_->enableWrite();
	watcher_->enableClose();
}

TcpConnection::~TcpConnection() {
	// release read buffer
	while (readHead_) {
		readHead_ = bufferPool_->putNext(readHead_);
	}
	// release write buffer
	while (writeHead_) {
		writeHead_ = bufferPool_->putNext(writeHead_);
	}
	::close(sockFd_);
}

TcpConnectionPtr TcpConnection::create(EventLoop* loop, int sockFd, 
	const InetAddress& localAddr, const InetAddress& peerAddr) {
	return std::make_shared<TcpConnection>(loop, sockFd, localAddr, peerAddr);
}

void TcpConnection::establishAsync() {
	if (close_ || establish_) {
		return;
	}
	loop_->runInLoop(std::bind(&TcpConnection::establish, shared_from_this()));
}

void TcpConnection::shutdownAsync() {
	if (close_) {
		return;
	}
	loop_->runInLoop(std::bind(&TcpConnection::shutdown, shared_from_this()));
}

void TcpConnection::closeAsync() {
	if (close_) {
		return;
	}
	loop_->runInLoop(std::bind(&TcpConnection::close, shared_from_this()));
}

void TcpConnection::destroyAsync() {
	if (close_) {
		return;
	}
	loop_->runInLoop(std::bind(&TcpConnection::destroy, shared_from_this()));
}

void TcpConnection::establish() {
	if (establish_) {
		return;
	}
	establish_ = true;
	// buffer
	readHead_ = readTail_ = bufferPool_->take();
	writeHead_ = writeTail_ = bufferPool_->take();
	readSize_ = writeSize_ = 0;
	watcher_->start();
	if (connectCallback_) {
		connectCallback_(shared_from_this());
	}
}

void TcpConnection::shutdown() {
	::shutdown(sockFd_, SHUT_WR);
}

void TcpConnection::close() {
	TcpConnectionPtr sharedthis = shared_from_this();
	disconnectCallback_(sharedthis);
	closeCallback_(sharedthis);
}

void TcpConnection::destroy() {
	if (close_) {
		return;
	}
	close_ = true;
	watcher_->stop();
}

void TcpConnection::sendData(PacketPtr packet) {
	if (close_) {
		return;
	}
	loop_->runInLoop(std::bind(&TcpConnection::send, this, packet));
}

void TcpConnection::send(PacketPtr packet) {
	uint16_t size = packet->getSize();
	 // write data
	if (writeData(packet->getPacket(), size + sizeof(uint16_t))) {
		watcher_->activeWrite();
	}
};

bool TcpConnection::PeekData(const char* data, const size_t size) {
	if (!data || 0 == size || size > readSize_) {
		return false;
	}

	char* buffer = const_cast<char*>(data);
	uint32_t readcount = readHead_->count; // head read count
	uint32_t rearcount = readHead_->capacity - readHead_->pop; // head rear count
	if (size < readcount) { // less head count
		if (size > rearcount) { //read both sides
			::memcpy(buffer, readHead_->buffer + readHead_->pop, rearcount); // pop -> end
			::memcpy(buffer + rearcount, readHead_->buffer, size - rearcount);
		} else { //read immediately
			::memcpy(buffer, readHead_->buffer + readHead_->pop, size); // pop -> pop +size
		}
	} else { // read head and other
		if (readcount > rearcount) { //read both sides
			::memcpy(buffer, readHead_->buffer + readHead_->pop, rearcount); // pop -> pop + rearcount
			::memcpy(buffer + rearcount, readHead_->buffer, readcount - rearcount); // begin -> all rear
		} else { //read immediately
			::memcpy(buffer, readHead_->buffer + readHead_->pop, readcount); // readcount
		}

		if (size > readcount) { // read other
			buffer += readcount;
			Buffer* readhead = readHead_->next;
			uint32_t rearsize = static_cast<uint32_t>(size) - readcount;
			do {
				readcount = readhead->count;
				if (rearsize < readcount) {
					::memcpy(buffer, readhead->buffer, rearsize);
					break;
				} else {
					::memcpy(buffer, readhead->buffer, readcount);
					if (rearsize > readcount) {
						buffer += readcount;
						readhead = readhead->next;
						rearsize -= readcount;
					} else { // rearsize == readcount
						break;
					}
				}
			} while (rearsize > 0);
		}
	}
	return true;
}

bool TcpConnection::ReadData(const char* data, const size_t size) {
	if (!data || 0 == size || size > readSize_) {
		return false;
	}

	char* buffer = const_cast<char*>(data);
	uint32_t readcount = readHead_->count; // head read count
	uint32_t rearcount = readHead_->capacity - readHead_->pop; // head rear count
	if (size < readcount) { // less head count
		if (size > rearcount) { //read both sides
			::memcpy(buffer, readHead_->buffer + readHead_->pop, rearcount); // pop -> end
			readHead_->pop = static_cast<uint32_t>(size) - rearcount; // move pop
			::memcpy(buffer + rearcount, readHead_->buffer, readHead_->pop); // begin -> new pop
		} else { //read immediately
			::memcpy(buffer, readHead_->buffer + readHead_->pop, size); // pop -> pop +size
			readHead_->pop = (readHead_->pop + static_cast<uint32_t>(size)) % readHead_->capacity;
		}
		readHead_->count -= static_cast<uint32_t>(size); // buffer not empty
	} else { // read head and other
		if (readcount > rearcount) { //read both sides
			::memcpy(buffer, readHead_->buffer + readHead_->pop, rearcount); // pop -> pop + rearcount
			::memcpy(buffer + rearcount, readHead_->buffer, readcount - rearcount); // begin -> all rear
		} else { //read immediately
			::memcpy(buffer, readHead_->buffer + readHead_->pop, readcount); // readcount
		}

		if (size > readcount) { // read other
			buffer += readcount;
			readHead_ = bufferPool_->putNext(readHead_);
			uint32_t rearsize = static_cast<uint32_t>(size) - readcount;
			do {
				readcount = readHead_->count;
				if (rearsize < readcount) {
					::memcpy(buffer, readHead_->buffer, rearsize);
					readHead_->pop = rearsize;
					break;
				} else {
					::memcpy(buffer, readHead_->buffer, readcount);
					if (rearsize > readcount) {
						buffer += readcount;
						readHead_ = bufferPool_->putNext(readHead_);
						rearsize -= readcount;
					} else { // rearsize == readcount
						if (readHead_ == readTail_) { // buffer empty
							readHead_->push = readHead_->pop = readHead_->count = 0;
						} else { // push move next
							readHead_ = bufferPool_->putNext(readHead_);
						}
						break;
					}
				}
			} while (rearsize > 0);
		} else { // size == readcount
			if (readHead_ == readTail_) { // buffer empty
				readHead_->push = readHead_->pop = readHead_->count = 0;
			} else { // push move next
				readHead_ = bufferPool_->putNext(readHead_);
			}
		}
	}
	readSize_ -= size;	// size
	return true;
}

bool TcpConnection::writeData(const char* data, const size_t size) {
	if (!data || 0 == size) {
		return false;
	}

	char* buffer = const_cast<char*>(data);
	uint32_t writecount = writeTail_->capacity - writeTail_->count;
	if (size < writecount) {
		uint32_t rearcount = writeTail_->capacity - writeTail_->push;
		if (writecount > rearcount) {
			::memcpy(writeTail_->buffer + writeTail_->push, buffer, rearcount);
			writeTail_->push = writecount - rearcount;
			::memcpy(writeTail_->buffer, buffer + rearcount, writeTail_->push);
		} else {
			::memcpy(writeTail_->buffer + writeTail_->push, buffer, writecount);
			writeTail_->push = (writeTail_->push + static_cast<uint32_t>(size)) % writeTail_->capacity;
		}
		writeTail_->count += static_cast<uint32_t>(size);
	} else {
		uint32_t rearsize = static_cast<uint32_t>(size);
		if (writecount > 0) {
			uint32_t rearcount = writeTail_->capacity - writeTail_->push;
			if (writecount > rearcount) {
				::memcpy(writeTail_->buffer + writeTail_->push, buffer, rearcount);
				::memcpy(writeTail_->buffer, buffer + rearcount, writecount - rearcount);
			} else {
				::memcpy(writeTail_->buffer + writeTail_->push, buffer, writecount);
			}
			writeTail_->push = writeTail_->pop;
			writeTail_->count = writeTail_->capacity;
			buffer += writecount;
			rearsize -= writecount;
		}
		while (rearsize > 0) {
			writeTail_ = bufferPool_->takeNext(writeTail_);
			writecount = writeTail_->capacity;
			if (rearsize > writecount) {
				::memcpy(writeTail_->buffer, buffer, writecount);
				writeTail_->count = writeTail_->capacity;
				buffer += writecount;
				rearsize -= writecount;
			} else {
				::memcpy(writeTail_->buffer, buffer, rearsize);
				writeTail_->push = writeTail_->count = rearsize;
				break;
			}
		}
	}
	writeSize_ += size;
	return true;
}

void TcpConnection::handleRead() {
	if (!watcher_->isReadable()) {
		return;
	}

	struct iovec iov[3];
	int iovcnt = 0;
	uint32_t rearcount = readTail_->capacity - readTail_->count;
	if (rearcount > 0) { // tail buffer
		if (readTail_->push < readTail_->pop) { // less(Tail<Head)
			iov[iovcnt].iov_base = readTail_->buffer + readTail_->push;
			iov[iovcnt].iov_len = readTail_->capacity - readTail_->count;
			++ iovcnt; // iovcnt = 1
		} else { // more(Tail>Head) and empty(Tail=Head)
			iov[iovcnt].iov_base = readTail_->buffer + readTail_->push;
			iov[iovcnt].iov_len = readTail_->capacity - readTail_->push;
			++ iovcnt; // iovcnt = 1
			if (readTail_->pop > 0) {
				iov[iovcnt].iov_base = readTail_->buffer;
				iov[iovcnt].iov_len = readTail_->pop;
				++ iovcnt; // iovcnt = 2
			}
		}
	}
	// next buffer (for read extra)
	Buffer* next = bufferPool_->takeNext(readTail_);
	iov[iovcnt].iov_base = next->buffer;
	iov[iovcnt].iov_len = next->capacity;
	++ iovcnt; // iovcnt <= 3
	// read fd
	const ssize_t readsize = ::readv(sockFd_, iov, iovcnt);
	if (readsize < 0) {
		// error
		watcher_->setReadable(false);
		return;
	}
	// read successful
	uint32_t size = rearcount + next->capacity;
	if (readsize > size) { // error?
		watcher_->setReadable(false);
		return;
	}
	// fill tail
	if (readsize < rearcount) {
		readTail_->push = (readTail_->push + static_cast<uint32_t>(readsize)) % readTail_->capacity;
		readTail_->count += static_cast<uint32_t>(readsize);
	} else { // tail full
		readTail_->push = readTail_->pop;
		readTail_->count = readTail_->capacity;
		if (readsize > rearcount) {// fill next
			next->count = next->push = static_cast<uint32_t>(readsize) - rearcount;
			readTail_ = next;
		}
	}
	// add readsize
	readSize_ += readsize;

	if (receiveCallback_) { // callback
		while (true) {
			uint16_t size;
			if (!PeekData((char*)&size, sizeof(uint16_t))) {
				break;
			}
			size = ntohs(size);
			if (size + sizeof(uint16_t) < readSize_) {
				break;
			}
			PacketPtr packet = Packet::create();
			if (!ReadData(packet->getPacket(), size + sizeof(uint16_t))) {
				break;
			}
			receiveCallback_(shared_from_this(), packet);
		}
	}
	// socket receive buffer not empty may be,  at use EPOLL ET
	if (readsize == size) {
		watcher_->activeRead();
	}
}

void TcpConnection::handleWrite() {
	if (!watcher_->isWriteable() || 0 == writeSize_) {
		return;
	}

	struct iovec iov[3];
	int iovcnt = 0;
	uint32_t writecount = writeHead_->count;
	if (writeHead_->pop < writeHead_->push) { // less(Head<Tail)
		iov[iovcnt].iov_base = writeHead_->buffer + writeHead_->pop;
		iov[iovcnt].iov_len = writecount;
		++ iovcnt; // iovcnt = 1
	} else { // more(Head>Tail) and full(Tail=Head)
		iov[iovcnt].iov_base = writeHead_->buffer + writeHead_->pop;
		iov[iovcnt].iov_len = writeHead_->capacity - writeHead_->pop;
		++ iovcnt; // iovcnt = 1
		if (writeHead_->push > 0) {
			iov[iovcnt].iov_base = writeHead_->buffer;
			iov[iovcnt].iov_len = writeHead_->push;
			++ iovcnt; // iovcnt = 2
		}
	}
	uint32_t size = writecount;
	Buffer* next = writeHead_->next;
	if (next) {
		iov[iovcnt].iov_base = next->buffer;
		iov[iovcnt].iov_len = next->count;
		++ iovcnt; // iovcnt <= 3
		size += next->count;
	}
	// write fd
	const ssize_t writesize = ::writev(sockFd_, iov, iovcnt);
	if (writesize < 0) {
		// error
		watcher_->setWriteable(false);
		return;
	}
	// write successful
	if (writesize > size) { // error?
		watcher_->setWriteable(false);
		return;
	}
	if (writesize < writecount) {
		writeHead_->pop = (writeHead_->pop + static_cast<uint32_t>(writesize)) % writeHead_->capacity;
		writeHead_->count -= static_cast<uint32_t>(writesize);
	} else if (writesize > writecount) {
		next->pop = static_cast<uint32_t>(writesize) - writecount;
		next->count -= writeHead_->pop;
		writeHead_ = bufferPool_->putNext(writeHead_);
	} else { // writesize == rearcount
		if (next) {// push move next
			writeHead_ = bufferPool_->putNext(writeHead_);
		} else {
			writeHead_->push = writeHead_->pop = writeHead_->count = 0;
		}
	}
	// remove size
	writeSize_ -= writesize;
	//if (m_sendCallback) { // callback
	//	m_sendCallback(shared_from_this(), writesize);
	//}
	// send buffer not empty, continue write
	if (writesize == size && writeSize_ > 0) {
		watcher_->activeWrite();
	}
}

void TcpConnection::handleClose() {
	TcpConnectionPtr sharedthis = shared_from_this();
	if (disconnectCallback_) {
		disconnectCallback_(sharedthis);
	}
	if (closeCallback_) {
		closeCallback_(sharedthis);
	}
}
