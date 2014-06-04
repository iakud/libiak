#include "base/LogFile.h"
#include "base/Logger.h"
#include <unistd.h>

using namespace iak;

std::unique_ptr<LogFile> g_logFile;

void outputFunc(const char* msg, int len) {
	g_logFile->append(msg, len);
}

void flushFunc() {
	g_logFile->flush();
}

int main(int argc, char* argv[]) {
	char name[256];
	strncpy(name, argv[0], 256);
	g_logFile.reset(new LogFile(::basename(name), 200*1000));
	Logger::SetOutput(outputFunc);
	Logger::SetFlush(flushFunc);

	std::string line = "1234567890 abcdefg FDAGFSADGASDF   ";
	for (int i=0; i<1000; ++i) {
		LOG_INFO<<line<<i;
		::usleep(1000);
	}

}
