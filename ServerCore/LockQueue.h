#pragma once

#include "ThreadManager.h"

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		MutexGuard LockGuard(_mtx);
		_items.push(item);
	}

	T Pop()
	{
		MutexGuard LockGuard(_mtx);

		if (_items.empty())
		{
			return T();
		}

		T ret = _items.front();
		_items.pop();
		return ret;
	}

	void PopAll(OUT std::vector<T>& items)
	{
		MutexGuard LockGuard(_mtx);

		while(T item = Pop())
		{
			items.push_back(item);
		}
	}
	void Clear();


private:
	std::recursive_mutex _mtx;
	std::queue<T> _items;
	
};

template<typename T>
inline void LockQueue<T>::Clear()
{
}
