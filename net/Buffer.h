#ifndef IAK_NET_BUFFERPOOL_H
#define IAK_NET_BUFFERPOOL_H

#include <base/NonCopyable.h>

#include <stdint.h>
#include <stdio.h>

namespace iak {

struct Buffer {
	char* buffer = NULL;
	int capacity = 0;
	int count = 0;
	int push = 0;
	int pop = 0;
	Buffer* next = NULL;
}; // end struct Buffer

class BufferPool : public NonCopyable {
public:
	BufferPool(const int capacity, const int size)
		: capacity_(capacity)
		, size_(size)
		, count_(0) {
	}

	~BufferPool() {
	}

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
	const int capacity_;
	const int size_;
	uint32_t count_;
	Buffer* head_;		// head buffer
	Buffer* tail_;		// tail buffer
}; // end class BufferPool

} // end namespace iak

#endif // IAK_NET_BUFFERPOOL_H
