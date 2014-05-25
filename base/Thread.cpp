#include "Thread.h"
#include "Timestamp.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace iak {

__thread int t_id = 0;
__thread const char* t_name = "unknown";

class ThreadNameInitializer {
public:
	ThreadNameInitializer() {
		t_name = "main";
	}
};

ThreadNameInitializer init;

}

using namespace iak;

pid_t Thread::gettid() {
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

bool Thread::IsMainThread() {
	return GetId() == ::getpid();
}

void SleepUsec(int64_t usec) {
	struct timespec ts = {
		static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond),
		static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000)
	};
	::nanosleep(&ts, NULL);
}
