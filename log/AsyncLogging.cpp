#include "AsyncLog.h"
#include "LogFile.h"

#include <base/Timestamp.h>

#include <stdio.h>
#include <memory.h>

using namespace iak;

AsyncLogging::AsyncLogging(int flushInterval)
	: flushInterval_(flushInterval)
	, running_(false)
	, thread_(std::bind(&AsyncLogging::threadFunc, this), "Logger")
	, latch_(1)
	, mutex_()
	, cond_(mutex_)
	, currentBuffer_(std::make_shared<Buffer>())
    , nextBuffer_(std::make_shared<Buffer>())
    , buffers_() {
	//currentBuffer_->bzero();
	//nextBuffer_->bzero();
	buffers_.reserve(16);
}

void AsyncLogging::append(LogFilePtr logfile, const char* logline, int len) {
	MutexGuard lock(mutex_);
	if (currentBuffer_->avail() > len) {
		currentBuffer_->append(logline, len);
	} else {
		buffers_.push_back(currentBuffer_);
		if (nextBuffer_) {
			currentBuffer_.reset();
			currentBuffer_.swap(nextBuffer_);
		} else {
			currentBuffer_.reset(new Buffer); // Rarely happens
		}
		currentBuffer_->append(logline, len);
		cond_.signal();
	}
}

void AsyncLogging::threadFunc() {
	assert(running_ == true);
	latch_.countDown();
	LogFile output(basename_, rollSize_, false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	//newBuffer1->bzero();
	//newBuffer2->bzero();
	std::vector<BufferPtr> buffersToWrite;
	buffersToWrite.reserve(16);
	while (running_) {
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		assert(buffersToWrite.empty());

		{
			MutexGuard lock(mutex_);
			if (buffers_.empty()) {  // unusual usage!
				cond_.timedwait(flushInterval_ * kNanoSecondsPerSecond);
			}
			buffers_.push_back(currentBuffer_);
			currentBuffer_.reset();
			currentBuffer_.swap(newBuffer1);
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_) {
				nextBuffer_.swap(newBuffer2);
			}
		}

		assert(!buffersToWrite.empty());
		if (buffersToWrite.size() > 25) {
			char buf[256];
			::snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
				Timestamp::now().toFormattedString().c_str(), buffersToWrite.size()-2);
			::fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
		}

		for (size_t i = 0; i < buffersToWrite.size(); ++i) {
			output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
		}

		if (buffersToWrite.size() > 2) {
			buffersToWrite.resize(2);
		}

		if (!newBuffer1) {
			assert(!buffersToWrite.empty());
			newBuffer1 = buffersToWrite.back();
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2) {
			assert(!buffersToWrite.empty());
			newBuffer2 = buffersToWrite.back();
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}

