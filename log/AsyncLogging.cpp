#include "AsyncLogging.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

using namespace iak;

AsyncLogging::AsyncLogging()
	: running_(false)
	, thread_()
	, latch_(1)
	, mutex_()
	, cv_()
	, asyncLoggers_() {
	
}

AsyncLogging::~AsyncLogging() {
	if (running_) {
		stop();
	}
}

void AsyncLogging::append(AsyncLoggerPtr&& asyncLogger, const char* logline, int len) {
	std::unique_lock<std::mutex> lock(mutex_);
	asyncLogger->appendToBuffer_locked(logline, len);
	asyncLoggers_.insert(asyncLogger);
	cv_.notify_one();
}

void AsyncLogging::threadFunc() {
	assert(running_ == true);
	latch_.countDown();

	while (running_) {
		std::set<AsyncLoggerPtr> asyncLoggersToWrite;
		swapAsyncLoggersToWrite(asyncLoggersToWrite);
		for (const AsyncLoggerPtr& asyncLogger : asyncLoggersToWrite) {
			asyncLogger->appendBuffers_unlocked();
		}
	}
}

void AsyncLogging::swapAsyncLoggersToWrite(std::set<AsyncLoggerPtr>& asyncLoggersToWrite) {
	std::unique_lock<std::mutex> lock(mutex_);
	if (asyncLoggers_.empty()) {
		cv_.wait(lock);
	}
	asyncLoggersToWrite.swap(asyncLoggers_);
	for (const AsyncLoggerPtr& asyncLogger : asyncLoggersToWrite) {
		asyncLogger->swapBuffersToWrite_locked();
	}
}