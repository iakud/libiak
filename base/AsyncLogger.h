#ifndef IAK_BASE_ASYNCLOGGGER_H
#define IAK_BASE_ASYNCLOGGGER_H

#include "NonCopyable.h"
#include <muduo/base/BlockingQueue.h>
#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include "Mutex.h"
#include "Thread.h"

#include <muduo/base/LogStream.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace iak {

class AsyncLogger : public NonCopyable {
public:
	AsyncLogger(const string& basename,
				size_t rollSize,
				int flushInterval = 3);

	~AsyncLogger() {
		if (running_) {
			stop();
		}
	}

	void Append(const char* logline, int len);

	void Start() {
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop() {
		running_ = false;
		cond_.notify();
		thread_.join();
	}

 private:

  // declare but not define, prevent compiler-synthesized functions
  AsyncLogging(const AsyncLogging&);  // ptr_container
  void operator=(const AsyncLogging&);  // ptr_container

	void threadFunc();

	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr;

  const int flushInterval_;
  bool running_;
  string basename_;
  size_t rollSize_;
  muduo::Thread thread_;
  muduo::CountDownLatch latch_;
  muduo::MutexLock mutex_;
  muduo::Condition cond_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	std::vector<BufferPtr> buffers_;
};

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGGER_H
