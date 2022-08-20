
#include "PaperBurnComponent.h"
#include "../PathManager.h"
#include "StaticMeshComponent.h"
#include "AnimationMeshComponent.h"
#include "../Resource/Material/Material.h"
#include "../Scene/SceneResource.h"
#include "../Scene/Scene.h"

CPaperBurnComponent::CPaperBurnComponent()
{
	SetTypeID<CPaperBurnComponent>();
	m_CBuffer = new CPaperBurnConstantBuffer;

	m_CBuffer->Init();

	m_FinishTime = 2.f;

	m_StartPaperBurn = false;

	m_EndEvent = PaperBurnEndEvent::None;

	m_Filter = 0.f;
}

CPaperBurnComponent::CPaperBurnComponent(const CPaperBurnComponent& com)	:
	CObjectComponent(com)
{
	m_CBuffer = com.m_CBuffer->Clone();

	m_FinishTime = com.m_FinishTime;
	m_StartPaperBurn = false;
	m_Filter = 0.f;
	m_EndEvent = com.m_EndEvent;

	size_t Size = com.m_vecMaterial.size();
	m_vecMaterial.resize(Size);

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterial[i] = com.m_vecMaterial[i]->Clone();
	}

	// TODO : 복사 처리
}

CPaperBurnComponent::~CPaperBurnComponent()
{
	SAFE_DELETE(m_CBuffer);

	auto iter = m_vecMaterial.begin();
	auto iterEnd = m_vecMaterial.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->DeleteRenderCallback(this);
	}
}

void CPaperBurnComponent::StartPaperBurn()
{
	if (m_vecMaterial.empty() || m_StartPaperBurn)
		return;

	auto iter = m_vecMaterial.begin();
	auto iterEnd = m_vecMaterial.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->SetMetallic(false);
		(*iter)->SetPaperBurn(true);
	}
	m_StartPaperBurn = true;
}

void CPaperBurnComponent::ResetPaperBurn()
{
	m_StartPaperBurn = false;
	m_Filter = 0.f;

	auto iter = m_vecMaterial.begin();
	auto iterEnd = m_vecMaterial.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->SetPaperBurn(false);
	}

	if (m_PaperBurnComponent)
	{
		m_PaperBurnComponent->SetDrawShadow(true);
	}
	else
	{
		m_Object->GetRootComponent()->SetDrawShadow(true);
	}
}

void CPaperBurnComponent::SetInverse(bool Enable)
{
	m_CBuffer->SetInverse(Enable);
}

bool CPaperBurnComponent::SetPaperBurnComponent(CSceneComponent* Comp)
{
	bool Ret = false;

	if (Comp->GetTypeID() == typeid(CStaticMeshComponent).hash_code())
	{
		SetPaperBurnComponent((CStaticMeshComponent*)Comp);
		Ret = true;
	}
	else if (Comp->GetTypeID() == typeid(CAnimationMeshComponent).hash_code())
	{
		SetPaperBurnComponent((CAnimationMeshComponent*)Comp);
		Ret = true;
	}

	if (Ret)
	{
		m_PaperBurnComponentName = Comp->GetName();
		m_PaperBurnComponent = Comp;
	}

	return Ret;
}

void CPaperBurnComponent::SetPaperBurnComponent(CStaticMeshComponent* SMeshCom)
{
	int SlotCount = SMeshCom->GetMaterialSlotCount();

	CMaterial* Mat = nullptr;
	for (int i = 0; i < SlotCount; ++i)
	{
		Mat = SMeshCom->GetMaterial(i);
		AddMaterial(Mat);
	}
}

void CPaperBurnComponent::SetPaperBurnComponent(CAnimationMeshComponent* AMeshCom)
{
	int SlotCount = AMeshCom->GetMaterialSlotCount();

	CMaterial* Mat = nullptr;
	for (int i = 0; i < SlotCount; ++i)
	{
		Mat = AMeshCom->GetMaterial(i);
		AddMaterial(Mat);
	}
}

void CPaperBurnComponent::AddMaterial(CMaterial* Material)
{
	m_vecMaterial.push_back(Material);
	Material->AddRenderCallback(this, &CPaperBurnComponent::SetShader);
}

