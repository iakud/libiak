#ifndef IAK_BASE_TIMEZONE_H
#define IAK_BASE_TIMEZONE_H

#include <memory>
#include <time.h>

namespace iak {
namespace base {

// 1970~2030
class TimeZone {
public:
	explicit TimeZone(const char* zonefile);

	struct tm toLocalTime(time_t time) const;
	time_t fromLocalTime(const struct tm&) const;

	// gmtime(3)
	static struct tm toUtcTime(time_t time, bool yday = false);
	// timegm(3)
	static time_t fromUtcTime(const struct tm&);

	// year in [1900..2500], month in [1..12], day in [1..31]
	static time_t fromUtcTime(int year, int month, int day,int hour, int minute, int seconds);

	operator bool() const {
		return static_cast<bool>(data_);
	}

	struct Data;

private:
	std::shared_ptr<Data> data_;
};

} // end namespace base
} // end namespace iak

#endif  // MUDUO_BASE_TIMEZONE_H
