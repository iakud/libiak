#include "LogStream.h"

#include <type_traits>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace iak {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof digits == 20, "sizeof digits == 20");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "sizeof digitsHex == 17");

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value) {
	T i = value;
	char* p = buf;

	do {
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0) {
		*p++ = '-';
	}

	*p = '\0';
	std::reverse(buf, p);
	return p - buf;
}

size_t convertHex(char buf[], uintptr_t value) {
	uintptr_t i = value;
	char* p = buf;

	do {
		int lsd = i % 16;
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

} // end namespace iak

#define NUMERICSIZE_MAX 32

using namespace iak;

static_assert(NUMERICSIZE_MAX - 10 > std::numeric_limits<double>::digits10, "NUMERICSIZE_MAX - 10 > std::numeric_limits<double>::digits10");
static_assert(NUMERICSIZE_MAX - 10 > std::numeric_limits<long double>::digits10, "NUMERICSIZE_MAX - 10 > std::numeric_limits<long double>::digits10");
static_assert(NUMERICSIZE_MAX - 10 > std::numeric_limits<long>::digits10, "NUMERICSIZE_MAX - 10 > std::numeric_limits<long>::digits10");
static_assert(NUMERICSIZE_MAX - 10 > std::numeric_limits<long long>::digits10, "NUMERICSIZE_MAX - 10 > std::numeric_limits<long long>::digits10");

LogStream& LogStream::operator<<(short v) {
	formatInteger(static_cast<int>(v));
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
	formatInteger(static_cast<unsigned int>(v));
	return *this;
}

LogStream& LogStream::operator<<(int v) {
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v) {
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v) {
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
	formatInteger(v);
	return *this;
}

template<typename T>
void LogStream::formatInteger(T v) {
	if (avail() >= NUMERICSIZE_MAX) {
		cur_ += convert(cur_, v);
	}
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
LogStream& LogStream::operator<<(double v) {
	if (avail() >= NUMERICSIZE_MAX) {
		cur_ += ::snprintf(cur_, NUMERICSIZE_MAX, "%.12g", v);
	}
	return *this;
}

LogStream& LogStream::operator<<(const void* p) {
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if (avail() >= NUMERICSIZE_MAX) {
		cur_[0] = '0';
		cur_[1] = 'x';
		cur_ += 2;
		cur_ += convertHex(cur_, v);
	}
	return *this;
}

template<typename T>
LogFormat::LogFormat(const char* fmt, T val) {
	static_assert(std::is_arithmetic<T>::value, "std::is_arithmetic<T>::value");
	length_ = ::snprintf(data_, sizeof data_, fmt, val);
	assert(static_cast<size_t>(length_) < sizeof data_);
}

// explicit instantiations
template LogFormat::LogFormat(const char* fmt, char);
template LogFormat::LogFormat(const char* fmt, unsigned char);

template LogFormat::LogFormat(const char* fmt, short);
template LogFormat::LogFormat(const char* fmt, unsigned short);
template LogFormat::LogFormat(const char* fmt, int);
template LogFormat::LogFormat(const char* fmt, unsigned int);
template LogFormat::LogFormat(const char* fmt, long);
template LogFormat::LogFormat(const char* fmt, unsigned long);
template LogFormat::LogFormat(const char* fmt, long long);
template LogFormat::LogFormat(const char* fmt, unsigned long long);

template LogFormat::LogFormat(const char* fmt, float);
template LogFormat::LogFormat(const char* fmt, double);