void CPaperBurnComponent::ClearMaterial()
{
	auto iter = m_vecMaterial.begin();
	auto iterEnd = m_vecMaterial.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->DeleteRenderCallback(this);
	}
	
	m_vecMaterial.clear();
}

void CPaperBurnComponent::SetBurnTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	if (!m_Scene->GetResource()->LoadTexture(Name, FileName, PathName))
		return;

	m_BurnTexture = m_Scene->GetResource()->FindTexture(Name);
}

void CPaperBurnComponent::SetBurnTexture(CTexture* Texture)
{
	m_BurnTexture = Texture;
}

void CPaperBurnComponent::SetOutLineColor(const Vector4& Color)
{
	m_CBuffer->SetOutColor(Color);
}

void CPaperBurnComponent::SetOutLineColor(float r, float g, float b, float a)
{
	m_CBuffer->SetOutColor(r, g, b, a);
}

void CPaperBurnComponent::SetInLineColor(const Vector4& Color)
{
	m_CBuffer->SetInColor(Color);
}

void CPaperBurnComponent::SetInLineColor(float r, float g, float b, float a)
{
	m_CBuffer->SetInColor(r, g, b, a);
}

void CPaperBurnComponent::SetCenterLineColor(const Vector4& Color)
{
	m_CBuffer->SetCenterColor(Color);
}

void CPaperBurnComponent::SetCenterLineColor(float r, float g, float b, float a)
{
	m_CBuffer->SetCenterColor(r, g, b, a);
}

void CPaperBurnComponent::SetInFilter(float Filter)
{
	m_CBuffer->SetInFilter(Filter);
}

void CPaperBurnComponent::SetOutFilter(float Filter)
{
	m_CBuffer->SetOutFilter(Filter);
}

void CPaperBurnComponent::SetCenterFilter(float Filter)
{
	m_CBuffer->SetCenterFilter(Filter);
}

void CPaperBurnComponent::Start()
{
	CObjectComponent::Start();

	if (!m_BurnTexture)
	{
		SetBurnTexture("DefaultBurnTexture", TEXT("DefaultPaperBurn.png"));
	}

	// 페이퍼번 컴포넌트 에디터 등에서 지정되지 않으면
	if (m_PaperBurnComponentName.empty())
	{
		// 루트 컴포넌트를 기본 PaperBurn 적용 컴포넌트로 설정
		CSceneComponent* Root = m_Object->GetRootComponent();

		// Static Or Animation Mesh Component일 경우에만
		// TODO : Particle 추가?
		if (Root)
		{
			SetPaperBurnComponent(Root);
		}
	}
	else
	{
		CSceneComponent* Comp = (CSceneComponent*)m_Object->FindComponent(m_PaperBurnComponentName);

		if (Comp)
		{
			SetPaperBurnComponent(Comp);
		}
	}
}

bool CPaperBurnComponent::Init()
{
	SetBurnTexture("DefaultBurnTexture", TEXT("DefaultPaperBurn.png"));

	return true;
}

void CPaperBurnComponent::Update(float DeltaTime)
{
	if (m_StartPaperBurn)
	{
		if (m_PaperBurnComponent)
		{
			m_PaperBurnComponent->SetDrawShadow(false);
		}
		else
		{
			m_Object->GetRootComponent()->SetDrawShadow(false);
		}

		m_Filter += DeltaTime / m_FinishTime;

		if (m_Filter >= 1.f)
		{
			m_Filter = 1.f;

			m_StartPaperBurn = false;

			if (m_FinishCallback)
			{
				m_FinishCallback();
			}

			switch (m_EndEvent)
			{
			case PaperBurnEndEvent::Destroy:
			{
				if (m_Object->GetRootComponent() == m_PaperBurnComponent)
				{
					m_Object->Destroy();
				}
				else
				{
					m_PaperBurnComponent->Destroy();
				}
			}
				break;
			case PaperBurnEndEvent::Disable:
				if (m_Object->GetRootComponent() == m_PaperBurnComponent)
				{
					m_Object->Enable(false);
				}
				else
				{
					m_PaperBurnComponent->Enable(false);
				}
				break;
			case PaperBurnEndEvent::Return:
				// TODO : Object Pool에 Return 해야 하는 경우
				break;
			case PaperBurnEndEvent::Reset:
				ResetPaperBurn();
				break;
			}

			bool Inverse = m_CBuffer->IsInverse();

				m_Filter = 0.f;
		}

		m_CBuffer->SetFilter(m_Filter);
	}
}

