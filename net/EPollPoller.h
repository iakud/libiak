#ifndef IAK_NET_EPOLLPOLLER_H
#define IAK_NET_EPOLLPOLLER_H

struct epoll_event;

namespace iak {

class Watcher;

class EPollPoller {
protected:
	static const int kEventSizeInit = 16;
public:
	EPollPoller();
	virtual ~EPollPoller();
	// noncopyable
	EPollPoller(const EPollPoller&) = delete;
	EPollPoller& operator=(const EPollPoller&) = delete;

	void poll(int timeout);
	void addWatcher(Watcher* watcher);
	void updateWatcher(Watcher* watcher);
	void removeWatcher(Watcher* watcher);

protected:
	int epollfd_;
	int eventsize_;
	struct epoll_event* events_;
}; // end class EPollPoller

} // end namespace iak

#endif // IAK_NET_EPOLLPOLLER_H
