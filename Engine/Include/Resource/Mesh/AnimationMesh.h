#pragma once
#include "Mesh.h"
class CAnimationMesh :
    public CMesh
{
	friend class CMeshManager;

protected:
	CAnimationMesh();
	virtual ~CAnimationMesh();

private:
	CSharedPtr<class CSkeleton>	m_Skeleton;
	class CStructuredBuffer* m_BoneBuffer;
	int m_InstancingCount;

public:
	class CSkeleton* GetSkeleton()	const
	{
		return m_Skeleton;
	}

	int GetBoneCount() const;

	class CStructuredBuffer* GetBoneBuffer() const
	{
		return m_BoneBuffer;
	}

	void SetBoneShader();
	void ResetBoneShader();

public:
	virtual bool Init();
	void SetSkeleton(class CSkeleton* Skeleton);
	void SetSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH);
	virtual bool LoadMeshFullPathMultibyte(const char* FullPath);
	virtual bool LoadMeshFullPathMultibyte(std::string& OutName, const char* FullPath) override;
	virtual bool ConvertFBXLocalFileMultiByte(const char* FullPath);

public:
	virtual bool ConvertFBX(class CFBXLoader* Loader, const char* FullPath);
	virtual bool SaveMesh(FILE* File);
	virtual bool LoadMesh(FILE* File);

private:
	// 에디터에서 FBX 변환 툴에서만 사용, 애니메이션 시퀀스 저장하고 나서 바로 Release함
	virtual bool ConvertFBXReleaseSequence(class CFBXLoader* Loader, const char* FullPath) override;
};

