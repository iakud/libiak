#include "Thread.h"

#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MICROSEC_PER_SEC (uint32_t(1e6))
#define NANOSEC_PER_MICROSEC (uint32_t(1e3))

namespace iak {

__thread int t_tid = 0;
__thread char t_tidstr[32];
__thread const char* t_name = "unknown";

class ThreadNameInitializer {
public:
	ThreadNameInitializer() {
		t_name = "main";
	}
}; // end class ThreadNameInitialize

ThreadNameInitializer init;

class ThreadRoutine : public NonCopyable {
public:
	typedef std::function<void()> ThreadFunc;
public:
	ThreadRoutine(ThreadFunc& func, 
				const std::string& name,
				std::shared_ptr<int>& tid)
		: func_(func)
		, name_(name)
		, tid_(tid) {
	}

	void Run() {
		std::shared_ptr<int> tid = tid_.lock();
		if (tid) {
			*tid = Thread::GetTid();
		}
		t_name = name_.c_str();
		func_();
	}

private:
	ThreadFunc func_;
	std::string name_;
	std::weak_ptr<int> tid_;
}; // end class ThreadRoutine

} // end namespace iak

using namespace iak;

void Thread::cacheTid() {
	t_tid = static_cast<int>(::syscall(SYS_gettid));
	int n = ::snprintf(t_tidstr, sizeof t_tidstr, "%5d ", t_tid);
	(void) n;
	assert(n == 6);
}

bool Thread::IsMainThread() {
	return GetTid() == ::getpid();
}

void SleepUsec(int64_t usec) {
	struct timespec ts = {
		static_cast<time_t>(usec / MICROSEC_PER_SEC),
		static_cast<long>(usec % MICROSEC_PER_SEC * NANOSEC_PER_MICROSEC)
	};
	::nanosleep(&ts, NULL);
}

void* StartRoutine(void* arg) {
	std::unique_ptr<ThreadRoutine> routine(static_cast<ThreadRoutine*>(arg));
	routine->Run();
	return NULL;
}

Thread::Thread(ThreadFunc&& func, const std::string& name)
	: started_(ATOMIC_FLAG_INIT)
	, joined_(ATOMIC_FLAG_INIT)
	, thread_(0)
	, tid_(new pid_t(0))
	, func_(func)
	, name_(name) {
}

Thread::~Thread() {
	if (started_.test_and_set() && !joined_.test_and_set()) {
		::pthread_detach(thread_);
	}
}

void Thread::Start() {
	if (started_.test_and_set()) {
		return;
	}
	ThreadRoutine* routine = new ThreadRoutine(func_, name_, tid_);
	if (::pthread_create(&thread_, NULL, StartRoutine, routine)) {
		delete routine;
		// LOG
	}
}

void Thread::Join() {
	if (joined_.test_and_set()) {
		return;
	}
	::pthread_join(thread_, NULL);
}
