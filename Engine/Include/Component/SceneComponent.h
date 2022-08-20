#pragma once

#include "Component.h"
#include "Transform.h"
#include "../Resource/Mesh/Mesh.h"
#include "../Resource/Material/Material.h"

struct InstancingCheckCount
{
	std::string LayerName;
	CMesh* Mesh;
	std::vector<CGraphicShader*> vecInstancingShader;				// Material ����ŭ
	std::vector<ShaderParams> vecShaderParams;						// Material ����ŭ
	std::list<class CSceneComponent*> InstancingList;

	InstancingCheckCount() :
		Mesh(nullptr)
	{
	}
};

class CSceneComponent :
	public CComponent
{
	friend class CGameObject;

protected:
	CSceneComponent();
	CSceneComponent(const CSceneComponent& com);
	virtual ~CSceneComponent();

protected:
	CTransform* m_Transform;
	CSceneComponent* m_Parent;
	std::vector<CSharedPtr<CSceneComponent>>    m_vecChild;
	bool    m_Render;
	std::string		m_LayerName;
	class CSkeletonSocket* m_Socket;
	SphereInfo m_SphereInfo;
	bool m_Culling;
	bool m_ReceiveDecal;
	bool m_Instancing;
	bool m_DrawShadow;
	static std::list<InstancingCheckCount*> m_InstancingCheckList;
	std::function<void(CSceneComponent*)> m_OnDestroyCallBack;


public:
	void SetInstancingInfo(Instancing3DInfo* Info);
	void SetInstancingShadowInfo(Instancing3DInfo* Info);

	static const std::list<InstancingCheckCount*>* GetInstancingCheckList()
	{
		return &m_InstancingCheckList;
	}

	static void DestroyInstancingCheckList()
	{
		auto	iter = m_InstancingCheckList.begin();
		auto	iterEnd = m_InstancingCheckList.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE((*iter));
		}
	}

	CSceneComponent* GetParent()	const
	{
		return m_Parent;
	}

	bool GetInstancing() const
	{
		return m_Instancing;
	}

	bool GetRender() const
	{
		return m_Render;
	}

	bool GetCulling() const
	{
		return m_Culling;
	}

	SphereInfo GetSphereInfo() const
	{
		SphereInfo Info;
		
		// Info.Center = m_SphereInfo.Center * GetWorldScale() + GetWorldPos();
		Info.Center = m_SphereInfo.Center.TransformCoord(GetWorldMatrix());
		Info.Radius = m_SphereInfo.Radius;

		return Info;
	}

	SphereInfo GetSphereInfoViewSpace() const;

	const SphereInfo& GetSphereOriginInfo() const
	{
		return m_SphereInfo;
	}

	CTransform* GetTransform()	const
	{
		return m_Transform;
	}

	std::string GetLayerName()	const
	{
		return m_LayerName;
	}

	bool IsRender() const
	{
		return m_Render;
	}

	bool IsDrawShadow() const
	{
		return m_DrawShadow;
	}

	float GetViewZ() const;

public:
	void SetInstancing(bool Instancing)
	{
		m_Instancing = Instancing;
	}

	void SetReceiveDecal(bool Decal)
	{
		m_ReceiveDecal = Decal;
	}

	void SetLayerName(const std::string& Name)
	{
		m_LayerName = Name;
	}

	void SetRender(bool Render)
	{
		m_Render = Render;
	}

	void SetDrawShadow(bool Enable)
	{
		m_DrawShadow = Enable;
	}

	virtual void Enable(bool Enable) override;

public:
	void SetSceneComponent(class CGameObject* Object);
	void GetAllSceneComponentsName(std::vector<FindComponentName>& vecNames);
	virtual void Destroy() override;
	void DestroySingle();
	void GetAllSceneComponentsPointer(std::vector<CSceneComponent*>& OutVecSceneComp);

public:
	virtual void SetScene(class CScene* Scene);
	virtual void SetGameObject(class CGameObject* Object);

