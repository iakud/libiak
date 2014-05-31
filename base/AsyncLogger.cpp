#include <muduo/base/AsyncLogging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Timestamp.h>

#include <stdio.h>

#define ASYNCLOGGER_BUFFER_SIZE 4096 * 1024

namespace iak {

class AsyncLogger::Buffer : public NonCopyable {
public:
	Buffer()
		: cur_(data_) {
	}

	~Buffer() {
	}

	void Append(const char* buf, size_t len) {
		if ((size_t)avail() > len) {
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* GetData() const {
		return data_;
	}

	int GetLength() const {
		return static_cast<int>(cur_ - data_);
	}

	char* Current() { return cur_; }
	int Avail() const { return end() - cur_; }
	void Add(size_t len) { cur_ += len; }

	void Reset() { cur_ = data_;}
	void Zero() { ::bzero(data_, sizeof(data_); }
private:
	const char* end() const {
		return data_ + sizeof(data_);
	}

	char data_[ASYNCLOGGER_BUFFER_SIZE];
	char* cur_;
}; // end class AsyncLogger::Buffer

} // end namespace iak

using namespace iak;

AsyncLogger::AsyncLogger(const string& basename,
						size_t rollSize,
						int flushInterval)
	: flushInterval_(flushInterval)
	, running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogger::threadFunc, this), "Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_)
	, currentBuffer_(std::make_shared<Buffer>())
    , nextBuffer_(std::make_shared<Buffer>())
    , buffers_() {
	currentBuffer_->Zero();
	nextBuffer_->Zero();
	buffers_.reserve(16);
}

void AsyncLogger::append(const char* logline, int len) {
	MutexGuard lock(mutex_);
	if (currentBuffer_->Avail() > len) {
		currentBuffer_->Append(logline, len);
	} else {
		buffers_.push_back(currentBuffer_.release());

		if (nextBuffer_) {
			currentBuffer_ = boost::ptr_container::move(nextBuffer_);
		} else {
			currentBuffer_.reset(new Buffer); // Rarely happens
		}
		currentBuffer_->append(logline, len);
		cond_.Notify();
	}
}

void AsyncLogger::threadFunc() {
	assert(running_ == true);
	latch_.countDown();
	LogFile output(basename_, rollSize_, false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);
  while (running_)
  {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());

    {
      muduo::MutexLockGuard lock(mutex_);
      if (buffers_.empty())  // unusual usage!
      {
        cond_.waitForSeconds(flushInterval_);
      }
      buffers_.push_back(currentBuffer_.release());
      currentBuffer_ = boost::ptr_container::move(newBuffer1);
      buffersToWrite.swap(buffers_);
      if (!nextBuffer_)
      {
        nextBuffer_ = boost::ptr_container::move(newBuffer2);
      }
    }

    assert(!buffersToWrite.empty());

    if (buffersToWrite.size() > 25)
    {
      char buf[256];
      snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
               Timestamp::now().toFormattedString().c_str(),
               buffersToWrite.size()-2);
      fputs(buf, stderr);
      output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
    }

    for (size_t i = 0; i < buffersToWrite.size(); ++i)
    {
      // FIXME: use unbuffered stdio FILE ? or use ::writev ?
      output.append(buffersToWrite[i].data(), buffersToWrite[i].length());
    }

    if (buffersToWrite.size() > 2)
    {
      // drop non-bzero-ed buffers, avoid trashing
      buffersToWrite.resize(2);
    }

    if (!newBuffer1)
    {
      assert(!buffersToWrite.empty());
      newBuffer1 = buffersToWrite.pop_back();
      newBuffer1->reset();
    }

    if (!newBuffer2)
    {
      assert(!buffersToWrite.empty());
      newBuffer2 = buffersToWrite.pop_back();
      newBuffer2->reset();
    }

    buffersToWrite.clear();
    output.flush();
  }
  output.flush();
}

