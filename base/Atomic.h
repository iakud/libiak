#ifndef IAK_BASE_ATOMIC_H
#define IAK_BASE_ATOMIC_H

#include "NonCopyable.h"

#include <stdint.h>

namespace iak {

class AtomicInt32 : public NonCopyable {
public:
	AtomicInt32()
		: value_(0) {
	}
	
	// if need copying and assignment
	//
	// AtomicInt32(const AtomicInt32& rhs)
	//   : m_value(rhs.get())
	// {}
	//
	// AtomicInt32& operator=(const AtomicInt32& rhs)
	// {
	//   getAndSet(rhs.get());
	//   return *this;
	// }

	int32_t get() {
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	int32_t getAndAdd(int32_t value) {
		return __sync_fetch_and_add(&value_, value);
	}

	int32_t addAndGet(int32_t value) {
		return getAndAdd(value) + value;
	}

	int32_t incrementAndGet() {
		return addAndGet(1);
	}

	int32_t decrementAndGet() {
		return addAndGet(-1);
	}

	void add(int32_t value) {
		getAndAdd(value);
	}

	void increment() {
		incrementAndGet();
	}

	void decrement() {
		decrementAndGet();
	}

	int32_t getAndSet(int32_t value) {
		return __sync_lock_test_and_set(&value_, value);
	}

private:
	volatile int32_t value_;
}; // end class AtomicInt32

class AtomicInt64 : public NonCopyable {
public:
	AtomicInt64()
		: value_(0) {
	}
	
	// if need copying and assignment
	//
	// AtomicInt64(const AtomicInt64& rhs)
	//   : m_value(rhs.get())
	// {}
	//
	// AtomicInt64& operator=(const AtomicInt64& rhs)
	// {
	//   getAndSet(rhs.get());
	//   return *this;
	// }

	int64_t get() {
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	int64_t getAndAdd(int64_t value) {
		return __sync_fetch_and_add(&value_, value);
	}

	int64_t addAndGet(int64_t value) {
		return getAndAdd(value) + value;
	}

	int64_t incrementAndGet() {
		return addAndGet(1);
	}

	int64_t decrementAndGet() {
		return addAndGet(-1);
	}

	void add(int64_t value) {
		getAndAdd(value);
	}

	void increment() {
		incrementAndGet();
	}

	void decrement() {
		decrementAndGet();
	}

	int64_t getAndSet(int64_t value) {
		return __sync_lock_test_and_set(&value_, value);
	}

private:
	volatile int64_t value_;
}; // end class AtomicInt64

} // end namespace iak

#endif  // MUDUO_BASE_ATOMIC_H
