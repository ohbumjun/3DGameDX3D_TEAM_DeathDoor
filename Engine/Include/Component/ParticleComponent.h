#pragma once

#include "SceneComponent.h"
#include "../Resource/Particle/Particle.h"
#include "../Resource/Mesh/Mesh.h"
#include "../Resource/Shader/ParticleUpdateShader.h"
#include "../Resource/Shader/ParticleRenderShader.h"
#include "../Resource/Shader/ParticleConstantBuffer.h"
#include "../Resource/Shader/ParticleTempValConstantBuffer.h"
#include "../Resource/Material/Material.h"

class CParticleComponent :
    public CSceneComponent
{
	friend class CGameObject;

protected:
	CParticleComponent();
	CParticleComponent(const CParticleComponent& com);
	virtual ~CParticleComponent();

protected:
	CSharedPtr<CParticle>					m_Particle;
	std::vector<class CStructuredBuffer*>	m_vecStructuredBuffer;
	CSharedPtr<CMesh>						m_Mesh;
	CSharedPtr<CMaterial>					m_Material;
	CSharedPtr<CParticleUpdateShader>		m_UpdateShader;
	CParticleConstantBuffer*				m_CBuffer;
	CParticleTempValConstantBuffer*				m_TempVCBuffer;
	ParticleInfo							m_Info;
	ParticleInfoShared						m_InfoShared;
	float									m_SpawnTime;
	float									m_SpawnTimeMax;
	// BillBoard
	bool  m_BillBoardEffect;
	bool m_UpdateInitBillBoardDir;
	Vector3 m_InitBillBoardZLookDir;
	Vector3 m_InitBillBoardXLookDir;
	// Move
	Vector3 m_ParticleRotOffset;
	// ���� Delay Time -> ex. 0.5�� �Ŀ� �ش� ��ġ�� ��Ÿ���� �ϱ�
	float m_InitActiveDelayTime;
	// �Ͻ������� �����ǰ� ������� Pariticle ���� ���� �� (�ش� �ð� ���� Enable False ó�����ش�)
	float m_TempCreateAccTime;
	float m_TempCreateAccTimeMax;
private :
	std::string m_ParticleName;
public:
	void StartParticle(const Vector3& StartPos);
	void SetParticle(const std::string& Name);
	void SetParticle(CParticle* Particle);
	void SetParticleWithOutCloneShader(CParticle* Particle);
	void SetSpawnTime(float Time);
	void ExecuteComputeShader();
private :
	void ApplyBillBoardEffect();
public:
	virtual void Start();
	virtual bool Init();
	virtual void Reset();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void RenderParticleEffectEditor();
	virtual void PostRender();
	virtual CParticleComponent* Clone();
public :
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	// Editor ���� Test ������ Ư�� Button�� ������ ó������ �ٽ� �����ϰ� �ϴ� ���
public :
	void DestroyExistingParticles();
	void RecreateOnlyOnceCreatedParticle();
	void RecreateOnlyOnceCreatedParticleWithOutLifeTimeSetting();
private :
	virtual bool SaveOnly(FILE* File);
	virtual bool LoadOnly(FILE* File);
public:
	float GetInitActiveDelayTime() const
	{
		return m_InitActiveDelayTime;
	}
	bool IsBillBoardEffectEnable() const
	{
		return m_BillBoardEffect;
	}
	CParticleConstantBuffer* GetCBuffer() const
	{
		return m_CBuffer;
	}
	CParticle* GetParticle() const
	{
		return m_Particle;
	}
	const std::string& GetParticleName() const
	{
		return m_ParticleName;
	}
	CMaterial* GetMaterial() const
	{
		return m_Material;
	}
public:
	// void SetGravityEffect(bool Enable)
	// {
	// 	m_ParticleMoveSpeed = Enable;
	// }
	// void SetParticleMoveSpeed(float Speed)
	// {
	// 	m_ParticleMoveSpeed = Speed;
	// }
	void SetParticleClassFileName(const std::string& ParticleFileName)
	{
		m_ParticleName = ParticleFileName;
	}
	void SetMaterial(class CMaterial* Material)
	{
		m_Material = Material;
	}
	void SetBillBoardEffect(bool Enable);
	
	// void SetBazierMoveEffect(bool Enable)
	// {
	// 	m_BazierMoveEffect = Enable;
	// }
	void SetInitActiveDelayTime(float Time)
	{
		m_InitActiveDelayTime = Time;
	}
};

