#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <vector>
#include <algorithm>

template <typename T>
struct Less {
	bool operator() (const T& x, const T& y) const {
		return x < y;
	}
};

template <typename T, typename Compare = Less<T>>
class MinHeap {
public:
	MinHeap(const Compare& comp = Compare());
	size_t size() { return heap_.size(); }
	bool empty() { return heap_.empty(); }
	void clear() { heap_.clear(); }
	T min() { return heap_.front(); }

	void insert(const T& node);
	T remove();

private:
	void siftup();
	void siftdown();

	std::vector<T> heap_;
	Compare comp_;
};

template <typename T, typename Compare>
MinHeap<T, Compare>::MinHeap(const Compare& comp) : comp_(comp) {
}

template <typename T, typename Compare>
void MinHeap<T, Compare>::insert(const T& node) {
	heap_.push_back(node);
	siftup();
}

template <typename T, typename Compare>
T MinHeap<T, Compare>::remove() {
	T node = heap_.front();
	heap_.front() = heap_.back();
	heap_.pop_back();
	siftdown();
	return node;
}

template <typename T, typename Compare>
void MinHeap<T, Compare>::siftup() {
	size_t index = heap_.size() - 1;
	size_t parent = (index - 1) / 2;
	while (index > 0) {
		if (comp_(heap_[index], heap_[parent])) {
			std::swap(heap_[parent], heap_[index]);
			index = parent;
			parent = (index - 1) / 2;
		} else {
			break;
		}
	}
}

template <typename T, typename Compare>
void MinHeap<T, Compare>::siftdown() {
	size_t index = 0;
	size_t child = index * 2 + 1;
	while (child < heap_.size()) {
		if (child < heap_.size() - 1 && comp_(heap_[child + 1], heap_[child])) {
			++child;
		}
		if (comp_(heap_[child], heap_[index])) {
			std::swap(heap_[index], heap_[child]);
			index = child;
			child = index * 2 + 1;
		} else {
			break;
		}
	}
}

#endif // _MIN_HEAP_H_