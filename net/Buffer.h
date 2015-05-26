#ifndef IAK_NET_BUFFERPOOL_H
#define IAK_NET_BUFFERPOOL_H

#include <stdint.h>
#include <stdio.h>

namespace iak {
namespace net {

struct Buffer {
	char* buffer = NULL;
	uint32_t capacity = 0;
	uint32_t count = 0;
	uint32_t push = 0;
	uint32_t pop = 0;
	Buffer* next = NULL;
}; // end struct Buffer

class BufferPool {
public:
	BufferPool(const uint32_t capacity, const uint32_t size)
		: capacity_(capacity)
		, size_(size)
		, count_(0) {
	}

	~BufferPool() {
	}
	// noncopyable
	BufferPool(const BufferPool&) = delete;
	BufferPool& operator=(const BufferPool&) = delete;

	// put, return next buffer
	Buffer* putNext(Buffer* buffer) {
		Buffer* next = buffer->next;
		put(buffer);
		return next;
	}

	// take, return next buffer
	Buffer* takeNext(Buffer* buffer) {
		Buffer* next = take();
		buffer->next = next;
		return next;
	}

	void put(Buffer* buffer) {
		delete[] buffer->buffer;
		delete buffer;
	}

	Buffer* take() {
		Buffer* buffer = new Buffer();
		buffer->buffer = new char[capacity_];
		buffer->capacity = capacity_;
		//buffer->count = 0;
		//buffer->push = 0;
		//buffer->pop = 0;
		//buffer->next = NULL;
		return buffer;
	}

private:
	const uint32_t capacity_;
	const uint32_t size_;
	uint32_t count_;
	Buffer* head_;		// head buffer
	Buffer* tail_;		// tail buffer
}; // end class BufferPool

} // end namespace net
} // end namespace iak

#endif // IAK_NET_BUFFERPOOL_H
