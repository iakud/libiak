#ifndef IAK_BASE_LOGBUFFER_H
#define IAK_BASE_LOGBUFFER_H

#include "NonCopyable.h"

namespace iak {

template<int SIZE>
class LogBuffer : public NonCopyable {
public:
	LogBuffer()
		: cur_(data_) {
	}

	~LogBuffer() {
	}

	void Append(const char* buf, size_t len) {
		if ((size_t)avail() > len) {
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* GetData() const { return data_; }
	size_t GetLength() const {
		return static_cast<int>(cur_ - data_);
	}

	char* GetCurrent() { return cur_; }
	int Avail() const { return end() - cur_; }
	void Add(size_t len) { cur_ += len; }

	void reset() { cur_ = data_;}
	void bzero() { ::bzero(data_, sizeof(data_); }
private:
	const char* end() const {
		return data_ + sizeof(data_);
	}

	char data_[SIZE];
	char* cur_;
};

} // end namespace iak
#endif // IAK_BASE_LOGBUFFER_H
