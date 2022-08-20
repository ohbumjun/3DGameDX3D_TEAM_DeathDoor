#pragma once

#include "../../Ref.h"
#include "../Material/Material.h"

class CMesh :
	public CRef
{
	friend class CMeshManager;

protected:
	CMesh();
	virtual ~CMesh() = 0;

protected:
	class CScene* m_Scene;

public:
	void SetScene(class CScene* Scene)
	{
		m_Scene = Scene;
	}

protected:
	std::vector<MeshContainer*>	m_vecContainer;
	std::vector<MeshSlot*>		m_vecMeshSlot;
	std::vector<CSharedPtr<CMaterial>>	m_vecMaterialSlot;

	Vector3	m_Min;
	Vector3	m_Max;
	Mesh_Type	m_MeshType;

	// Load, Save 편의를 위한 Full Path 정보 세팅
protected:
	char m_FullPath[MAX_PATH];

public :
	void SetFullPath(const char* FullPath)
	{
		strcpy_s(m_FullPath, FullPath);
	}

public:
	const char* GetFullPath() const
	{
		return m_FullPath;
	}

	Mesh_Type GetMeshType() const
	{
		return m_MeshType;
	}

	Vector3 GetMin() const
	{
		return m_Min;
	}

	Vector3 GetMax() const
	{
		return m_Max;
	}

	const std::vector<CSharedPtr<CMaterial>>* GetMaterialSlots()	const
	{
		return &m_vecMaterialSlot;
	}

	size_t GetMaterialSlotCount() const
	{
		return m_vecMaterialSlot.size();
	}

	CMaterial* GetMaterial(int SlotIndex)
	{
		return m_vecMaterialSlot[SlotIndex];
	}

	void AddMaterialSlot(CMaterial* Material)
	{
		m_vecMaterialSlot.push_back(Material);
	}

	void AddMaterialSlot(const std::string& Name);

public:
	virtual bool CreateMesh(void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN);
	bool LoadMesh(const TCHAR* FileName, const std::string& PathName = MESH_PATH);
	bool LoadMesh(std::string& OutName, const TCHAR* FileName, const std::string& PathName = MESH_PATH);
	bool LoadMeshFullPath(const TCHAR* FullPath);
	bool LoadMeshFullPath(std::string& OutName, const TCHAR* FullPath);
	bool LoadMeshMultibyte(const char* FileName, const std::string& PathName = MESH_PATH);
	bool LoadMeshMultibyte(std::string& OutName, const char* FileName, const std::string& PathName = MESH_PATH);
	virtual bool LoadMeshFullPathMultibyte(const char* FullPath);
	virtual bool LoadMeshFullPathMultibyte(std::string& OutName, const char* FullPath);
	virtual bool Init();
	virtual void Render();
	virtual void Render(int SlotNumber);
	virtual void RenderInstancing(int Count);
	virtual void RenderInstancing(int Count, int SlotNumber);
	virtual void RenderInstancing(ID3D11Buffer* InstancingBuffer, unsigned int InstanceSize, int Count);
	virtual void RenderInstancing(ID3D11Buffer* InstancingBuffer, unsigned int InstanceSize, int Count,
		int SlotNumber);

protected:
	bool CreateBuffer(Buffer_Type Type, void* Data, int Size, int Count,
		D3D11_USAGE Usage, ID3D11Buffer** Buffer);


public:
	bool SaveMeshFile(const char* FullPath);
	bool LoadMeshFile(const char* FullPath);
	virtual bool ConvertFBX(class CFBXLoader* Loader, const char* FullPath);
	virtual bool SaveMesh(FILE* File);
	virtual bool LoadMesh(FILE* File);
protected:
	bool CheckLoadedMaterialTexture(class CMaterial* Material);
public:
	// 에디터에서 FBX 변환 툴에서만 사용, 애니메이션 시퀀스 저장하고 나서 바로 Release함
	virtual bool ConvertFBXReleaseSequence(class CFBXLoader* Loader, const char* FullPath);
};

