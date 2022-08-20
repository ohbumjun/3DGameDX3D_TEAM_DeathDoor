#pragma once

#include "GameInfo.h"

class CPoolThread
{
	friend class CThreadPool;
public :
	CPoolThread(std::condition_variable* CV, std::mutex* Mtx, class CThreadPool* Owner);
	~CPoolThread();
private:
	class CThreadPool* m_OwnerPool;
	std::thread m_Thread;
	std::function<void()> m_EndFunction;

	std::condition_variable *m_CV;
	std::mutex *m_Mtx;
private :
	bool Init();
	void WorkThread();
};

