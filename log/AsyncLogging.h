#ifndef IAK_BASE_ASYNCLOGGING_H
#define IAK_BASE_ASYNCLOGGING_H

#include "LogBuffer.h"
#include "LogFile.h"

#include <base/NonCopyable.h>
#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>
#include <base/Mutex.h>
#include <base/Thread.h>

#include <memory>
#include <vector>
#include <string>

namespace iak {

class AsyncLogging : public NonCopyable {
public:
	AsyncLogging(int flushInterval = 3);

	~AsyncLogging() {
		if (running_) {
			stop();
		}
	}

	void append(LogFilePtr logfile, const char* logline, int len);

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
	//AsyncLogger(const AsyncLogger&);  // ptr_container
	//void operator=(const AsyncLogger&);  // ptr_container

	static const uint32_t kNanoSecondsPerSecond = 1e9;

	void threadFunc();

	const uint32_t flushInterval_;
	bool running_;

	Thread thread_;
	CountDownLatch latch_;
	Mutex mutex_;
	Condition cond_;

	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	std::vector<LogBufferPtr> buffers_;
};

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGING_H
