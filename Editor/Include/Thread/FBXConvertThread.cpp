#include "FBXConvertThread.h"
#include "Resource/Mesh/AnimationMesh.h"
#include "Resource/ResourceManager.h"

CFBXConvertThread::CFBXConvertThread()
{
	SetLoop(true);

	m_ExitEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

CFBXConvertThread::~CFBXConvertThread()
{
	m_Loop = false;

	WaitForSingleObject(m_ExitEvent, INFINITE);

	CloseHandle(m_ExitEvent);
}

bool CFBXConvertThread::Init()
{
	if (!CThread::Init())
	{
		assert(false);
		return false;
	}

	return true;
}

void CFBXConvertThread::Run()
{
	while (m_Loop)
	{
		if (!m_WorkQueue.empty())
		{
			// FBX -> msh, bne, sqc파일로 변환한다.
			bool result = CResourceManager::GetInst()->ConvertFBXLocalFormatFullPathMultiByte(m_WorkQueue.front().FBXFullPath.c_str());

			if (!result)
			{
				// 실패한경우 콜백
				if (m_FailCallBack)
				{
					m_StartSize = 0;
					m_FailCallBack(m_WorkQueue.front().FBXFullPath);
				}
				m_WorkQueue.empty();
				continue;
			}

			LoadingMessage msg;
			msg.Percent = 1.f - (float)((float)(m_WorkQueue.size() - 1) / (float)m_StartSize);
			msg.Complete = m_WorkQueue.size() == 1 ? true : false;
			msg.Message = m_WorkQueue.front().FBXFullPath;

			m_WorkQueue.pop();

			if (m_LoadingCallBack)
			{
				if (msg.Complete)
				{
					m_StartSize = 0;
				}

				m_LoadingCallBack(msg);
			}
		}
	}

	SetEvent(m_ExitEvent);
}

void CFBXConvertThread::AddWork(const std::string& FBXFullPath)
{
	FBXConvertWorkData data;
	data.FBXFullPath = FBXFullPath;
	m_WorkQueue.push(data);

	++m_StartSize;
}
