#ifndef IAK_BASE_LOGSTREAM_H
#define IAK_BASE_LOGSTREAM_H

#include "NonCopyable.h"

#include <assert.h>
#include <memory.h> // memcpy
#include <string>

namespace iak {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

class LogStream : public NonCopyable {
public:
  typedef LogBuffer<kSmallBuffer> Buffer;

	LogStream& operator<<(bool v) {
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

	LogStream& operator<<(short v) {
		formatInteger(static_cast<int>(v));
		return *this;
	}

	LogStream& operator<<(unsigned short v) {
		formatInteger(static_cast<unsigned int>(v));
		return *this;
	}

	LogStream& operator<<(int v) {
		formatInteger(v);
		return *this;
	}

	LogStream& operator<<(unsigned int v) {
		formatInteger(v);
		return *this;
	}

	LogStream& operator<<(long v) {
		formatInteger(v);
		return *this;
	}

	LogStream& operator<<(unsigned long v) {
		formatInteger(v);
		return *this;
	}

	LogStream& operator<<(long long v) {
		formatInteger(v);
		return *this;
	}

	LogStream& operator<<(unsigned long long v) {
		formatInteger(v);
	}


	LogStream& operator<<(float v) {
		*this << static_cast<double>(v);
		return *this;
	}

	LogStream& operator<<(double v);

	LogStream& operator<<(char v) {
		buffer_.append(&v, 1);
		return *this;
	}

	LogStream& operator<<(unsigned char v) {
		buffer_.append(&v, 1);
		return *this;
	}

	LogStream& operator<<(const char* v) {
		buffer_.append(v, strlen(v));
		return *this;
	}

	LogStream& operator<<(const std::string& v) {
		buffer_.append(v.c_str(), v.size());
		return *this;
	}

	LogStream& operator<<(const void*);

	void append(const char* data, int len) { buffer_.append(data, len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }

private:
	template<typename T>
	void formatInteger(T);

	Buffer buffer_;
}; // end class LogStream

} // end namespace iak

#endif  // IAK_BASE_LOGSTREAM_H

