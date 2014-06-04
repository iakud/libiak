#ifndef IAK_BASE_DATE_H
#define IAK_BASE_DATE_H


#include <string>

namespace iak {

struct tm;

// 1900..2500
class Date {
public:
	struct YearMonthDay {
		int year;	// [1900..2500]
		int month;	// [1..12]
		int day;	// [1..31]
	};
	
	static const int kDaysPerWeek = 7;
	static const int kJulianDayOf1970_01_01;

public:
	Date()
		: julianDayNumber_(0) {
	}

	Date(int year, int month, int day);

	explicit Date(int julianDayNum)
		: julianDayNumber_(julianDayNum) {
	}

	explicit Date(const struct tm&);

	void swap(Date& that) {
		std::swap(julianDayNumber_, that.julianDayNumber_);
	}

	bool valid() const { return julianDayNumber_ > 0; }

	std::string toIsoString() const;

	struct YearMonthDay yearMonthDay() const;

	int year() const {
		return yearMonthDay().year;
	}

	int month() const {
		return yearMonthDay().month;
	}

	int day() const {
		return yearMonthDay().day;
	}

	// [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
	int weekDay() const {
		return (julianDayNumber_+1) % kDaysPerWeek;
	}

	int julianDayNumber() const { return julianDayNumber_; }

	bool operator<(Date& rhs) {
		return julianDayNumber_ < rhs.julianDayNumber_;
	}

	bool operator==(Date& rhs) {
		return julianDayNumber_ == y.julianDayNumber_;
	}

private:
	int julianDayNumber_;
}; // end class Date

} // end namespace iak

#endif  // MUDUO_BASE_DATE_H
