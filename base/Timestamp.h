#ifndef IAK_BASE_TIMESTAMP_H
#define IAK_BASE_TIMESTAMP_H

#include <stdint.h>

namespace iak {

class Timestamp {
public:
	static Timestamp Now();

public:
	Timestamp()
		: m_time(0) {
	}

	/* Constructs a Timestamp object using
	 * a microseconds time value.*/
	explicit Timestamp(int64_t time)
		: m_time(0) {
	}
	
	/* Return the number of microseconds since
	 * January 1, 1970, 00:00:00 GMT
	 * represented by this Timestamp object.*/
	int64_t GetTime() const {
		return m_time;
	}

	operator bool() const {
		return m_time > 0;
	}

private:
	int64_t m_time;
};

inline bool operator==(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() == rhs.GetTime();
}

inline bool operator!=(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() != rhs.GetTime();
}

inline bool operator<(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() < rhs.GetTime();
}

inline bool operator<=(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() <= rhs.GetTime();
}

inline bool operator>(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() > rhs.GetTime();
}

inline bool operator>=(Timestamp& lhs, Timestamp& rhs) {
	return lhs.GetTime() >= rhs.GetTime();
}

} // end namespace iak

#endif // IAK_BASE_TIMESTAMP_H
