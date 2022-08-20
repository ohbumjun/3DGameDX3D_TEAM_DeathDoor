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

	// 1) �񵿱������� �ش� �����带 ���� ó���� ���� ���� ���̴�. ���� (promise-future)������ Ȱ���Ѵ� -> �̸� �Ϲ� �Լ��� �����ϱ� ���� packaged_task�� Ȱ��
	// 2) std::result_of<F(Args...)>::type �� ���ؼ� �ش� �Լ��� ����Ÿ���� ���� �� �ְ�, ::type �� ���, ���� Ÿ���̹Ƿ�
	//  �ش� ���� Ÿ������ �����Ϸ����� �˷��ֱ� ���ؼ� typename Ű���带 �ۼ��Ѵ�.
	// 3) �Ϻ��� ���� (���� ����) ������ �����ϱ� ���ؼ� Universal_Reference ���·� ���ڸ� �����ϰ�, �Լ� ���� �ȿ��� std::forward �Լ��� ����Ͽ� �����尡 ������ �Լ��� ���ڷ� ����
	template<typename F, typename ...Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);
};

template<typename F, typename ...Args>
inline std::future<typename std::result_of<F(Args...)>::type> CThreadPool::EnqueueJob(F&& f, Args && ...args)
{
	using return_type = std::future<typename std::result_of<F(Args...)>::type>;

	// 1. std::bind(std::forward<F>(f), std::forward<Args>(args)...) �� ���ؼ�, ���ڸ� �޴� �Լ���, ���� Ÿ���� return_type �̰�, ���ڴ� args �� �޴� �Լ� ��ü�� ������.
	// 2. std::packaged_task �� ���ؼ� promise-future ������ �����Ѵ�. ��, �ش� �Լ��� �����ϴ� ���� future ��ü�� set_value ���� ���̴�.
	// 3. shared_ptr �� ����������ν�, ���� WorkThread �Լ��� ���� �ش� Job�� ������ ��, ���������� �������� �ϰ�, ���̻� �ش� packaged_task �� ����ϴ� ��ü�� ������ �ڵ����� �Ҹ�ǰ� �Ѵ�.
	auto Job = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> Job_Future = Job->get_future();

	{
		std::lock_guard<std::mutex> lock(m_Mtx);
		
		m_Jobs.push([Job]() {(*Job)(); });
	}

	// �ڰ� �ִ� Thread �� �ϳ� �����.
	m_CV.notify_one();

	return Job_Future;
}