void CPaperBurnComponent::PostUpdate(float DeltaTime)
{
}

void CPaperBurnComponent::PrevRender()
{
}

void CPaperBurnComponent::Render()
{
}

void CPaperBurnComponent::PostRender()
{
}

CPaperBurnComponent* CPaperBurnComponent::Clone()
{
	return new CPaperBurnComponent(*this);
}

bool CPaperBurnComponent::Save(FILE* File)
{
	CObjectComponent::Save(File);

	Vector4 InCol, OutCol, CenterCol;
	float InFil, OutFil, CenterFil;
	bool Inverse = false;

	InCol = m_CBuffer->GetInlineColor();
	OutCol = m_CBuffer->GetOutlineColor();
	CenterCol = m_CBuffer->GetCenterlineColor();
	InFil = m_CBuffer->GetInFilter();
	OutFil = m_CBuffer->GetOutFilter();
	CenterFil = m_CBuffer->GetCenterFilter();
	Inverse = m_CBuffer->IsInverse();

	fwrite(&InCol, sizeof(Vector4), 1, File);
	fwrite(&OutCol, sizeof(Vector4), 1, File);
	fwrite(&CenterCol, sizeof(Vector4), 1, File);
	fwrite(&InFil, sizeof(float), 1, File);
	fwrite(&OutFil, sizeof(float), 1, File);
	fwrite(&CenterFil, sizeof(float), 1, File);
	fwrite(&Inverse, sizeof(bool), 1, File);

	fwrite(&m_FinishTime, sizeof(float), 1, File);
	fwrite(&m_EndEvent, sizeof(PaperBurnEndEvent), 1, File);

	size_t Length = (int)m_PaperBurnComponentName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_PaperBurnComponentName.c_str(), sizeof(char), Length, File);

	std::string TexName = m_BurnTexture->GetName();
	Length = (int)TexName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(TexName.c_str(), sizeof(char), Length, File);

	const std::vector<TextureResourceInfo*>& vecTextureResourceInfo = m_BurnTexture->GetVecTextureInfo();
	size_t Size = vecTextureResourceInfo.size();
	fwrite(&Size, sizeof(size_t), 1, File);

	TCHAR* FName = nullptr;
	char* PName = nullptr;
	for (size_t i = 0; i < Size; ++i)
	{
		FName = vecTextureResourceInfo[i]->FileName;
		PName = vecTextureResourceInfo[i]->PathName;

		Length = lstrlen(FName);
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(FName, sizeof(TCHAR), Length, File);

		Length = strlen(PName);
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(PName, sizeof(char), Length, File);
	}

	return true;
}

