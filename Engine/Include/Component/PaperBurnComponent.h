#pragma once

#include "ObjectComponent.h"
#include "../Resource/Shader/PaperBurnConstantBuffer.h"
#include "../Resource/Texture/Texture.h"

class CPaperBurnComponent :
	public CObjectComponent
{
	friend class CGameObject;

protected:
	CPaperBurnComponent();
	CPaperBurnComponent(const CPaperBurnComponent& com);
	virtual ~CPaperBurnComponent();

private:
	CPaperBurnConstantBuffer* m_CBuffer;
	std::function<void()>   m_FinishCallback;
	float					m_FinishTime;
	CSharedPtr<CTexture>	m_BurnTexture;
	bool					m_StartPaperBurn;
	float					m_Filter;
	PaperBurnEndEvent		m_EndEvent;
	std::string				m_PaperBurnComponentName;
	class CSceneComponent*	m_PaperBurnComponent;
	std::vector<CSharedPtr<class CMaterial>> m_vecMaterial;

public:
	void SetInstancingInfo(Instancing3DInfo* Info)
	{
		m_CBuffer->SetInstancingInfo(Info);
	}

	void StartPaperBurn();
	void ResetPaperBurn();
	void SetInverse(bool Enable);
	void SetFinishTime(float Time)
	{
		m_FinishTime = Time;
	}

	bool SetPaperBurnComponent(class CSceneComponent* Comp);
	void SetPaperBurnComponent(class CStaticMeshComponent* SMeshCom);
	void SetPaperBurnComponent(class CAnimationMeshComponent* AMeshCom);
	void AddMaterial(class CMaterial* Material);
	void ClearMaterial();

	void SetBurnTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
	void SetBurnTexture(class CTexture* Texture);

	void SetOutLineColor(const Vector4& Color);
	void SetOutLineColor(float r, float g, float b, float a);
	void SetInLineColor(const Vector4& Color);
	void SetInLineColor(float r, float g, float b, float a);
	void SetCenterLineColor(const Vector4& Color);
	void SetCenterLineColor(float r, float g, float b, float a);
	void SetInFilter(float Filter);
	void SetOutFilter(float Filter);
	void SetCenterFilter(float Filter);

	void SetEndEvent(PaperBurnEndEvent EventType)
	{
		m_EndEvent = EventType;
	}

public:
	bool IsStart() const
	{
		return m_StartPaperBurn;
	}

	float GetFinishTime() const
	{
		return m_FinishTime;
	}

	const std::string& GetPaperBurnComponentName() const
	{
		return m_PaperBurnComponentName;
	}

	PaperBurnEndEvent GetEndEvent() const
	{
		return m_EndEvent;
	}

	bool IsInverse() const;
	CTexture* GetTexture() const;
	const Vector4& GetOutlineColor() const;
	const Vector4& GetInlineColor() const;
	const Vector4& GetCenterlineColor() const;
	float GetOutFilter() const;
	float GetInFilter() const;
	float GetCenterFilter() const;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CPaperBurnComponent* Clone();
    virtual bool Save(FILE* File) override;
    virtual bool Load(FILE* File) override;
    virtual bool SaveOnly(FILE* File) override;
    virtual bool LoadOnly(FILE* File) override;

public:
	void SetShader();

public:
	template <typename T>
	void SetFinishCallback(T* Obj, void(T::* Func)())
	{
		m_FinishCallback = std::bind(Func, Obj);
	}
};

