#include "Thread.h"

#include <unistd.h>
#include <sys/syscall.h>

#define MICROSEC_PER_SEC (uint32_t(1e6))
#define NANOSEC_PER_MICROSEC (uint32_t(1e3))

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

class ThreadRoutine : public NonCopyable {
public:
	typedef std::function<void()> ThreadFunc;
public:
	ThreadRoutine(ThreadFunc& func, 
				const std::string& name,
				std::shared_ptr<int>& tid)
		: m_func(func)
		, m_name(name)
		, m_tid(tid) {
	}

	void Run() {
		std::shared_ptr<int> tid = m_tid.lock();
		if (tid) {
			*tid = Thread::GetId();
		}
		t_name = m_name.c_str();
		m_func();
	}

private:
	ThreadFunc m_func;
	std::string m_name;
	std::weak_ptr<int> m_tid;
};

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
	: m_started(ATOMIC_FLAG_INIT)
	, m_joined(ATOMIC_FLAG_INIT)
	, m_thread(0)
	, m_tid(new pid_t(0))
	, m_func(func)
	, m_name(name) {
}

Thread::~Thread() {
	if (m_started.test_and_set() && !m_joined.test_and_set()) {
		::pthread_detach(m_thread);
	}
}

void Thread::Start() {
	if (m_started.test_and_set()) {
		return;
	}

	ThreadRoutine* routine = new ThreadRoutine(m_func, m_name, m_tid);
	if (::pthread_create(&m_thread, NULL, StartRoutine, routine)) {
		delete routine;
		// LOG
	}
}

void Thread::Join() {
	if (m_joined.test_and_set()) {
		return;
	}

	::pthread_join(m_thread, NULL);
}
