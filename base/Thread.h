#ifndef IAK_BASE_THREAD_H
#define IAK_BASE_THREAD_H

#include "NonCopyable.h"

#include <atomic>
#include <memory>
#include <functional>
#include <stdint.h>
#include <pthread.h>

namespace iak {

extern __thread int t_id;
extern __thread const char* t_name;

class Thread : public NonCopyable {
private:
	typedef std::function<void()> ThreadFunc;

public:
	static int GetId() {
		if (t_id == 0) {
			t_id = gettid();
		}
		return t_id;
	}

	static const char* GetName() {
		return t_name;
	}

	static bool IsMainThread();
	static void SleepUsec(int64_t usec);
private:
	static pid_t gettid();

public:
	explicit Thread(ThreadFunc&& func, const std::string& name);
	~Thread();

	void Start();
	void Join();

private:
	std::atomic_flag m_started;
	std::atomic_flag m_joined;
	pthread_t m_thread;
	std::shared_ptr<int> m_tid;
	ThreadFunc m_func;
	std::string m_name;
};

} // end namespace iak

#endif // IAK_BASE_THREAD_H
