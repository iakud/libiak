#ifndef IAK_NET_EPOLLPOLLER_H
#define IAK_NET_EPOLLPOLLER_H

#include <base/NonCopyable.h>

struct epoll_event;

namespace iak {

class Watcher;

class EPollPoller : public NonCopyable {
public:
	EPollPoller();
	virtual ~EPollPoller();

	void poll(int timeout);
	void addWatcher(Watcher* watcher);
	void updateWatcher(Watcher* watcher);
	void removeWatcher(Watcher* watcher);

protected:
	static const int kEventSizeInit = 16;

	int epollfd_;
	int eventsize_;
	struct epoll_event* events_;
}; // end class EPollPoller

} // end namespace iak

#endif // IAK_NET_EPOLLPOLLER_H
