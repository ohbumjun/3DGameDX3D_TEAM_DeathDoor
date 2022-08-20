#include "ThreadPool.h"

CThreadPool::CThreadPool(size_t NumThreads) :
	m_NumThreads(NumThreads),
	m_StopAll(false)
{
	for (size_t i = 0; i < m_NumThreads; ++i)
	{
		// m_Workers[i]->m_Thread = std::thread([this]() {this->WorkThread(); });
		m_Workers.push_back(new CPoolThread(&m_CV, &m_Mtx, this));
		m_Workers[i]->Init();
	}
}

CThreadPool::~CThreadPool()
{
	m_StopAll = true;

	m_CV.notify_all();

	for (size_t i = 0; i < m_NumThreads; ++i)
	{
		// m_Workers[i]->m_Thread.join();
		SAFE_DELETE(m_Workers[i]);
	}
}

bool CThreadPool::Init()
{


	return true;
}

// void CThreadPool::WorkThread()
// {
// 	while (true)
// 	{
// 		std::unique_lock<std::mutex> lock(m_Mtx);
// 		
// 		// �Ʒ��� ������ �������� �ʴ´ٸ�
// 		// 1) lock �� unlock �ϰ�
// 		// 2) ���� ������ ���� ������ �����ְ� �ȴ�.
// 		m_CV.wait(lock, [this]() {return !this->m_Jobs.empty() || m_StopAll; });
// 
// 		if (m_StopAll && m_Jobs.empty())
// 			return;
// 
// 		std::function<void()> Job = m_Jobs.front();
// 
// 		m_Jobs.pop();
// 
// 		lock.unlock();
// 
// 		Job();
// 	}
// }
