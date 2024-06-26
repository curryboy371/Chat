#pragma once

// std 함수 포인터 & 람다를 사용하여 callback  job 만듦

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback)
		:_callback(std::move(callback))
	{

	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Excute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

