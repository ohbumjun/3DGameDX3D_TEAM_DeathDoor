#pragma once

#include "Component/SceneComponent.h"

class CEyeLaserComponent :
    public CSceneComponent
{
	friend class CGameObject;

protected:
	CEyeLaserComponent();
	CEyeLaserComponent(const CEyeLaserComponent& com);
	virtual ~CEyeLaserComponent();

private:
	// TriggerBox를 플레이어가 타격한 횟수 -> 1개만 하면 EyeLaser깨어남, 4개 모두 부시면 EyeLaser도 파괴
	int m_TriggerHitCount;
	class CGameObject* m_Player;
	class CGameObject* m_LaserGenerator;
	Vector3 m_CurrentLaserLeftRightDir;
	Vector3 m_CurrentLaserUpDownDir;
	Vector3 m_CurrentLaserDir;
	Vector3 m_NormalDir;

	Matrix m_LaserLeftRightRotMatrix;
	Matrix m_LaserUpDownRotMatrix;

	//CSharedPtr<CShader>	m_Shader;
	CSharedPtr<CMesh> m_LaserPlaneMesh;
	CSharedPtr<CMaterial>	m_Material;

	class CColliderHalfLine* m_RayCollider;

	bool m_FaceCameraOnce;
	float m_AccTime;
	float m_LaserGenTime;

	bool m_LaserShootSoundPlay;

public:
	CMesh* GetMesh()	const
	{
		return m_LaserPlaneMesh;
	}

	CMaterial* GetMaterial()	const
	{
		return m_Material;
	}

	void SetWakeEnd();

	void AddTriggerCount()
	{
		++m_TriggerHitCount;
	}

	int GetTriggerCount() const
	{
		return m_TriggerHitCount;
	}

	bool IsTransparent()	const
	{
		return m_Material->IsTransparent();
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CEyeLaserComponent* Clone();


public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

private:
	void TrackPlayer(float DeltaTime);
	void ChangeToWakeAnimation();
	void ChangeToDieAnimation();
	void ChangeToIdleAnimation();
	void FaceCamera();
	void OnRaserCollisionBegin(const CollisionResult& Result);
	void OnRaserCollisionStay(const CollisionResult& Result);
	void OnRaserCollisionEnd(const CollisionResult& Result);

public:
	void SetBaseColor(const Vector4& Color, int Index = 0);
	void SetBaseColor(float r, float g, float b, float a, int Index = 0);
	void SetEmissiveColor(const Vector4& Color, int Index = 0);
	void SetEmissiveColor(float r, float g, float b, float a, int Index = 0);
	void SetTransparencyMaterial(bool Enable);
	void SetMaterialShader(const std::string& Name);
	void OnChangeMaterialShader(CGraphicShader* NewShader);
	void ChangeInstancingLayer();
	void OnCreateNewInstancingCheckCount();

	void SetOpacity(float Opacity);
	void AddOpacity(float Opacity);
};

