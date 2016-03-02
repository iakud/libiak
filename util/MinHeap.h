#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <vector>
#include <algorithm>

template <class T>
class MinHeap {
public:
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
};

template <class T>
void MinHeap<T>::insert(const T& node) {
	heap_.push_back(node);
	siftup();
}

template <class T>
T MinHeap<T>::remove() {
	T node = heap_.front();
	heap_.front() = heap_.back();
	heap_.pop_back();
	siftdown();
	return node;
}

template <class T>
void MinHeap<T>::siftup() {
	size_t index = heap_.size() - 1;
	size_t parent = (index - 1) / 2;
	while (index > 0) {
		if (heap_[index] < heap_[parent]) {
			std::swap(heap_[parent], heap_[index]);
			index = parent;
			parent = (index - 1) / 2;
		} else {
			break;
		}
	}
}

template <class T>
void MinHeap<T>::siftdown() {
	size_t index = 0;
	size_t child = index * 2 + 1;
	while (child < heap_.size()) {
		if (child < heap_.size() - 1 && heap_[child + 1] < heap_[child]) {
			++child;
		}
		if (heap_[child] < heap_[index]) {
			std::swap(heap_[index], heap_[child]);
			index = child;
			child = index * 2 + 1;
		} else {
			break;
		}
	}
}

#endif // _MIN_HEAP_H_