bool CPaperBurnComponent::Load(FILE* File)
{
	CObjectComponent::Load(File);

	Vector4 InCol, OutCol, CenterCol;
	float InFil, OutFil, CenterFil;
	bool Inverse = false;

	fread(&InCol, sizeof(Vector4), 1, File);
	fread(&OutCol, sizeof(Vector4), 1, File);
	fread(&CenterCol, sizeof(Vector4), 1, File);
	fread(&InFil, sizeof(float), 1, File);
	fread(&OutFil, sizeof(float), 1, File);
	fread(&CenterFil, sizeof(float), 1, File);
	fread(&Inverse, sizeof(bool), 1, File);

	m_CBuffer->SetInColor(InCol);
	m_CBuffer->SetOutColor(OutCol);
	m_CBuffer->SetCenterColor(CenterCol);
	m_CBuffer->SetInFilter(InFil);
	m_CBuffer->SetOutFilter(OutFil);
	m_CBuffer->SetCenterFilter(CenterFil);
	m_CBuffer->SetInverse(Inverse);

	fread(&m_FinishTime, sizeof(float), 1, File);
	fread(&m_EndEvent, sizeof(PaperBurnEndEvent), 1, File);

	int Length = 0;
	char Buf[128] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_PaperBurnComponentName = Buf;

	ZeroMemory(&Buf, Length);
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);

	std::string TexName = Buf;
	CTexture* LoadedTex = m_Scene->GetResource()->FindTexture(TexName);

	if (LoadedTex)
	{
		m_BurnTexture = LoadedTex;
	}

	size_t Size = 0;
	fread(&Size, sizeof(size_t), 1, File);

	std::vector<TCHAR*> vecFullPath;
	vecFullPath.resize(Size);

	TCHAR BufFileName[MAX_PATH] = {};
	char BufPathName[MAX_PATH] = {};

	for (size_t i = 0; i < Size; ++i)
	{
		vecFullPath[i] = new TCHAR[MAX_PATH];

		ZeroMemory(&BufFileName, MAX_PATH);
		ZeroMemory(&BufPathName, MAX_PATH);

		fread(&Length, sizeof(int), 1, File);
		fread(&BufFileName, sizeof(TCHAR), Length, File);
		fread(&Length, sizeof(int), 1, File);
		fread(&BufPathName, sizeof(char), Length, File);

		const PathInfo* Info = CPathManager::GetInst()->FindPath(BufPathName);
		lstrcpy(vecFullPath[i], Info->Path);
		lstrcat(vecFullPath[i], BufFileName);
	}
	
	if (!m_BurnTexture)
	{
		if (!m_Scene->GetResource()->LoadTextureFullPath(TexName, vecFullPath))
		{
			for (size_t i = 0; i < Size; ++i)
			{
				SAFE_DELETE_ARRAY(vecFullPath[i]);
			}
			return false;
		}

		m_BurnTexture = m_Scene->GetResource()->FindTexture(TexName);
	}

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE_ARRAY(vecFullPath[i]);
	}

	return true;
}

bool CPaperBurnComponent::SaveOnly(FILE* File)
{
	CObjectComponent::SaveOnly(File);

	Vector4 InCol, OutCol, CenterCol;
	float InFil, OutFil, CenterFil;
	bool Inverse = false;

	InCol = m_CBuffer->GetInlineColor();
	OutCol = m_CBuffer->GetOutlineColor();
	CenterCol = m_CBuffer->GetCenterlineColor();
	InFil = m_CBuffer->GetInFilter();
	OutFil = m_CBuffer->GetOutFilter();
	CenterFil = m_CBuffer->GetCenterFilter();
	Inverse = m_CBuffer->IsInverse();

	fwrite(&InCol, sizeof(Vector4), 1, File);
	fwrite(&OutCol, sizeof(Vector4), 1, File);
	fwrite(&CenterCol, sizeof(Vector4), 1, File);
	fwrite(&InFil, sizeof(float), 1, File);
	fwrite(&OutFil, sizeof(float), 1, File);
	fwrite(&CenterFil, sizeof(float), 1, File);
	fwrite(&Inverse, sizeof(bool), 1, File);

	fwrite(&m_FinishTime, sizeof(float), 1, File);
	fwrite(&m_EndEvent, sizeof(PaperBurnEndEvent), 1, File);

	int Length = (int)m_PaperBurnComponentName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_PaperBurnComponentName.c_str(), sizeof(char), Length, File);

	std::string TexName = m_BurnTexture->GetName();
	Length = (int)TexName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(TexName.c_str(), sizeof(char), Length, File);

	const std::vector<TextureResourceInfo*>& vecTextureResourceInfo = m_BurnTexture->GetVecTextureInfo();
	size_t Size = vecTextureResourceInfo.size();
	fwrite(&Size, sizeof(size_t), 1, File);

	TCHAR* FName = nullptr;
	char* PName = nullptr;
	for (size_t i = 0; i < Size; ++i)
	{
		FName = vecTextureResourceInfo[i]->FileName;
		PName = vecTextureResourceInfo[i]->PathName;

		Length = lstrlen(FName);
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(FName, sizeof(TCHAR), Length, File);

		Length = strlen(PName);
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(PName, sizeof(char), Length, File);
	}

	return true;
}

