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

	LogStream& operator<<(bool v) { Append(v ? "1" : "0", 1); return *this; }
	LogStream& operator<<(short v) { formatInteger(static_cast<int>(v)); return *this; }
	LogStream& operator<<(unsigned short v) { formatInteger(static_cast<unsigned int>(v)); return *this; }
	LogStream& operator<<(int v) { formatInteger(v); return *this; }
	LogStream& operator<<(unsigned int v) { formatInteger(v); return *this; }
	LogStream& operator<<(long v) { formatInteger(v); return *this; }
	LogStream& operator<<(unsigned long v) { formatInteger(v); return *this; }
	LogStream& operator<<(long long v) { formatInteger(v); return *this; }
	LogStream& operator<<(unsigned long long v) { formatInteger(v); }
	LogStream& operator<<(float v) { *this << static_cast<double>(v); return *this; }
	LogStream& operator<<(double v);
	LogStream& operator<<(char v) { Append(&v, 1); return *this; }
	//LogStream& operator<<(unsigned char v) { Append(&v, 1); return *this; }
	LogStream& operator<<(const char* v) { Append(v, strlen(v)); return *this; }
	LogStream& operator<<(const std::string& v) { Append(v.c_str(), v.size()); return *this; }
	LogStream& operator<<(const void*);

	const char* GetData() const { return data_; }
	int GetLength() const { return static_cast<int>(cur_ - data_); }
	void Reset() { cur_ = data_; }

	void Append(const char* data, size_t len) {
		if (static_cast<size_t>(avail()) > len) {
			::memcpy(cur_, data, len);
			cur_ += len;
		}
	}

private:
	template<typename T>
	void formatInteger(T);

	int avail() const { return end_ - cur_; }

	char data_[LOGSTREAM_SIZE];
	char* cur_;
	char* end_;
}; // end class LogStream

class LogFormat {
public:
	template<typename T>
	LogFormat(const char* fmt, T val);

	const char* GetData() const { return data_; }
	int GetLength() const { return length_; }

private:
	char data_[LOGFORMAT_SIZE];
	int length_;
}; // end class LogFormat

inline LogStream& operator<<(LogStream& stream, const LogFormat& fmt) {
	stream.Append(fmt.GetData(), fmt.GetLength());
	return stream; 
}

} // end namespace iak

#endif  // IAK_BASE_LOGSTREAM_H

