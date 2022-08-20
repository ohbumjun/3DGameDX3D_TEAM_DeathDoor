#pragma once

#include "../GameInfo.h"

class CThreadPool
{
private :
	std::size_t m_NumThreads;
	std::vector<std::thread> m_Workers;
	std::queue<std::function<void()>> m_Jobs;
	std::condition_variable m_CV;
	std::mutex m_Mtx;
	bool m_StopAll;
public : 
	CThreadPool(size_t NumThreads);
	~CThreadPool();
public :
	void WorkThread();

	// 1) 비동기적으로 해당 쓰레드를 통해 처리한 값을 얻어올 것이다. 따라서 (promise-future)패턴을 활용한다 -> 이를 일반 함수에 적용하기 위해 packaged_task를 활용
	// 2) std::result_of<F(Args...)>::type 을 통해서 해당 함수의 리턴타입을 얻어올 수 있고, ::type 의 경우, 의존 타입이므로
	//  해당 값이 타입임을 컴파일러에게 알려주기 위해서 typename 키워드를 작성한다.
	// 3) 완벽한 전달 (복사 방지) 패턴을 적용하기 위해서 Universal_Reference 형태로 인자를 전달하고, 함수 본문 안에서 std::forward 함수를 사용하여 쓰레드가 실행할 함수의 인자로 전달
	template<typename F, typename ...Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);
};

template<typename F, typename ...Args>
inline std::future<typename std::result_of<F(Args...)>::type> CThreadPool::EnqueueJob(F&& f, Args && ...args)
{
	using return_type = std::future<typename std::result_of<F(Args...)>::type>;

	// 1. std::bind(std::forward<F>(f), std::forward<Args>(args)...) 을 통해서, 인자를 받는 함수를, 리턴 타입이 return_type 이고, 인자는 args 를 받는 함수 객체를 만들어낸다.
	// 2. std::packaged_task 을 통해서 promise-future 패턴을 적용한다. 즉, 해당 함수가 리턴하는 값을 future 객체에 set_value 해줄 것이다.
	// 3. shared_ptr 로 만들어줌으로써, 차후 WorkThread 함수를 통해 해당 Job을 꺼내올 때, 정상적으로 꺼내오게 하고, 더이상 해당 packaged_task 를 사용하는 객체가 없으면 자동으로 소멸되게 한다.
	auto Job = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> Job_Future = Job->get_future();

	{
		std::lock_guard<std::mutex> lock(m_Mtx);
		
		m_Jobs.push([Job]() {(*Job)(); });
	}

	// 자고 있는 Thread 를 하나 깨운다.
	m_CV.notify_one();

	return Job_Future;
}
