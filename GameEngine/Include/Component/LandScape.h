#pragma once
#include "SceneComponent.h"

struct Polygon
{
	Vector3 v1;
	Vector3 v2;
	Vector3 v3;
};

class CLandScape :
	public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CLandScape();
	CLandScape(const CLandScape& com);
	virtual ~CLandScape();

protected:
	CSharedPtr<class CStaticMesh>	m_Mesh;
	int		m_CountX;
	int		m_CountZ;
	std::vector<Vertex3D>		m_vecVtx;
	std::vector<Vector3>		m_vecPos;
	std::vector<unsigned int>	m_vecIndex;
	std::vector<Vector3>		m_vecFaceNormal;
	std::vector<CSharedPtr<class CMaterial>> m_vecMaterialSlot;
	class CLandScapeConstantBuffer* m_CBuffer;
	Vector3 m_Min;
	Vector3 m_Max;

	std::string m_MeshName;
	float m_YFactor;
	float m_PrevYFactor;
	bool m_DebugRender;
	CSharedPtr<class CShader> m_WireFrameShader;
	CSharedPtr<CRenderState> m_WireFrameState;
	bool m_Create;

public:
	const Vector3& GetMin()	const
	{
		return m_Min;
	}

	const Vector3& GetMax()	const
	{
		return m_Max;
	}

	CMaterial* GetMaterial(int Index = 0)    const
	{
		return m_vecMaterialSlot[Index];
	}

	float GetYFactor() const
	{
		return m_YFactor;
	}

	bool IsCreate() const
	{
		return m_Create;
	}

	void SetDebugRender(bool Debug)
	{
		m_DebugRender = Debug;
	}

public:
	void CreateLandScape(const std::string& Name,
		int CountX, int CountZ,
		CTexture* HeightMapTexture,
		float YFactor = 30.f);
	void CreateLandScape(const std::string& Name,
		int CountX, int CountZ,
		const TCHAR* HeightMap = nullptr,
		const std::string& PathName = TEXTURE_PATH,
		float YFactor = 30.f);
	void SetMaterial(class CMaterial* Material, int Index = 0);
	void SetMaterial(const std::string& Name, int Index = 0);
	void AddMaterial(class CMaterial* Material);
	void AddMaterial(const std::string& Name);
	void SetDetailLevel(float Level);
	void SetSplatCount(int Count);

	void SetYFactor(float YFactor);

public:
	float GetHeight(const Vector3& Pos);
	bool CheckInArea(const Vector3& StartPos, const Vector3& EndPos, Vector3& OutPos);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderShadowMap();
	virtual void PostRender();
	virtual CLandScape* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
    virtual bool SaveOnly(FILE* File) override;
    virtual bool LoadOnly(FILE* File) override;

private:
	void ComputeNormal();
	void ComputeTangent();

	void RecreateY();

	static bool SortHeight(struct Polygon Src, struct Polygon Dest);
};


