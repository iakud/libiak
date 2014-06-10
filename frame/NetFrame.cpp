#include "NetFrame.h"
#include "AsyncNet.h"

using namespace iak;

void NetFrame::init(uint32_t count) {
	AsyncNet::init(count);
}

void NetFrame::destroy() {
	AsyncNet::destroy();
}

void NetFrame::dispatch() {
	AsyncNet::dispatch();
}
