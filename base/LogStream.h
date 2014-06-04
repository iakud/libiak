#ifndef IAK_BASE_LOGSTREAM_H
#define IAK_BASE_LOGSTREAM_H

#include "NonCopyable.h"

#include <string>
#include <memory.h> // memcpy

namespace iak {

#define LOGSTREAM_SIZE 4096
#define LOGFORMAT_SIZE 32

class LogStream : public NonCopyable {
public:
	LogStream()
		: cur_(data_)
		, end_(data_ + sizeof data_) {
	}

	LogStream& operator<<(short v);
	LogStream& operator<<(unsigned short v);
	LogStream& operator<<(int v);
	LogStream& operator<<(unsigned int v);
	LogStream& operator<<(long v);
	LogStream& operator<<(unsigned long v);
	LogStream& operator<<(long long v);
	LogStream& operator<<(unsigned long long v);

	LogStream& operator<<(float v) { *this << static_cast<double>(v); return *this; }
	LogStream& operator<<(double v);
	LogStream& operator<<(char v) { append(&v, 1); return *this; }
	//LogStream& operator<<(unsigned char v) { Append(&v, 1); return *this; }
	LogStream& operator<<(const char* v) { append(v, strlen(v)); return *this; }
	LogStream& operator<<(const std::string& v) { append(v.c_str(), v.size()); return *this; }
	LogStream& operator<<(bool v) { append(v ? "1" : "0", 1); return *this; }
	LogStream& operator<<(const void*);

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(cur_ - data_); }
	void reset() { cur_ = data_; }

	void append(const char* buf, size_t len) {
		if (static_cast<size_t>(avail()) > len) {
			::memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

private:
	template<typename T>
	void formatInteger(T);

	int avail() const { return static_cast<int>(end_ - cur_); }

	char data_[LOGSTREAM_SIZE];
	char* cur_;
	char* end_;
}; // end class LogStream

class LogFormat {
public:
	template<typename T>
	LogFormat(const char* fmt, T val);

	const char* data() const { return data_; }
	int length() const { return length_; }

private:
	char data_[LOGFORMAT_SIZE];
	int length_;
}; // end class LogFormat

inline LogStream& operator<<(LogStream& stream, const LogFormat& fmt) {
	stream.append(fmt.data(), fmt.length());
	return stream; 
}

} // end namespace iak

#endif  // IAK_BASE_LOGSTREAM_H

