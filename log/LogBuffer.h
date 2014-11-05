#ifndef IAK_LOG_LOGBUFFER_H
#define IAK_LOG_LOGBUFFER_H

#include <base/NonCopyable.h>

#include <memory.h> // memcpy
#include <memory>

namespace iak {

class LogBuffer;
typedef std::shared_ptr<LogBuffer> LogBufferPtr;

class LogBuffer : public NonCopyable {

public:
	static LogBufferPtr make() {
		return std::make_shared<LogBuffer>();
	}

public:
	LogBuffer()
		: cur_(data_)
		, end_(data_ + sizeof data_) {
	}

	~LogBuffer() {
	}

	void append(const char* buf, size_t len) {
		if (static_cast<size_t>(avail()) > len) {
			::memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* data() const {
		return data_;
	}

	int length() const {
		return static_cast<int>(cur_ - data_);
	}

	int avail() const { return static_cast<int>(end_ - cur_); }

	void reset() { cur_ = data_; }

private:
	static const int kBufferSize = 4096 * 1024;

	char data_[kBufferSize];
	char* cur_;
	char* end_;
}; // end class LogBuffer

} // end namespace iak

#endif  // IAK_LOG_LOGBUFFER_H

