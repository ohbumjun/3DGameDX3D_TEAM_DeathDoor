
#include "MeshManager.h"
#include "SpriteMesh.h"
#include "StaticMesh.h"
#include "AnimationMesh.h"
#include "NavMesh.h"
#include "../../PathManager.h"
#include "FBXLoader.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	auto iter = m_mapMesh.begin();
	auto iterEnd = m_mapMesh.end();

	for (; iter != iterEnd;)
	{
		iter = m_mapMesh.erase(iter);
		iterEnd = m_mapMesh.end();
	}

	m_mapMesh.empty();
}

bool CMeshManager::Init()
{
	CMesh* SpriteMesh = new CSpriteMesh;

	if (!SpriteMesh->Init())
	{
		SpriteMesh->Release();
		return false;
	}

	SpriteMesh->SetName("SpriteMesh");
	m_mapMesh.insert(std::make_pair("SpriteMesh", SpriteMesh));

	CMesh* FrameRectMesh = new CStaticMesh;

	Vector3	FrameRectPos[5] =
	{
		Vector3(0.f, 1.f, 0.f),
		Vector3(1.f, 1.f, 0.f),
		Vector3(1.f, 0.f, 0.f),
		Vector3(0.f, 0.f, 0.f),
		Vector3(0.f, 1.f, 0.f)
	};

	FrameRectMesh->SetName("FrameRect");
	FrameRectMesh->CreateMesh(FrameRectPos, sizeof(Vector3), 5, D3D11_USAGE_IMMUTABLE, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_mapMesh.insert(std::make_pair("FrameRect", FrameRectMesh));



	CMesh* Box2DMesh = new CStaticMesh;

	Vector3	Box2DPos[5] =
	{
		Vector3(-0.5f, 0.5f, 0.f),
		Vector3(0.5f, 0.5f, 0.f),
		Vector3(0.5f, -0.5f, 0.f),
		Vector3(-0.5f, -0.5f, 0.f),
		Vector3(-0.5f, 0.5f, 0.f)
	};

	Box2DMesh->SetName("Box2D");
	Box2DMesh->CreateMesh(Box2DPos, sizeof(Vector3), 5, D3D11_USAGE_IMMUTABLE, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_mapMesh.insert(std::make_pair("Box2D", Box2DMesh));



	CMesh* CircleMesh = new CStaticMesh;

	Vector3	CirclrPos[31];

	CirclrPos[0] = Vector3(0.5f, 0.f, 0.f);

	for (int i = 1; i < 31; ++i)
	{
		CirclrPos[i].x = cosf(DegreeToRadian(12.f * i)) * 0.5f;
		CirclrPos[i].y = sinf(DegreeToRadian(12.f * i)) * 0.5f;
	}

	CircleMesh->SetName("Circle");
	CircleMesh->CreateMesh(CirclrPos, sizeof(Vector3), 31, D3D11_USAGE_IMMUTABLE, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_mapMesh.insert(std::make_pair("Circle", CircleMesh));


	CMesh* WidgetMesh = new CStaticMesh;

	VertexUV	WidgetVtx[4] =
	{
		VertexUV(Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f)),
		VertexUV(Vector3(1.f, 1.f, 0.f), Vector2(1.f, 0.f)),
		VertexUV(Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f)),
		VertexUV(Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f))
	};

	int	WidgetIdx[6] = { 0, 1, 3, 0, 3, 2 };

	WidgetMesh->SetName("WidgetMesh");
	WidgetMesh->CreateMesh(WidgetVtx, sizeof(VertexUV), 4, D3D11_USAGE_IMMUTABLE,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		WidgetIdx, 4, 6, D3D11_USAGE_IMMUTABLE,
		DXGI_FORMAT_R32_UINT);

	m_mapMesh.insert(std::make_pair("WidgetMesh", WidgetMesh));

	CMesh* ParticlePointMesh = new CStaticMesh;

	Vector3	Point;

	ParticlePointMesh->SetName("ParticlePointMesh");
	ParticlePointMesh->CreateMesh(&Point, sizeof(Vector3), 1, D3D11_USAGE_IMMUTABLE,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	m_mapMesh.insert(std::make_pair("ParticlePointMesh", ParticlePointMesh));


	std::vector<Vertex3D>	vecSphere;
	std::vector<Vector3>	vecSpherePos;
	std::vector<int>		vecSphereIndex;

	CreateSphere(vecSphere, vecSphereIndex, 1.f, 10);

	size_t	SphereSize = vecSphere.size();
	vecSpherePos.resize(SphereSize);

	for (size_t i = 0; i < SphereSize; ++i)
	{
		vecSpherePos[i] = vecSphere[i].Pos;
	}

	CreateMesh(Mesh_Type ::Static, "SpherePos",
		&vecSpherePos[0], sizeof(Vector3), (int)SphereSize,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&vecSphereIndex[0], sizeof(int), vecSphereIndex.size(),
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);


	std::vector<Vertex3D>	vecColliderSphere;
	std::vector<Vector3>	vecColliderSpherePos;
	std::vector<int>		vecColliderSphereIndex;

	CreateSphere(vecColliderSphere, vecColliderSphereIndex, 1.f, 2);

	size_t	ColliderSphereSize = vecColliderSphere.size();
	vecColliderSpherePos.resize(ColliderSphereSize);

	for (size_t i = 0; i < ColliderSphereSize; ++i)
	{
		vecColliderSpherePos[i] = vecColliderSphere[i].Pos;
	}

	CreateMesh(Mesh_Type::Static, "SphereLinePos",
		&vecColliderSpherePos[0], sizeof(Vector3), ColliderSphereSize,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		&vecColliderSphereIndex[0], sizeof(int), (int)vecColliderSphereIndex.size(),
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

	Vector3	CubePos[8] =
	{
		Vector3(-0.5f, 0.5f, -0.5f),
		Vector3(0.5f, 0.5f, -0.5f),
		Vector3(-0.5f, -0.5f, -0.5f),
		Vector3(0.5f, -0.5f, -0.5f),
		Vector3(-0.5f, 0.5f, 0.5f),
		Vector3(0.5f, 0.5f, 0.5f),
		Vector3(-0.5f, -0.5f, 0.5f),
		Vector3(0.5f, -0.5f, 0.5f)
	};

	int	CubeIdx[36] =
	{
		0, 1, 3, 0, 3, 2,	// Near
		1, 5, 7, 1, 7, 3,	// Right
		5, 4, 6, 5, 6, 7,	// Far
		4, 0, 2, 4, 2, 6,	// Left
		4, 5, 1, 4, 1, 0,	// Top
		2, 3, 7, 2, 7, 6	// Bottom
	};

	Vertex3D CubeVertex[8];

	for (int i = 0; i < 8; ++i)
		CubeVertex[i].Pos = CubePos[i];


	CubeVertex[0].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[1].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[2].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[3].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[4].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[5].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[6].Normal = Vector3(1.f, 1.f, 1.f);
	CubeVertex[7].Normal = Vector3(1.f, 1.f, 1.f);
	

	CreateMesh(Mesh_Type::Static, "CubePos",
		&CubePos[0], sizeof(Vector3), 8,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&CubeIdx[0], sizeof(int), 36,
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

	int	CubeLineIdx[24] =
	{
		0, 1, 1, 3, 2, 3, 0, 2, 4, 5, 5, 7, 7, 6, 4, 6, 0, 4, 1, 5, 2, 6, 3, 7
	};

	CreateMesh(Mesh_Type::Static, "CubeLinePos",
		&CubeVertex[0], sizeof(Vertex3D), 8,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		&CubeLineIdx[0], sizeof(int), 24,
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);


	Vertex3D HalfLineVertex[2];

	Vector3	HalfLinePos[2] =
	{
		Vector3(0.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f)
	};

	HalfLineVertex[0].Pos = HalfLinePos[0];
	HalfLineVertex[1].Pos = HalfLinePos[1];

	int HalfLineIdx[2] = { 0, 1 };

	CreateMesh(Mesh_Type::Static, "HalfLinePos",
		&HalfLineVertex[0], sizeof(Vertex3D), 2,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		&HalfLineIdx[0], sizeof(int), 2,
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

	Vertex3D DefaultPlaneVertcies[4];
	
	DefaultPlaneVertcies[0].Pos = Vector3(-0.5f, 0.f, 0.5f);
	DefaultPlaneVertcies[1].Pos = Vector3(0.5f, 0.f, 0.5f);
	DefaultPlaneVertcies[2].Pos = Vector3(0.5f, 0.f, -0.5f);
	DefaultPlaneVertcies[3].Pos = Vector3(-0.5f, 0.f, -0.5f);
	DefaultPlaneVertcies[0].UV = Vector2(0.f, 0.f);
	DefaultPlaneVertcies[1].UV = Vector2(1.f, 0.f);
	DefaultPlaneVertcies[2].UV = Vector2(1.f, 1.f);
	DefaultPlaneVertcies[3].UV = Vector2(0.f, 1.f);
	DefaultPlaneVertcies[0].Normal = Vector3(0.f, 1.f, 0.f);
	DefaultPlaneVertcies[1].Normal = Vector3(0.f, 1.f, 0.f);
	DefaultPlaneVertcies[2].Normal = Vector3(0.f, 1.f, 0.f);
	DefaultPlaneVertcies[3].Normal = Vector3(0.f, 1.f, 0.f);
	DefaultPlaneVertcies[0].Tangent = Vector3(1.f, 0.f, 0.f);
	DefaultPlaneVertcies[1].Tangent = Vector3(1.f, 0.f, 0.f);
	DefaultPlaneVertcies[2].Tangent = Vector3(1.f, 0.f, 0.f);
	DefaultPlaneVertcies[3].Tangent = Vector3(1.f, 0.f, 0.f);
	DefaultPlaneVertcies[0].Binormal = Vector3(0.f, 0.f, 1.f);
	DefaultPlaneVertcies[1].Binormal = Vector3(0.f, 0.f, 1.f);
	DefaultPlaneVertcies[2].Binormal= Vector3(0.f, 0.f, 1.f);
	DefaultPlaneVertcies[3].Binormal = Vector3(0.f, 0.f, 1.f);

	int DefaultPlaneIndices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	CreateMesh(Mesh_Type::Static, "DefaultPlane",
		&DefaultPlaneVertcies[0], sizeof(Vertex3D), 4,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&DefaultPlaneIndices[0], sizeof(int), 6,
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);


	CMesh* PlaneMesh = new CStaticMesh;

	std::vector<Vertex3D> vecPlaneVertex;
	vecPlaneVertex.resize(4);

	Vector3	PlaneMeshPos[4] =
	{
		Vector3(-0.5f, 0.f, 0.f),
		Vector3(0.5f, 0.f, 0.f),
		Vector3(0.5f, 0.f, -1.f),
		Vector3(-0.5f, 0.f, -1.f)
	};
	
	for (size_t i = 0; i < 4; ++i)
		vecPlaneVertex[i].Pos = PlaneMeshPos[i];

	vecPlaneVertex[0].UV = Vector2(0.f, 1.f);
	vecPlaneVertex[1].UV = Vector2(0.f, 0.f);
	vecPlaneVertex[2].UV = Vector2(1.f, 0.f);
	vecPlaneVertex[3].UV = Vector2(1.f, 1.f);

	vecPlaneVertex[0].Normal = Vector3(0.f, 1.f, 0.f);
	vecPlaneVertex[1].Normal = Vector3(0.f, 1.f, 0.f);
	vecPlaneVertex[2].Normal = Vector3(0.f, 1.f, 0.f);
	vecPlaneVertex[3].Normal = Vector3(0.f, 1.f, 0.f);

	int PlaneMeshIndex[6] = { 0, 1, 2, 0, 2, 3 };

	PlaneMesh->SetName("PlaneMesh");

	CreateMesh(Mesh_Type::Static, "PlaneMesh",
		&vecPlaneVertex[0], sizeof(Vertex3D), 4,
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&PlaneMeshIndex[0], sizeof(int), 6,
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

	m_mapMesh.insert(std::make_pair("PlaneMesh", PlaneMesh));

	return true;
}

bool CMeshManager::CreateMesh(Mesh_Type Type, const std::string& Name,
	void* VtxData, int Size, int Count,
	D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive, 
	void* IdxData, int IdxSize, int IdxCount, 
	D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt, CScene* Scene)
{
	CMesh* Mesh = FindMesh(Name);

	if (Mesh)
		return true;

	switch (Type)
	{
	case Mesh_Type::Sprite:
		Mesh = new CSpriteMesh;
		break;
	case Mesh_Type::Static:
		Mesh = new CStaticMesh;
		break;
	case Mesh_Type::Animation:
		Mesh = new CAnimationMesh;
		break;
	}

	Mesh->SetName(Name);
	Mesh->SetScene(Scene);

	if (!Mesh->CreateMesh(VtxData, Size, Count, Usage,
		Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt))
	{
		SAFE_RELEASE(Mesh);
		return false;
	}

	m_mapMesh.insert(std::make_pair(Name, Mesh));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CMeshManager::CreateNavMesh(CNavMesh* NavMesh, const std::string& Name,
	void* VtxData, int Size, int Count, D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive,
	void* IdxData, int IdxSize, int IdxCount, D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt, CScene* Scene)
{
	CMesh* Mesh = FindMesh(Name);

	if (Mesh)
		return true;

	NavMesh->SetName(Name);
	NavMesh->SetScene(Scene);

	if (!NavMesh->CreateMesh(VtxData, Size, Count, Usage,
		Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt))
	{
		SAFE_RELEASE(NavMesh);
		return false;
	}

	m_mapMesh.insert(std::make_pair(Name, NavMesh));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CMeshManager::LoadMesh(Mesh_Type Type, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName, class CScene* Scene)
{
	TCHAR	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		lstrcpy(FullPath, Info->Path);

	lstrcat(FullPath, FileName);

	return LoadMeshFullPath(Type, Name, FullPath, Scene);
}

bool CMeshManager::LoadMesh(std::string& OutName, Mesh_Type Type, const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	TCHAR	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		lstrcpy(FullPath, Info->Path);

	lstrcat(FullPath, FileName);

	return LoadMeshFullPath(OutName, Type, FullPath, Scene);
}

bool CMeshManager::LoadMeshFullPath(Mesh_Type Type, 
	const std::string& Name, const TCHAR* FullPath, class CScene* Scene)
{
	char	FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

	int	ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

	strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

	return LoadMeshFullPathMultibyte(Type, Name, FullPathMultibyte, Scene);
}

bool CMeshManager::LoadMeshFullPath(std::string& OutName, Mesh_Type Type, const TCHAR* FullPath, CScene* Scene)
{
	char	FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

	int	ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

	strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

	return LoadMeshFullPathMultibyte(OutName,Type, FullPathMultibyte, Scene);
}

bool CMeshManager::LoadMeshMultibyte(Mesh_Type Type,
	const std::string& Name, const char* FileName, 
	const std::string& PathName, class CScene* Scene)
{
	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	return LoadMeshFullPathMultibyte(Type, Name, FullPath, Scene);
}

bool CMeshManager::LoadMeshMultibyte(std::string& OutName, Mesh_Type Type, const char* FileName, const std::string& PathName, CScene* Scene)
{
	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	return LoadMeshFullPathMultibyte(OutName, Type, FullPath, Scene);
}

bool CMeshManager::LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name, const char* FullPath, class CScene* Scene)
{
	CMesh* Mesh = FindMesh(Name);

	if (Mesh)
		return true;

	switch (Type)
	{
	case Mesh_Type::Sprite:
		Mesh = new CSpriteMesh;
		break;
	case Mesh_Type::Static:
		Mesh = new CStaticMesh;
		break;
	case Mesh_Type::Animation:
		Mesh = new CAnimationMesh;
		break;
	case Mesh_Type::Nav:
		Mesh = new CNavMesh;
		break;
	}

	Mesh->SetName(Name);
	Mesh->SetScene(Scene);
	Mesh->SetFullPath(FullPath);

	if (!Mesh->LoadMeshFullPathMultibyte(FullPath))
	{
		SAFE_RELEASE(Mesh);
		return false;
	}

	m_mapMesh.insert(std::make_pair(Name, Mesh));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CMeshManager::LoadMeshFullPathMultibyte(std::string& OutName, Mesh_Type Type, const char* FullPath, CScene* Scene)
{
	CMesh* Mesh = nullptr;

	switch (Type)
	{
	case Mesh_Type::Static:
		Mesh = new CStaticMesh;
		break;
	case Mesh_Type::Animation:
		Mesh = new CAnimationMesh;
		break;
	}

	Mesh->SetScene(Scene);

	if (!Mesh->LoadMeshFullPathMultibyte(OutName, FullPath))
	{
		SAFE_RELEASE(Mesh);
		return false;
	}

	if (FindMesh(OutName))
	{
		SAFE_RELEASE(Mesh);
		return true;
	}

	m_mapMesh.insert(std::make_pair(OutName, Mesh));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

bool CMeshManager::SetMeshSkeleton(const std::string& Name, CSkeleton* Skeleton)
{
	CAnimationMesh* Mesh = (CAnimationMesh*)FindMesh(Name);

	if (!Mesh)
		return false;

	else if (!Mesh->CheckType<CAnimationMesh>())
		return false;

	Mesh->SetSkeleton(Skeleton);

	return true;
}

CMesh* CMeshManager::FindMesh(const std::string& Name)
{
	auto	iter = m_mapMesh.find(Name);

	if (iter == m_mapMesh.end())
		return nullptr;

	return iter->second;
}

bool CMeshManager::ConvertFBXLocalFormatFullPathMultiByte(const char* FullPath)
{
	CAnimationMesh* Mesh = new CAnimationMesh;

	Mesh->SetName("DummyMeshForConvert");

	if (!Mesh->ConvertFBXLocalFileMultiByte(FullPath))
	{
		SAFE_RELEASE(Mesh);
		return false;
	}

	// Mesh 변환 이후 Release
	SAFE_RELEASE(Mesh);

	return true;
}

void CMeshManager::ReleaseMesh(const std::string& Name)
{
	auto	iter = m_mapMesh.find(Name);

	if (iter != m_mapMesh.end())
	{
		if (iter->second->GetRefCount() == 1)
		{
			m_mapMesh.erase(iter);

			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
		}
	}
}

bool CMeshManager::CreateSphere(std::vector<Vertex3D>& vecVertex, std::vector<int>& vecIndex, float Radius,
	unsigned int SubDivision)
{
	// Put a cap on the number of subdivisions.
	SubDivision = min(SubDivision, 5);

	// Approximate a sphere by tessellating an icosahedron.
	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	vecVertex.resize(12);
	vecIndex.resize(60);

	for (UINT i = 0; i < 12; ++i)
		vecVertex[i].Pos = pos[i];

	for (UINT i = 0; i < 60; ++i)
		vecIndex[i] = k[i];

	for (UINT i = 0; i < SubDivision; ++i)
		Subdivide(vecVertex, vecIndex);

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < vecVertex.size(); ++i)
	{
		// Project onto unit sphere.
		Vector3	vN = vecVertex[i].Pos;
		vN.Normalize();

		// Project onto sphere.
		Vector3 p = vN * Radius;

		vecVertex[i].Pos = p;
		// Normal이 있을 경우 따로 저장한다.

		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			vecVertex[i].Pos.x,
			vecVertex[i].Pos.z);

		float phi = acosf(vecVertex[i].Pos.y / Radius);

		vecVertex[i].UV.x = theta / XM_2PI;
		vecVertex[i].UV.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		vecVertex[i].Tangent.x = -Radius * sinf(phi) * sinf(theta);
		vecVertex[i].Tangent.y = 0.0f;
		vecVertex[i].Tangent.z = Radius * sinf(phi) * cosf(theta);

		vecVertex[i].Binormal = vecVertex[i].Normal.Cross(vecVertex[i].Tangent);
		vecVertex[i].Binormal.Normalize();
	}

	return true;
}

void CMeshManager::Subdivide(std::vector<Vertex3D>& vecVertices, std::vector<int>& vecIndices)
{
	// Save a copy of the input geometry.
	std::vector<Vertex3D>	vecCopyVertex = vecVertices;
	std::vector<int>	vecCopyIndex = vecIndices;


	vecVertices.resize(0);
	vecIndices.resize(0);

	//       v1
	//       *
	//      / \
					//     /   \
	//  m0*-----*m1
//   / \   / \
	//  /   \ /   \
	// *-----*-----*
// v0    m2     v2

	UINT numTris = vecCopyIndex.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex3D v0 = vecCopyVertex[vecCopyIndex[i * 3 + 0]];
		Vertex3D v1 = vecCopyVertex[vecCopyIndex[i * 3 + 1]];
		Vertex3D v2 = vecCopyVertex[vecCopyIndex[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex3D m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.Pos = Vector3(
			0.5f * (v0.Pos.x + v1.Pos.x),
			0.5f * (v0.Pos.y + v1.Pos.y),
			0.5f * (v0.Pos.z + v1.Pos.z));

		m1.Pos = Vector3(
			0.5f * (v1.Pos.x + v2.Pos.x),
			0.5f * (v1.Pos.y + v2.Pos.y),
			0.5f * (v1.Pos.z + v2.Pos.z));

		m2.Pos = Vector3(
			0.5f * (v0.Pos.x + v2.Pos.x),
			0.5f * (v0.Pos.y + v2.Pos.y),
			0.5f * (v0.Pos.z + v2.Pos.z));

		//
		// Add new geometry.
		//

		vecVertices.push_back(v0); // 0
		vecVertices.push_back(v1); // 1
		vecVertices.push_back(v2); // 2
		vecVertices.push_back(m0); // 3
		vecVertices.push_back(m1); // 4
		vecVertices.push_back(m2); // 5

		vecIndices.push_back(i * 6 + 0);
		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 5);

		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 4);
		vecIndices.push_back(i * 6 + 5);

		vecIndices.push_back(i * 6 + 5);
		vecIndices.push_back(i * 6 + 4);
		vecIndices.push_back(i * 6 + 2);

		vecIndices.push_back(i * 6 + 3);
		vecIndices.push_back(i * 6 + 1);
		vecIndices.push_back(i * 6 + 4);
	}
}

float CMeshManager::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + PI; // in [0, 2*pi).

	return theta;
}

void CMeshManager::LoadAniFile()
{
	CFBXLoader Loader;
	Loader.LoadAniFile();
}
