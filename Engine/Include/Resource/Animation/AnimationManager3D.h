#pragma once

#include "AnimationSequence.h"
#include "Skeleton.h"

class CAnimationManager3D
{
	friend class CResourceManager;

private:
	CAnimationManager3D();
	~CAnimationManager3D();

private:
	std::unordered_map<std::string, CSharedPtr<CAnimationSequence>>	m_mapSequence;
	std::unordered_map<std::string, CSharedPtr<CSkeleton>>	m_mapSkeleton;
	std::function<void()> m_ChangeCallBack;

public:
	bool Init();
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		struct _tagFbxAnimationClip* Clip, class CScene* Scene = nullptr);
	bool LoadSequenceConvertFBX(const std::string& Name, bool bLoop, struct _tagFbxAnimationClip* pClip);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		int StartFrame, int EndFrame, float PlayTime,
		const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name, const char* FullPath, class CScene* Scene = nullptr);
	// Load한 파일 본래의 Name 을 세팅
	bool LoadAnimationSequenceFullPathMultibyteSetOriginFileName(bool Loop, std::string& Name, const char* FullPath);
	CAnimationSequence* FindAnimationSequence(const std::string& Name);
	bool AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName,
		const Vector3& Offset = Vector3::Zero, const Vector3& OffsetRot = Vector3::Zero);
	void ReleaseSequence(const std::string& Name);
	void ReleaseAnimationSequence3D(class CAnimationSequence* ExistingSequence);
	CAnimationSequence* CreateBasicAnimationSequence(const std::string& Name);
	void DeleteSequence(const std::string& Name);
	void DeleteSequence(const CAnimationSequence* const Sequence);

	// 기존 Animation Sequence 정보로부터 일부를 잘라내서 새로운 Sequence 를 만들어내는 기능
	bool EditSequenceClip(class CAnimationSequence* ExistingSequence, const std::string& NewName, 
		int StartFrame, int EndFrame, const char* SaveFullPathMultibyte);

	// Skeleton
	bool LoadSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPath(const std::string& Name, const TCHAR* FullPath,
		class CScene* Scene = nullptr);
	bool LoadSkeletonMultibyte(const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath,
		class CScene* Scene = nullptr);
	CSkeleton* FindSkeleton(const std::string& Name);
	void ReleaseSkeleton(const std::string& Name);

public:
	template <typename T>
	void AddResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ChangeCallBack = std::bind(Func, Obj);
	}
};

