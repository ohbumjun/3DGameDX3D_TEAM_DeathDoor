#pragma once

#include "Thread.h"
#include "ThreadQueue.h"
#include "../EditorInfo.h"

class CFBXConvertThread :
    public CThread
{
public:
	CFBXConvertThread();
	virtual ~CFBXConvertThread();

public:
	bool Init();
	virtual void Run();

public:
	void AddWork(const std::string& FBXFullPath);

public:
	template<typename T>
	void SetLoadingCallBack(T* Obj, void(T::* Func)(const LoadingMessage&))
	{
		m_LoadingCallBack = std::bind(Func, Obj, std::placeholders::_1);
	}

	template<typename T>
	void SetCompleteCallBack(T* Obj, void(T::* Func)(const std::string&))
	{
		m_LoadingCallBack = std::bind(Func, Obj, std::placeholders::_1);
	}

private:
	CThreadQueue<FBXConvertWorkData> m_WorkQueue;
	HANDLE m_ExitEvent;
	std::function<void(const LoadingMessage&)> m_LoadingCallBack;
	std::function<void(const std::string&)> m_FailCallBack;
	
	int m_StartSize;

};