public:
	virtual void AddChild(CSceneComponent* Child, const std::string& SocketName = "");
	virtual void AddChild(class CGameObject* Child, const std::string& SocketName = "");
	bool DeleteChild(CSceneComponent* Child);
	bool DeleteChild(const std::string& Name);
	bool ReplaceComponent(CSceneComponent* DestNode);
	bool DeleteComponent();
	size_t GetChildCount()	const;
	CSceneComponent* GetChild(int Idx)	const;
	CSceneComponent* FindComponent(const std::string& Name);

	// �θ𿡰� ���� �ڽĸ�Ͽ��� ������ϰ�, ���� ��嵵 m_Parent = nullptr
	void ClearParent();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void CheckCollision();
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderShadowMap();
	virtual void RenderDebug();
	virtual void PostRender();
	virtual void Reset() override;
	virtual CSceneComponent* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
    virtual bool SaveOnly(FILE* File) override;
    virtual bool LoadOnly(FILE* File) override;

public :
	virtual void RenderAnimationEditor();
	virtual void RenderParticleEffectEditor();

public:
	void SetState(Transform_State State)
	{
		m_Transform->m_State = State;
	}

	const Transform_State& GetState()	const
	{
		return m_Transform->m_State;
	}

public:
	template <typename T>
	void AddOnDestoryCallBack(T* Obj, void(T::* Func)(CSceneComponent*))
	{
		m_OnDestroyCallBack = std::bind(Func, Obj, std::placeholders::_1);
	}

public:	// Transform
	void SetUpdateByMat(bool UpdateByMat);
	void DecomposeWorld();

	// �ڽ��� �� �������� �̵�
	void AddWorldPosByLocalAxis(AXIS Axis, float Amount);
	void AddWorldPosByLocalAxis(const Vector3& Pos);

	void SetTransformByWorldMatrix(const Matrix& matTRS);

	void SetInheritScale(bool Inherit)
	{
		m_Transform->SetInheritScale(Inherit);
	}

	void SetInheritRotX(bool Inherit)
	{
		m_Transform->SetInheritRotX(Inherit);
	}

	void SetInheritRotY(bool Inherit)
	{
		m_Transform->SetInheritRotY(Inherit);
	}

	void SetInheritRotZ(bool Inherit)
	{
		m_Transform->SetInheritRotZ(Inherit);
	}

	void SetInheritParentRotationPosX(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosX(Inherit);
	}

	void SetInheritParentRotationPosY(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosY(Inherit);
	}

	void SetInheritParentRotationPosZ(bool Inherit)
	{
		m_Transform->SetInheritParentRotationPosZ(Inherit);
	}

	void InheritScale(bool Current)
	{
		m_Transform->InheritScale(Current);
	}

	void InheritRotation(bool Current)
	{
		m_Transform->InheritRotation(Current);
	}

	void InheritParentRotationPos(bool Current)
	{
		m_Transform->InheritParentRotationPos(Current);
	}

	void InheritWorldScale(bool Current)
	{
		m_Transform->InheritWorldScale(Current);
	}

	void InheritWorldRotation(bool Current)
	{
		m_Transform->InheritWorldRotation(Current);
	}

public:
	Vector3 GetRelativeScale()	const
	{
		return m_Transform->GetRelativeScale();
	}

	Vector3 GetRelativeRot()	const
	{
		return m_Transform->GetRelativeRot();
	}

	Vector3 GetRelativePos()	const
	{
		return m_Transform->GetRelativePos();
	}

	Vector3 GetRelativeAxis(AXIS Axis)
	{
		return m_Transform->GetRelativeAxis(Axis);
	}

public:
	void SetRelativeScale(const Vector3& Scale)
	{
		m_Transform->SetRelativeScale(Scale);
	}

	void SetRelativeScale(float x, float y, float z)
	{
		m_Transform->SetRelativeScale(x, y, z);
	}

	void SetRelativeRotation(const Vector3& Rot)
	{
		m_Transform->SetRelativeRotation(Rot);
	}

	void SetRelativeRotation(float x, float y, float z)
	{
		m_Transform->SetRelativeRotation(x, y, z);
	}

	void SetRelativeRotationX(float x)
	{
		m_Transform->SetRelativeRotationX(x);
	}

	void SetRelativeRotationY(float y)
	{
		m_Transform->SetRelativeRotationY(y);
	}

	void SetRelativeRotationZ(float z)
	{
		m_Transform->SetRelativeRotationZ(z);
	}

	void SetRelativePos(const Vector3& Pos)
	{
		m_Transform->SetRelativePos(Pos);
	}

	void SetRelativePos(float x, float y, float z)
	{
		m_Transform->SetRelativePos(x, y, z);
	}

	void AddRelativeScale(const Vector3& Scale)
	{
		m_Transform->AddRelativeScale(Scale);
	}

	void AddRelativeScale(float x, float y, float z)
	{
		m_Transform->AddRelativeScale(x, y, z);
	}

	void AddRelativeRotation(const Vector3& Rot)
	{
		m_Transform->AddRelativeRotation(Rot);
	}

	void AddRelativeRotation(float x, float y, float z)
	{
		m_Transform->AddRelativeRotation(x, y, z);
	}

	void AddRelativeRotationX(float x)
	{
		m_Transform->AddRelativeRotationX(x);
	}

	void AddRelativeRotationY(float y)
	{
		m_Transform->AddRelativeRotationY(y);
	}

	void AddRelativeRotationZ(float z)
	{
		m_Transform->AddRelativeRotationZ(z);
	}

	void AddRelativePos(const Vector3& Pos)
	{
		m_Transform->AddRelativePos(Pos);
	}

	void AddRelativePos(float x, float y, float z)
	{
		m_Transform->AddRelativePos(x, y, z);
	}

