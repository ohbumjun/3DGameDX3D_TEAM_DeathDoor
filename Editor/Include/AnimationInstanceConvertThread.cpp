#include "AnimationInstanceConvertThread.h"
#include "Resource/Mesh/AnimationMesh.h"
#include "Resource/ResourceManager.h"
#include "EditorUtil.h"
#include "EditorManager.h"
#include "Window/AnimationEditor.h"

CAnimationInstanceConvertThread::CAnimationInstanceConvertThread() 
{
	SetLoop(true);
}

CAnimationInstanceConvertThread::~CAnimationInstanceConvertThread()
{
	m_Loop = false;
}

bool CAnimationInstanceConvertThread::Init()
{
	if (!CThread::Init())
	{
		assert(false);
		return false;
	}

	return true;
}

void CAnimationInstanceConvertThread::Run()
{
	while (m_Loop)
	{ 
		int InitSize = m_WorkQueue.size();
		
		while (!m_WorkQueue.empty())
		{
			AnimInstanceWorkData Data = m_WorkQueue.front();
		
			// CEditorManager::GetInst()->GetAnimationEditor()->AddSequenceToDummyAnimationInstance(Data.AnimSequenceFullPath.c_str());
		
			LoadingMessage msg;
			msg.Percent = 1.f - (float)((float)(m_WorkQueue.size() - 1) / (float)InitSize);
			msg.Complete = m_WorkQueue.size() == 1 ? true : false;
			msg.Message = m_WorkQueue.front().AnimSequenceFileName;
		
			if (m_LoadingCallBack)
			{
				m_LoadingCallBack(msg);
			}
		
			m_WorkQueue.pop();
		}
	}
}

void CAnimationInstanceConvertThread::AddWork(const std::string& AnimationSequenceFullPath)
{
	AnimInstanceWorkData data;
	data.AnimSequenceFullPath = AnimationSequenceFullPath;

	char CopyAnimSequenceFullPath[MAX_PATH] = {};
	strcpy_s(CopyAnimSequenceFullPath, AnimationSequenceFullPath.c_str());

	char SqcFileName[MAX_PATH] = {};
	char SqcExt[_MAX_EXT] = {};

	_splitpath_s(CopyAnimSequenceFullPath, nullptr, 0, nullptr, 0, SqcFileName, MAX_PATH, SqcExt, _MAX_EXT);

	data.AnimSequenceFullPath = CopyAnimSequenceFullPath;
	data.AnimSequenceFileName = SqcFileName;
	m_WorkQueue.push(data);
}
