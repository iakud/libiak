#ifndef IAK_BASE_THREAD_H
#define IAK_BASE_THREAD_H

#include "NonCopyable.h"

#include <atomic>
#include <memory>
#include <functional>
#include <stdint.h>
#include <pthread.h>

namespace iak {

extern __thread int t_tid;
extern __thread char t_tidstr[32];
extern __thread const char* t_name;

class Thread : public NonCopyable {
private:
	typedef std::function<void()> ThreadFunc;

public:
	static int tid() {
		if (t_tid == 0) {
			cacheTid();
		}
		return t_tid;
	}

	static const char* tidString() {
		if (t_tid == 0) {
			cacheTid();
		}
		return t_tidstr;
	}

	static const char* getName() {
		return t_name;
	}

	static bool isMainThread();
	static void sleepUsec(int64_t usec);
private:
	static void cacheTid();

public:
	explicit Thread(ThreadFunc&& func, const std::string& name);
	~Thread();

	void start();
	void join();

private:
	static const uint32_t kMicroSecondsPerSecond = 1e6;

	std::atomic_flag started_;
	std::atomic_flag joined_;
	pthread_t thread_;
	std::shared_ptr<int> tid_;
	ThreadFunc func_;
	std::string name_;
};

} // end namespace iak

#endif // IAK_BASE_THREAD_H