public:
	const Matrix& GetRotationMatrix() const
	{
		return m_Transform->GetRotationMatrix();
	}

	const Matrix& GetScaleMatrix() const
	{
		return m_Transform->GetRotationMatrix();
	}

	Vector3 GetWorldScale()	const
	{
		return m_Transform->GetWorldScale();
	}

	Vector3 GetWorldRot()	const
	{
		return m_Transform->GetWorldRot();
	}

	Vector3 GetWorldPos()	const
	{
		return m_Transform->GetWorldPos();
	}

	Vector3 GetPivot()	const
	{
		return m_Transform->GetPivot();
	}

	Vector3 GetMeshSize()	const
	{
		return m_Transform->GetMeshSize();
	}

	const Matrix& GetWorldMatrix()	const
	{
		return m_Transform->GetWorldMatrix();
	}

	Vector3 GetWorldAxis(AXIS Axis)
	{
		return m_Transform->GetWorldAxis(Axis);
	}

public:
	void SetPivot(const Vector3& Pivot)
	{
		m_Transform->SetPivot(Pivot);
	}

	void SetPivot(float x, float y, float z)
	{
		m_Transform->SetPivot(x, y, z);
	}

	void SetMeshSize(const Vector3& Size)
	{
		m_Transform->SetMeshSize(Size);
	}

	void SetMeshSize(float x, float y, float z)
	{
		m_Transform->SetMeshSize(x, y, z);
	}

public:
	void SetWorldScale(const Vector3& Scale)
	{
		m_Transform->SetWorldScale(Scale);
	}

	void SetWorldScale(float x, float y, float z)
	{
		m_Transform->SetWorldScale(x, y, z);
	}

	void SetWorldRotation(const Vector3& Rot)
	{
		m_Transform->SetWorldRotation(Rot);
	}

	void SetWorldRotation(float x, float y, float z)
	{
		m_Transform->SetWorldRotation(x, y, z);
	}

	void SetWorldRotationX(float x)
	{
		m_Transform->SetWorldRotationX(x);
	}

	void SetWorldRotationY(float y)
	{
		m_Transform->SetWorldRotationY(y);
	}

	void SetWorldRotationZ(float z)
	{
		m_Transform->SetWorldRotationZ(z);
	}

	void SetWorldPos(const Vector3& Pos)
	{
		m_Transform->SetWorldPos(Pos);
	}

	void SetWorldPos(float x, float y, float z)
	{
		m_Transform->SetWorldPos(x, y, z);
	}

	void AddWorldScale(const Vector3& Scale)
	{
		m_Transform->AddWorldScale(Scale);
	}

	void AddWorldScale(float x, float y, float z)
	{
		m_Transform->AddWorldScale(x, y, z);
	}

	void AddWorldRotation(const Vector3& Rot)
	{
		m_Transform->AddWorldRotation(Rot);
	}

	void AddWorldRotation(float x, float y, float z)
	{
		m_Transform->AddWorldRotation(x, y, z);
	}

	void AddWorldRotationX(float x)
	{
		m_Transform->AddWorldRotationX(x);
	}

	void AddWorldRotationY(float y)
	{
		m_Transform->AddWorldRotationY(y);
	}

	void AddWorldRotationZ(float z)
	{
		m_Transform->AddWorldRotationZ(z);
	}

	void AddWorldPos(const Vector3& Pos)
	{
		m_Transform->AddWorldPos(Pos);
	}

	void AddWorldPos(float x, float y, float z)
	{
		m_Transform->AddWorldPos(x, y, z);
	}
};

