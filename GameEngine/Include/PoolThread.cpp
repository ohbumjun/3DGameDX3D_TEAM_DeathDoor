#include "PoolThread.h"
#include "ThreadPool.h"

CPoolThread::CPoolThread(std::condition_variable* CV, std::mutex* Mtx, CThreadPool* Owner)
{
	m_CV = CV;
	m_Mtx = Mtx;
	m_OwnerPool = Owner;
}

CPoolThread::~CPoolThread()
{
	m_Thread.join();
}

bool CPoolThread::Init()
{
	m_Thread = std::thread([this]() {this->WorkThread(); });

	return true;
}

void CPoolThread::WorkThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(*m_Mtx);

		// 아래의 조건이 만족하지 않는다면
		// 1) lock 을 unlock 하고
		// 2) 누가 깨우지 않을 때까지 잠들어있게 된다.
		(*m_CV).wait(lock, [this]() {return !this->m_OwnerPool->IsJobEmpty() || m_OwnerPool->IsStopAll(); });

		if (m_OwnerPool->IsStopAll() && m_OwnerPool->IsJobEmpty())
			return;
		
		// std::function<void()> Job = m_Jobs.front();
		std::function<void()> Job = m_OwnerPool->GetQueueJob().front();
		
		// m_Jobs.pop();
		m_OwnerPool->GetQueueJob().pop();
		
		lock.unlock();
		
		Job();

		if (m_EndFunction)
			m_EndFunction();
	}
}
