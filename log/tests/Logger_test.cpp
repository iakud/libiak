#include <log/Logger.h>
#include <unistd.h>

using namespace iak::log;

int main(int argc, char* argv[]) {
	char name[256];
	//strncpy(name, argv[0], 256);
	std::string line = "1234567890 abcdefg FDAGFSADGASDF   ";
	::sprintf(name, "%s1", argv[0]);
	LoggerPtr logger1 = Logger::make(::basename(name), 200 * 1000, false, 3, true, true);
	for (int i=0; i<1000; ++i) {
		DLOG_INFO(logger1) << line << i;
		::usleep(1000);
	}
	::sprintf(name, "%s2", argv[0]);
	LoggerPtr logger2 = Logger::make(::basename(name), 200 * 1000, false, 3, true, true);
	for (int i = 0; i < 1000; ++i) {
		LOG_INFO(logger2) << line << i;
		::usleep(1000);
	}
}