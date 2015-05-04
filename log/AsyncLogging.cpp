#include "AsyncLogging.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

using namespace iak;

AsyncLogging::AsyncLogging()
	: running_(false)
	, thread_()
	, latch_(1)
	, mutex_()
	, cv_()
    , pendingFunctors_() {
	
}

void AsyncLogging::threadFunc() {
	assert(running_ == true);
	latch_.countDown();
	
	while (running_) {
		std::vector<Functor> functors;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (pendingFunctors_.empty()) {
				cv_.wait(lock);
			}
			functors.swap(pendingFunctors_);
		}
		for (Functor functor : functors) {
			functor();
		}
	}
}

