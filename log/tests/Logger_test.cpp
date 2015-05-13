#include <log/LogMessage.h>
#include <log/Logger.h>
#include <unistd.h>

using namespace iak;

LoggerPtr g_loggerPtr;

int main(int argc, char* argv[]) {
	char name[256];
	strncpy(name, argv[0], 256);
	g_loggerPtr = Logger::make(::basename(name), 200 * 1000);

	std::string line = "1234567890 abcdefg FDAGFSADGASDF   ";
	for (int i=0; i<1000; ++i) {
		LOG_INFO(g_loggerPtr) << line << i;
		::usleep(1000);
	}
}
