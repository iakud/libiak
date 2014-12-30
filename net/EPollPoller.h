#ifndef IAK_NET_EPOLLPOLLER_H
#define IAK_NET_EPOLLPOLLER_H

#include <base/NonCopyable.h>

namespace iak {

struct epoll_event;
class Watcher;

class EPollPoller : public NonCopyable {
public:
	EPollPoller();
	virtual ~EPollPoller();

protected:
	static const int kEventSizeInit = 16;

	void poll(int timeout) override;
	void addWatcher(Watcher* watcher) override;
	void updateWatcher(Watcher* watcher) override;
	void removeWatcher(Watcher* watcher) override;

	int epollfd_;
	int eventsize_;
	struct epoll_event* events_;
}; // end class EPollPoller

} // end namespace iak

#endif // IAK_NET_EPOLLPOLLER_H
