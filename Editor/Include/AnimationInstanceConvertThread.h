#pragma once
#include "Thread.h"
#include "ThreadQueue.h"
#include "EditorInfo.h"

class CAnimationInstanceConvertThread :
    public CThread
{
public:
	CAnimationInstanceConvertThread();
	virtual ~CAnimationInstanceConvertThread();

public:
	bool Init();
	virtual void Run();

public:
	void AddWork(const std::string& AnimationSequenceFullPath);

public:
	template<typename T>
	void SetLoadingCallBack(T* Obj, void(T::* Func)(const LoadingMessage&))
	{
		m_LoadingCallBack = std::bind(Func, Obj, std::placeholders::_1);
	}

private:
	CThreadQueue<AnimInstanceWorkData> m_WorkQueue;
	std::function<void(const LoadingMessage&)> m_LoadingCallBack;
	std::function<void(const std::string&)> m_FailCallBack;
};

