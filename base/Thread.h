#ifndef IAK_BASE_THREAD_H
#define IAK_BASE_THREAD_H

#include "NonCopyable.h"
#include <stdint.h>
#include <pthread.h>

namespace iak {

extern __thread int t_id;
extern __thread const char* t_name;

class Thread : public NonCopyable {
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
};

} // end namespace iak

#endif // IAK_BASE_THREAD_H
