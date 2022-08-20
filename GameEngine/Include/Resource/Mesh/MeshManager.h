#pragma once

#include "../../GameInfo.h"

class CMeshManager
{
	friend class CResourceManager;

private:
	CMeshManager();
	~CMeshManager();

private:
	std::unordered_map<std::string, CSharedPtr<class CMesh>>	m_mapMesh;
	std::function<void()> m_ChangeCallBack;

public :
	const std::unordered_map<std::string, CSharedPtr<class CMesh>>& GetMapMesh() const
	{
		return m_mapMesh;
	}

public:
	bool Init();
	bool CreateMesh(Mesh_Type Type, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN,
		class CScene* Scene = nullptr);
	bool CreateNavMesh(class CNavMesh* NavMesh, const std::string& Name, void* VtxData,
		int Size, int Count, D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive,
		void* IdxData, int IdxSize, int IdxCount, D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt, class CScene* Scene = nullptr);
	bool LoadMesh(Mesh_Type Type, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMesh(std::string& OutName, Mesh_Type Type,const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(std::string& OutName,Mesh_Type Type,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(std::string& OutName, Mesh_Type Type, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name,
		const char* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(std::string& OutName, Mesh_Type Type,
		const char* FullPath, class CScene* Scene = nullptr);
	bool SetMeshSkeleton(const std::string& Name, class CSkeleton* Skeleton);
	class CMesh* FindMesh(const std::string& Name);
	bool ConvertFBXLocalFormatFullPathMultiByte(const char* FullPath);
	void ReleaseMesh(const std::string& Name);

public:
	template <typename T>
	void AddResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ChangeCallBack = std::bind(Func, Obj);
	}

private:
	bool CreateSphere(std::vector<Vertex3D>& vecVertex, std::vector<int>& vecIndex, float Radius,
		unsigned int SubDivision);
	void Subdivide(std::vector<Vertex3D>& vecVertices, std::vector<int>& vecIndices);
	float AngleFromXY(float x, float y);


public:
	void LoadAniFile();
};

