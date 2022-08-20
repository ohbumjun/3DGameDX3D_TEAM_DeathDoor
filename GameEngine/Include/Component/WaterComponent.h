#pragma once

#include "SceneComponent.h"

class CWaterComponent :
    public CSceneComponent
{
	friend class CGameObject;

protected:
	CWaterComponent();
	CWaterComponent(const CWaterComponent& com);
	virtual ~CWaterComponent();

public:
	virtual void Start() override;
	virtual bool Init() override;
	virtual void Render() override;
	virtual CWaterComponent* Clone() override;
	virtual bool Save(FILE* File) override;
	virtual bool Load(FILE* File) override;
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public:
	void SetMaterial(class CMaterial* Mat);
	void SetSpeed(float Speed);
	void SetFoamDepth(float Depth);
	void SetAttn1(float Attn1);
	void SetAttn2(float Attn2);

public:
	CMaterial* GetMaterial()
	{
		return m_Material;
	}

	float GetSpeed() const;
	float GetFoamDepth() const;
	float GetAttn1() const;
	float GetAttn2() const;

protected:
	CSharedPtr<class CMesh> m_Mesh;
	CSharedPtr<class CMaterial> m_Material;
	class CWaterCBuffer* m_CBuffer;
};

