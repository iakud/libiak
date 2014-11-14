#include "AsyncLogging.h"

#include <stdio.h>
#include <memory.h>

using namespace iak;

AsyncLogging::AsyncLogging()
	: running_(false)
	, thread_(std::bind(&AsyncLogging::threadFunc, this), "AsyncLogging")
	, latch_(1)
	, mutex_()
	, cond_(mutex_)
    , pendingFunctors_() {
}

void AsyncLogging::threadFunc() {
	assert(running_ == true);
	latch_.countDown();
	
	while (running_) {
		std::vector<Functor> functors;
		{
			MutexGuard lock(mutex_);
			if (pendingFunctors_.empty()) {
				cond_.wait();
			}
			functors.swap(pendingFunctors_);
		}
		for (Functor functor : functors) {
			functor();
		}
	}
}

