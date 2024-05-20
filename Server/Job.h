#pragma once
#include "ThreadManager.h"





template<int... Remains>
struct seq
{
	int i = 0;
};

template<int N, int... Remains>
struct gen_seq : gen_seq<N - 1, N - 1, Remains...>
{
	int i = 0;
};

template<int... Remains>
struct gen_seq<0, Remains...> : seq<Remains...>
{
	int i = 0;
};

template<typename Ret, typename... Args>
void xapply(Ret(*func)(Args...), std::tuple<Args...>& tup)
{
	// gen_seq : Args 3 > 0,1,2 형태의 args로 변환
	return xapply_helper(func, gen_seq<sizeof...(Args)>(), tup);
};

template<typename F, typename... Args, int...ls>
void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
{
	// 0,1,2 형태로 변환된 args ls로 tupe을 get<0>, get<1>, get<2> 함
	(func)(std::get<ls>(tup)...);
}

template<typename T, typename Ret, typename... Args>
void xapply(T* obj, Ret(T::*func)(Args...), std::tuple<Args...>& tup)
{
	return xapply_helper(obj, func, gen_seq<sizeof...(Args)>(), tup);
};

// 클래스 멤버함수를 콜백
template<typename T, typename F, typename... Args, int...ls>
void xapply_helper(T* obj, F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(obj->*func)(std::get<ls>(tup)...);
}




// interface
class IJob
{
public:
	virtual void Excute() {}
};


// Functor
// normal functor
class NormalFuncJob
{
	//int	(*f)(int, int); // 반환형 (*변수명) (인자1,인자2)
	using FuncType = void(*)(int64, int32); // 함수 포인터
public:
	// 생성자에서 함수를 받고
	NormalFuncJob(FuncType func) : _func(func), _t1(0), _t2(0)
	{

	}
	NormalFuncJob(FuncType func, int64 t1, int32 t2) 
		: _func(func) ,_t1(t1), _t2(t2)
	{

	}

	// - 생성자 인자 사용 호출
	// 오퍼레이터를 통해 callback
	void operator()()
	{
		_func(_t1, _t2);
	}

	// Excute를 통해 callback
	void Excute(int64 t1, int32 t2)
	{
		_func(t1, t2);
	}

private:
	FuncType _func;

	int64 _t1;
	int32 _t2;
};


// variadic template functor
template<typename Ret, typename... Args>
class FuncJob
{
	//int	(*f)(int, int); // 반환형 (*변수명) (인자1,인자2)
	// 함수 포인터의 변수명은 생략 가능함.
	using FuncType = Ret(*)(Args...); // 함수 포인터
public:
	// 생성자에서 함수를 받고
	FuncJob(FuncType func) : _func(func)
	{

	}

	// Excute를 통해 callback
	Ret Excute(Args... args)
	{
		_func(args...);
	}

private:
	FuncType _func;
};

// variadic template functor
template<typename Ret, typename... Args>
class FuncJob2 : public IJob
{
	//int	(*f)(int, int); // 반환형 (*변수명) (인자1,인자2)
	// 함수 포인터의 변수명은 생략 가능함.
	using FuncType = Ret(*)(Args...); // 함수 포인터
public:
	// 생성자에서 함수를 받고
	FuncJob2(FuncType func, Args... args) 
		: _func(func), _tuple(args...)
	{

	}

	// 호출시 인자 호출
	// Excute를 통해 callback 
	virtual void Excute() override
	{
		// C++17부터 사용가능
		//std::apply(_func, _tuple);

		xapply(_func, _tuple);
	}

private:
	FuncType _func;
	std::tuple<Args...> _tuple; // 잡 생성시 인자를 가변적으로 들고 있게 하기위해
};


// variadic template functor
template<typename T, typename Ret, typename... Args>
class MemberJob2 : public IJob
{
	//int	(*f)(int, int); // 반환형 (*변수명) (인자1,인자2)
	// 함수 포인터의 변수명은 생략 가능함.
	using FuncType = Ret(T::*)(Args...); // 함수 포인터
public:
	// 생성자에서 함수를 받고
	MemberJob2(T* obj, FuncType func, Args... args)
		:_obj(obj), _func(func), _tuple(args...)
	{

	}

	// 호출시 인자 호출
	// Excute를 통해 callback 
	virtual void Excute() override
	{
		// C++17부터 사용가능
		//std::apply(_func, _tuple);

		xapply(_obj, _func, _tuple);
	}

private:
	T* _obj;
	FuncType _func;
	std::tuple<Args...> _tuple; // 잡 생성시 인자를 가변적으로 들고 있게 하기위해
};




class HealJob : public IJob
{
public:
	virtual void Excute() override
	{
		// target 
		// add hp

	}


public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};

using JobRef = std::shared_ptr<IJob>;

class JobQueue
{
public:
	void Push(JobRef job)
	{
		MutexGuard LockGuard(_mtx);

		_jobs.push(job);
	}

	JobRef Pop()
	{
		MutexGuard LockGuard(_mtx);

		if (_jobs.empty())
		{
			return nullptr;
		}

		JobRef ret = _jobs.front();
		_jobs.pop();
		return ret;
	}

private:
	std::recursive_mutex _mtx;
	std::queue<JobRef> _jobs;

};