bool CPaperBurnComponent::LoadOnly(FILE* File)
{
	CObjectComponent::LoadOnly(File);

	Vector4 InCol, OutCol, CenterCol;
	float InFil, OutFil, CenterFil;
	bool Inverse = false;

	fread(&InCol, sizeof(Vector4), 1, File);
	fread(&OutCol, sizeof(Vector4), 1, File);
	fread(&CenterCol, sizeof(Vector4), 1, File);
	fread(&InFil, sizeof(float), 1, File);
	fread(&OutFil, sizeof(float), 1, File);
	fread(&CenterFil, sizeof(float), 1, File);
	fread(&Inverse, sizeof(bool), 1, File);

	m_CBuffer->SetInColor(InCol);
	m_CBuffer->SetOutColor(OutCol);
	m_CBuffer->SetCenterColor(CenterCol);
	m_CBuffer->SetInFilter(InFil);
	m_CBuffer->SetOutFilter(OutFil);
	m_CBuffer->SetCenterFilter(CenterFil);
	m_CBuffer->SetInverse(Inverse);

	fread(&m_FinishTime, sizeof(float), 1, File);
	fread(&m_EndEvent, sizeof(PaperBurnEndEvent), 1, File);

	int Length = 0;
	char Buf[128] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_PaperBurnComponentName = Buf;

	ZeroMemory(&Buf, Length);
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);

	std::string TexName = Buf;
	CTexture* LoadedTex = m_Scene->GetResource()->FindTexture(TexName);

	if (LoadedTex)
	{
		m_BurnTexture = LoadedTex;
	}

	size_t Size = 0;
	fread(&Size, sizeof(size_t), 1, File);

	std::vector<TCHAR*> vecFullPath;
	vecFullPath.resize(Size);

	TCHAR BufFileName[MAX_PATH] = {};
	char BufPathName[MAX_PATH] = {};

	for (size_t i = 0; i < Size; ++i)
	{
		vecFullPath[i] = new TCHAR[MAX_PATH];

		ZeroMemory(&BufFileName, MAX_PATH);
		ZeroMemory(&BufPathName, MAX_PATH);

		fread(&Length, sizeof(int), 1, File);
		fread(&BufFileName, sizeof(TCHAR), Length, File);
		fread(&Length, sizeof(int), 1, File);
		fread(&BufPathName, sizeof(char), Length, File);

		const PathInfo* Info = CPathManager::GetInst()->FindPath(BufPathName);
		lstrcpy(vecFullPath[i], Info->Path);
		lstrcat(vecFullPath[i], BufFileName);
	}

	if (!m_BurnTexture)
	{
		if (!m_Scene->GetResource()->LoadTextureFullPath(TexName, vecFullPath))
		{
			for (size_t i = 0; i < Size; ++i)
			{
				SAFE_DELETE_ARRAY(vecFullPath[i]);
			}
			return false;
		}

		m_BurnTexture = m_Scene->GetResource()->FindTexture(TexName);
	}

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE_ARRAY(vecFullPath[i]);
	}

	return true;
}

void CPaperBurnComponent::SetShader()
{
	m_CBuffer->UpdateCBuffer();

	m_BurnTexture->SetShader(101, (int)Buffer_Shader_Type::Pixel, 0);
}

bool CPaperBurnComponent::IsInverse() const
{
	return m_CBuffer->IsInverse();
}

CTexture* CPaperBurnComponent::GetTexture() const
{
	return m_BurnTexture;
}

const Vector4& CPaperBurnComponent::GetOutlineColor() const
{
	return m_CBuffer->GetOutlineColor();
}

const Vector4& CPaperBurnComponent::GetInlineColor() const
{
	return m_CBuffer->GetInlineColor();
}

const Vector4& CPaperBurnComponent::GetCenterlineColor() const
{
	return m_CBuffer->GetCenterlineColor();
}

float CPaperBurnComponent::GetOutFilter() const
{
	return m_CBuffer->GetOutFilter();
}

float CPaperBurnComponent::GetInFilter() const
{
	return m_CBuffer->GetInFilter();
}

float CPaperBurnComponent::GetCenterFilter() const
{
	return m_CBuffer->GetCenterFilter();
}
