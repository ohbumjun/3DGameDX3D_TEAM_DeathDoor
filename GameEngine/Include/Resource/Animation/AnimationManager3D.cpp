
#include "AnimationManager3D.h"
#include "../../PathManager.h"

CAnimationManager3D::CAnimationManager3D()
{
}

CAnimationManager3D::~CAnimationManager3D()
{
	// Map Sequence
	auto iter = m_mapSequence.begin();
	auto iterEnd = m_mapSequence.end();

	for (; iter != iterEnd;)
	{
		iter = m_mapSequence.erase(iter);
		iterEnd = m_mapSequence.end();
	}

	m_mapSequence.clear();

	// MapSkeleton
	{
		auto iter = m_mapSkeleton.begin();
		auto iterEnd = m_mapSkeleton.end();

		for (; iter != iterEnd;)
		{
			iter = m_mapSkeleton.erase(iter);
			iterEnd = m_mapSkeleton.end();
		}
		m_mapSkeleton.clear();
	}
}

bool CAnimationManager3D::Init()
{
	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(const std::string& Name, bool Loop, 
	_tagFbxAnimationClip* Clip, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Loop, Clip))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadSequenceConvertFBX(const std::string& Name, bool bLoop, _tagFbxAnimationClip* pClip)
{
	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->SetName(Name);

	if (!Sequence->CreateSequenceConvertFBX(bLoop, pClip))
	{
		SAFE_DELETE(Sequence);
		return false;
	}
	
	m_mapSequence.insert(std::make_pair(Name, Sequence));

	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(const std::string& Name, bool Loop, 
	int StartFrame, int EndFrame, float PlayTime,
	const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Name, Loop, StartFrame, EndFrame, PlayTime, vecFrame))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadAnimationSequence(bool Loop, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(Loop, Name, FileName, PathName))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceFullPath(bool Loop, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequence(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name,
	const char* FullPath, class CScene* Scene)
{
	if (FindAnimationSequence(Name))
	{
		return true;
	}

	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Scene = Scene;

	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Sequence->m_Loop = Loop;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CAnimationManager3D::LoadAnimationSequenceFullPathMultibyteSetOriginFileName(bool Loop, std::string& Name, 
	const char* FullPath)
{
	CAnimationSequence* Sequence = new CAnimationSequence;

	Sequence->m_Loop = Loop;

	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}

	Name = Sequence->GetName();

	// 이미 존재한다.
	if (FindAnimationSequence(Name))
	{
		SAFE_DELETE(Sequence);
		return true;
	}

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

CAnimationSequence* CAnimationManager3D::FindAnimationSequence(const std::string& Name)
{
	auto	iter = m_mapSequence.find(Name);

	if (iter == m_mapSequence.end())
		return nullptr;

	return iter->second;
}

void CAnimationManager3D::ReleaseSequence(const std::string& Name) 
{
	auto	iter = m_mapSequence.find(Name);

	if (iter != m_mapSequence.end())
	{
		if (iter->second->GetRefCount() == 1)
		{
			m_mapSequence.erase(iter);
			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
		}
	}
}

void CAnimationManager3D::ReleaseAnimationSequence3D(CAnimationSequence* ExistingSequence)
{
	auto iter = m_mapSequence.begin();
	auto iterEnd = m_mapSequence.end();

	for (; iter != iterEnd; ++iter)
	{
		if (iter->second == ExistingSequence)
		{
			if (iter->second->GetRefCount() == 1)
			{
				if (m_ChangeCallBack)
				{
					m_ChangeCallBack();
				}
				m_mapSequence.erase(iter);
			}
			break;
		}
	}
}

CAnimationSequence* CAnimationManager3D::CreateBasicAnimationSequence(const std::string& Name)
{
	CAnimationSequence* Sequence = FindAnimationSequence(Name);

	if (Sequence)
		return Sequence;

	Sequence = new CAnimationSequence;

	// Sequence->m_Scene = Scene;

	/*
	if (!Sequence->CreateSequenceMultibyte(FullPath))
	{
		SAFE_DELETE(Sequence);
		return false;
	}
	*/
	

	Sequence->m_Loop = true;
	Sequence->SetName(Name);

	m_mapSequence.insert(std::make_pair(Name, Sequence));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return Sequence;
}

void CAnimationManager3D::DeleteSequence(const std::string& Name)
{
	auto iter = m_mapSequence.begin();
	auto iterEnd = m_mapSequence.end();
	
	for (; iter != iterEnd; ++iter)
	{
		if (iter->second->GetName() == Name)
		{
			m_mapSequence.erase(iter);

			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
			break;
		}
	}
}

void CAnimationManager3D::DeleteSequence(const CAnimationSequence* const Sequence)
{
	auto iter = m_mapSequence.begin();
	auto iterEnd = m_mapSequence.end();

	for (; iter != iterEnd; ++iter)
	{
		if (iter->second == Sequence)
		{
			m_mapSequence.erase(iter);

			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
			break;
		}
	}
}

bool CAnimationManager3D::EditSequenceClip(CAnimationSequence* ExistingSequence, 
	const std::string& NewName, int StartFrame, int EndFrame, 
	const char* SaveFullPathMultibyte)
{
	// 새로운 Sequence 를 만들어내서
	// 하드디스크에 저장 이후,
	// 메모리 상에서는 해제한다.
	CAnimationSequence* NewSequence = new CAnimationSequence;

	NewSequence->m_Loop = true;

	if (!NewSequence->CreateNewSequenceFromExistingSequence(ExistingSequence, 
		StartFrame, EndFrame, NewName))
	{
		SAFE_DELETE(NewSequence);
		return false;
	}

	strcpy_s(NewSequence->m_FullPath, SaveFullPathMultibyte);
	char						m_FileName[MAX_PATH];

	char	strAnimName[MAX_PATH] = {};
	_splitpath_s(NewSequence->m_FullPath, 0, 0, 0, 0, strAnimName, MAX_PATH, 0, 0);

	strcpy_s(NewSequence->m_FileName, strAnimName);
	strcat_s(NewSequence->m_FileName, ".sqc");

	NewSequence->SaveFullPathMultibyte(NewSequence->m_FullPath);

	SAFE_DELETE(NewSequence);

	return true;
}

bool CAnimationManager3D::LoadSkeleton(const std::string& Name, 
	const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	TCHAR	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		lstrcpy(FullPath, Info->Path);

	lstrcat(FullPath, FileName);

	return LoadSkeletonFullPath(Name, FullPath, Scene);
}

bool CAnimationManager3D::LoadSkeletonFullPath(const std::string& Name,
	const TCHAR* FullPath, CScene* Scene)
{
	char	FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

	int	ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

	strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

	return LoadSkeletonFullPathMultibyte(Name, FullPathMultibyte, Scene);
}

bool CAnimationManager3D::LoadSkeletonMultibyte(const std::string& Name, 
	const char* FileName, const std::string& PathName, CScene* Scene)
{
	if (FindSkeleton(Name))
	{
		return true;
	}

	CSkeleton* Skeleton = new CSkeleton;

	Skeleton->m_Scene = Scene;

	if (!Skeleton->LoadSkeleton(Scene, Name, FileName, PathName))
	{
		SAFE_DELETE(Skeleton);
		return false;
	}

	Skeleton->SetName(Name);

	m_mapSkeleton.insert(std::make_pair(Name, Skeleton));

	return true;
}

bool CAnimationManager3D::LoadSkeletonFullPathMultibyte(
	const std::string& Name, const char* FullPath, CScene* Scene)
{
	CSkeleton* Skeleton = FindSkeleton(Name);

	if (Skeleton)
		return true;

	Skeleton = new CSkeleton;

	Skeleton->m_Scene = Scene;

	if (!Skeleton->LoadSkeletonFullPath(Scene, Name, FullPath))
	{
		SAFE_DELETE(Skeleton);
		return false;
	}

	Skeleton->SetName(Name);

	m_mapSkeleton.insert(std::make_pair(Name, Skeleton));

	return true;
}

CSkeleton* CAnimationManager3D::FindSkeleton(const std::string& Name)
{
	auto	iter = m_mapSkeleton.find(Name);

	if (iter == m_mapSkeleton.end())
		return nullptr;

	return iter->second;
}

bool CAnimationManager3D::AddSocket(const std::string& SkeletonName,
	const std::string& BoneName, const std::string& SocketName,
	const Vector3& Offset, const Vector3& OffsetRot)
{
	CSkeleton* Skeleton = FindSkeleton(SkeletonName);

	if (!Skeleton)
		return false;

	Skeleton->AddSocket(BoneName, SocketName, Offset, OffsetRot);

	return true;
}

void CAnimationManager3D::ReleaseSkeleton(const std::string& Name)
{
	auto	iter = m_mapSkeleton.find(Name);

	if (iter != m_mapSkeleton.end())
	{
		if (iter->second->GetRefCount() == 1)
			m_mapSkeleton.erase(iter);
	}
}
