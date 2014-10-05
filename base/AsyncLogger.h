#ifndef IAK_BASE_ASYNCLOGGGER_H
#define IAK_BASE_ASYNCLOGGGER_H

#include "NonCopyable.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"

#include <memory>
#include <vector>
#include <string>

namespace iak {

class AsyncLogger : public NonCopyable {
public:
	AsyncLogger(const std::string& basename,
				size_t rollSize,
				int flushInterval = 3);

	~AsyncLogger() {
		if (running_) {
			stop();
		}
	}

	void append(const char* logline, int len);

	void start() {
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop() {
		running_ = false;
		cond_.signal();
		thread_.join();
	}

private:
	AsyncLogger(const AsyncLogger&);  // ptr_container
	void operator=(const AsyncLogger&);  // ptr_container

	static const unsigned int kMicroSecondsPerSecond = 1e6;

	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr;

	void threadFunc();

	const unsigned int flushInterval_;
	bool running_;
	std::string basename_;
	size_t rollSize_;

	Thread thread_;
	CountDownLatch latch_;
	Mutex mutex_;
	Condition cond_;

	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	std::vector<BufferPtr> buffers_;
};

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGGER_H
