
#include "AnimationMeshComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Render/RenderManager.h"
#include "../Resource/Shader/Standard2DConstantBuffer.h"
#include "../Animation/AnimationSequenceInstance.h"
#include "../GameObject/GameObject.h"
#include "../PathManager.h"

CAnimationMeshComponent::CAnimationMeshComponent()
{
	SetTypeID<CAnimationMeshComponent>();
	m_Render = true;
	m_Animation = nullptr;
	m_Transform->SetTransformState(Transform_State::Ground);
	
	m_InstanceID = 0;
}

CAnimationMeshComponent::CAnimationMeshComponent(const CAnimationMeshComponent& com) :
	CSceneComponent(com)
{
	m_Mesh = com.m_Mesh;

	m_vecMaterialSlot.clear();

	for (size_t i = 0; i < com.m_vecMaterialSlot.size(); ++i)
	{
		m_vecMaterialSlot.push_back(com.m_vecMaterialSlot[i]->Clone());
	}

	m_Skeleton = com.m_Skeleton->Clone();

	if (com.m_Animation)
	{
		m_Animation = com.m_Animation->Clone();
		m_Animation->SetSkeleton(m_Skeleton);
		m_Animation->SetInstancingBoneBuffer(m_Mesh->GetBoneBuffer());
	}
}

CAnimationMeshComponent::~CAnimationMeshComponent()
{
	DeleteInstancingCheckList();

	SAFE_DELETE(m_Animation);
	// SAFE_DELETE(m_Skeleton);
}

bool CAnimationMeshComponent::IsTransparent() const
{
	if (m_vecMaterialSlot.size() == 0)
	{
		return false;
	}

	// �ϳ��� Material�̶� Transparent��� ��� Material�� Transparent Layer�� ����
	return m_vecMaterialSlot[0]->IsTransparent();
}

void CAnimationMeshComponent::SetScene(CScene* Scene)
{
	CSceneComponent::SetScene(Scene);

	if (m_Skeleton)
	{
		m_Skeleton->m_Scene = Scene;
	}
	if (m_Animation)
	{
		m_Animation->SetScene(Scene);
	}
}

void CAnimationMeshComponent::SetAnimationInstance(CAnimationSequenceInstance* AnimInst)
{
	SAFE_DELETE(m_Animation);

	m_Animation = AnimInst;

	m_Animation->SetScene(m_Scene);
	m_Animation->SetOwner(this);
	m_Animation->SetSkeleton(m_Skeleton);
	m_Animation->SetInstancingBoneBuffer(m_Mesh->GetBoneBuffer());
}

void CAnimationMeshComponent::SetMesh(const std::string& Name)
{
	CAnimationMesh* FoundMesh = (CAnimationMesh*)m_Scene->GetResource()->FindMesh(Name);

	if (!FoundMesh)
		return;

	SetMesh(FoundMesh);
}

void CAnimationMeshComponent::SetMesh(CAnimationMesh* Mesh)
{
	m_Mesh = Mesh;

	m_Skeleton = m_Mesh->GetSkeleton()->Clone();

	DeleteInstancingCheckList();

	// Editor Save, Load �������� �ʿ��� �κ��� ����
	std::string MeshName = m_Mesh->GetName();
	m_Mesh->SetName(MeshName);

	// SAFE_DELETE(m_Skeleton);
	m_Skeleton = m_Mesh->GetSkeleton()->Clone();

	if (m_Animation)
	{
		m_Animation->SetSkeleton(m_Skeleton);
		m_Animation->SetInstancingBoneBuffer(m_Mesh->GetBoneBuffer());
	}

	m_vecMaterialSlot.clear();

	const std::vector<CSharedPtr<CMaterial>>* pMaterialSlots =
		m_Mesh->GetMaterialSlots();

	std::vector<CSharedPtr<CMaterial>>::const_iterator	iter = pMaterialSlots->begin();
	std::vector<CSharedPtr<CMaterial>>::const_iterator	iterEnd = pMaterialSlots->end();

	for (; iter != iterEnd; ++iter)
	{
		m_vecMaterialSlot.push_back((*iter)->Clone());

		(*iter)->SetScene(m_Scene);
	}

	SetMeshSize(m_Mesh->GetMax() - m_Mesh->GetMin());

	m_SphereInfo.Center = (m_Mesh->GetMax() + m_Mesh->GetMin()) / 2.f;

	auto	iter1 = m_InstancingCheckList.begin();
	auto	iter1End = m_InstancingCheckList.end();

	bool	Add = false;

	for (; iter1 != iter1End; ++iter1)
	{
		if ((*iter1)->Mesh == m_Mesh)
		{
			// ������ ������ ��� �ٸ� ���̾��� InstancingCheckList�� �����Ǿ�� �Ѵ�.
			if (m_LayerName != (*iter1)->LayerName)
			{
				continue;
			}

			bool	InstancingEnable = (*iter1)->InstancingList.back()->GetInstancing();

			(*iter1)->InstancingList.push_back(this);
			Add = true;

			// �ν��Ͻ� ������ �Ǵ��Ѵ�.
			if (InstancingEnable)
			{
				SetInstancing(InstancingEnable);
				m_InstanceID = (int)(*iter1)->InstancingList.size() - 1;
			}

			else
			{
				if ((*iter1)->InstancingList.size() == 10)
				{
					auto	iter2 = (*iter1)->InstancingList.begin();
					auto	iter2End = (*iter1)->InstancingList.end();

					int ID = 0;
					for (; iter2 != iter2End; ++iter2, ++ID)
					{
						((CAnimationMeshComponent*)(*iter2))->SetInstanceID(ID);
						(*iter2)->SetInstancing(true);
					}

					m_Instancing = true;
				}
			}

			break;
		}
	}

	if (!Add)
	{
		// �� ���̾ �� �޽� ������Ʈ�� ���� �߰��Ǵ� ���, ���ο� �ν��Ͻ� ī��Ʈ ����
		OnCreateNewInstancingCheckCount();
	}
}

void CAnimationMeshComponent::SetMaterial(CMaterial* Material, int Index)
{
	m_vecMaterialSlot[Index] = Material->Clone();

	m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CAnimationMeshComponent::AddMaterial(CMaterial* Material)
{
	m_vecMaterialSlot.push_back(Material->Clone());

	m_vecMaterialSlot[m_vecMaterialSlot.size() - 1]->SetScene(m_Scene);
}

void CAnimationMeshComponent::SetBaseColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetBaseColor(Color);
}

void CAnimationMeshComponent::SetBaseColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetBaseColor(r, g, b, a);
}

void CAnimationMeshComponent::SetAmbientColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetAmbientColor(Color);
}

void CAnimationMeshComponent::SetAmbientColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetAmbientColor(r, g, b, a);
}

void CAnimationMeshComponent::SetSpecularColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularColor(Color);
}

void CAnimationMeshComponent::SetSpecularColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularColor(r, g, b, a);
}

void CAnimationMeshComponent::SetEmissiveColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetEmissiveColor(Color);
}

void CAnimationMeshComponent::SetEmissiveColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetEmissiveColor(r, g, b, a);
}

void CAnimationMeshComponent::SetSpecularPower(float Power, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularPower(Power);
}

void CAnimationMeshComponent::SetRenderState(CRenderState* State, int Index)
{
	m_vecMaterialSlot[Index]->SetRenderState(State);
}

void CAnimationMeshComponent::SetRenderState(const std::string& Name, int Index)
{
	m_vecMaterialSlot[Index]->SetRenderState(Name);
}

void CAnimationMeshComponent::SetTransparencyAllMaterial(bool Enable)
{
	size_t Size = m_vecMaterialSlot.size();

	if (Enable)
	{
		bool AlreadyTransparent = m_LayerName == "Transparency";

		if (!AlreadyTransparent)
		{
			m_LayerName = "Transparency";
		}

		for (size_t i = 0; i < Size; ++i)
		{
			m_vecMaterialSlot[i]->SetTransparency(true);
		}

		// ��� Material�� Shader ��ü
		SetMaterialShaderAll("Transparent3DShader");

		// �ν��Ͻ� ���̾ �ٲ۴�.
		if (!AlreadyTransparent)
		{
			ChangeInstancingLayer();
		}
	}
	else
	{
		bool AlreadyOpaque = m_LayerName == "Default";

		if (!AlreadyOpaque)
		{
			m_LayerName = "Default";
		}

		for (size_t i = 0; i < Size; ++i)
		{
			// ���� ���̴��� �ǵ���
			m_vecMaterialSlot[i]->SetTransparency(false);
			m_vecMaterialSlot[i]->RevertShader();
		}

		// �ν��Ͻ� ���̾ �ٲ۴�.
		if (!AlreadyOpaque)
		{
			ChangeInstancingLayer();
		}
	}
}

void CAnimationMeshComponent::SetOpacity(float Opacity, int Index)
{
	m_vecMaterialSlot[Index]->SetOpacity(Opacity);
}

void CAnimationMeshComponent::AddOpacity(float Opacity, int Index)
{
	m_vecMaterialSlot[Index]->AddOpacity(Opacity);
}

void CAnimationMeshComponent::SetMetallic(bool Metallic, int Index)
{
	m_vecMaterialSlot[Index]->SetMetallic(Metallic);
}

void CAnimationMeshComponent::SetMaterialShaderAll(const std::string& Name)
{
	CGraphicShader* Shader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader(Name));

	if (!Shader)
		return;

	int Size = (int)m_vecMaterialSlot.size();
	for (int i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->SetShader(Shader);
		OnChangeMaterialShader(i, Shader);
	}
}

void CAnimationMeshComponent::SetMaterialShader(int MatIndex, const std::string& ShaderName)
{
	CGraphicShader* Shader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader(ShaderName));

	SetMaterialShader(MatIndex, Shader);
}

void CAnimationMeshComponent::SetMaterialShader(int MatIndex, CGraphicShader* Shader)
{
	if (!Shader)
	{
		return;
	}

	m_vecMaterialSlot[MatIndex]->SetShader(Shader);

	// Instancing Shader ����
	OnChangeMaterialShader(MatIndex, Shader);
}

void CAnimationMeshComponent::SetMaterialShaderParams(int MatIndex, const ShaderParams& Params)
{
	m_vecMaterialSlot[MatIndex]->SetShaderParams(Params);

	// Instancing Shader Parameter ����
	OnChangeMaterialShaderParams(MatIndex, Params);
}

void CAnimationMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, Texture);
}

void CAnimationMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, FileName, PathName);
}

void CAnimationMeshComponent::AddTextureFullPath(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath)
{
	m_vecMaterialSlot[MaterialIndex]->AddTextureFullPath(Register, ShaderType, Name, FullPath);
}

void CAnimationMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, vecFileName, PathName);
}

void CAnimationMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, Texture);
}

void CAnimationMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, FileName, PathName);
}

void CAnimationMeshComponent::SetTextureFullPath(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name,
	const TCHAR* FullPath)
{
	m_vecMaterialSlot[MaterialIndex]->SetTextureFullPath(Index, Register, ShaderType, Name, FullPath);
} 

void CAnimationMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, vecFileName, PathName);
}

void CAnimationMeshComponent::LoadAnimationMeshInstanceFromFile(const char* FileName)
{

}

bool CAnimationMeshComponent::DeleteInstancingCheckList()
{
	bool Deleted = false;

	auto	iter = m_InstancingCheckList.begin();
	auto	iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			// ������ ������ ��� �ٸ� ���̾��� InstancingCheckList�� �����Ǿ�� �Ѵ�.
			if (m_LayerName != (*iter)->LayerName)
			{
				continue;
			}

			auto	iter1 = (*iter)->InstancingList.begin();
			auto	iter1End = (*iter)->InstancingList.end();

			for (; iter1 != iter1End; ++iter1)
			{
				if (*iter1 == this)
				{
					Deleted = true;

					(*iter)->InstancingList.erase(iter1);

					if ((*iter)->InstancingList.size() < 10)
					{
						auto	iter2 = (*iter)->InstancingList.begin();
						auto	iter2End = (*iter)->InstancingList.end();

						for (; iter2 != iter2End; ++iter2)
						{
							((CAnimationMeshComponent*)(*iter2))->SetInstanceID(0);
							(*iter2)->SetInstancing(false);
						}
					}
					else
					{
						auto	iter2 = (*iter)->InstancingList.begin();
						auto	iter2End = (*iter)->InstancingList.end();

						int ID = 0;

						for (; iter2 != iter2End; ++iter2, ++ID)
						{
							((CAnimationMeshComponent*)(*iter2))->SetInstanceID(ID);
							(*iter2)->SetInstancing(true);
						}

					
					}
					break;
				}
			}

			if ((*iter)->InstancingList.empty())
			{
				SAFE_DELETE((*iter));
				m_InstancingCheckList.erase(iter);
			}

			break;
		}
	}

	return Deleted;
}

void CAnimationMeshComponent::Start()
{
	CSceneComponent::Start();

	if (m_Animation)
		m_Animation->Start();
}

bool CAnimationMeshComponent::Init()
{
	// m_Mesh = (CSpriteMesh*)m_Scene->GetResource()->FindMesh("SpriteMesh");
	// SetMaterial(m_Scene->GetResource()->FindMaterial("Color"));
	AddMaterial(m_Scene->GetResource()->FindMaterial("Color"));

	return true;
}

void CAnimationMeshComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	if (m_Animation)
		m_Animation->Update(DeltaTime);
}

void CAnimationMeshComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CAnimationMeshComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CAnimationMeshComponent::Render()
{
	CSceneComponent::Render();

	if (!m_Mesh)
		return;

	if (m_Animation)
		m_Animation->SetShader();

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

		m_vecMaterialSlot[i]->Render();

		m_Mesh->Render((int)i);

		m_vecMaterialSlot[i]->Reset();
	}

	// �÷��̾� ������ ó��
	if (m_Object == m_Scene->GetPlayerObject())
	{
		CRenderManager::GetInst()->RenderPlayer(m_Mesh);
	}

	if (m_Animation)
		m_Animation->ResetShader();
}

void CAnimationMeshComponent::RenderShadowMap()
{
	CSceneComponent::RenderShadowMap();

	if (!m_Mesh)
	{
		return;
	}

	if (m_Animation)
	{
		m_Animation->SetShader();
	}

	size_t Size = m_vecMaterialSlot.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->UpdateCBuffer();
		m_Mesh->Render((int)i);
	}

	if (m_Animation)
	{
		m_Animation->ResetShader();
	}
}

void CAnimationMeshComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CAnimationMeshComponent* CAnimationMeshComponent::Clone()
{
	return new CAnimationMeshComponent(*this);
}

bool CAnimationMeshComponent::Save(FILE* File)
{
	std::string	MeshName = m_Mesh->GetName();

	int	Length = (int)MeshName.length();

	fwrite(&Length, sizeof(int), 1, File);
	fwrite(MeshName.c_str(), sizeof(char), Length, File);

	// Animation Sqc ���� ���� Loading �Լ�
	// size_t AnimationSequenceLength = 
	size_t	 SeqFileNameLength = strlen(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFileName());
	fwrite(&SeqFileNameLength, sizeof(size_t), 1, File);
	fwrite(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFileName(), sizeof(char), SeqFileNameLength, File);

	// Animation File Name �̿��ؼ� anim File �� Load �� ���̴�.
	size_t	 AnimSavedFileNameLength = strlen(m_Animation->GetSavedFileName());
	fwrite(&AnimSavedFileNameLength, sizeof(size_t), 1, File);
	fwrite(m_Animation->GetSavedFileName(), sizeof(char), AnimSavedFileNameLength, File);

	// ���ο� Test Code
	int	MaterialSlotCount = (int)m_vecMaterialSlot.size();

	fwrite(&MaterialSlotCount, sizeof(int), 1, File);

	// ���� Bin/Material �� �ƴ϶�, Bin/Material/AnimationComponentMaterial �������� Save, Load �� �� �֤�
	// const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_PATH);
	const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_ANIMCOMPONENT_PATH);

	for (int idx = 0; idx < MaterialSlotCount; ++idx)
	{
		// Material �� Bin/Material Path �� �����ϱ�
		char MaterialBinPathMutlibyte[MAX_PATH] = {};

		if (MaterialPath)
			strcpy_s(MaterialBinPathMutlibyte, MaterialPath->PathMultibyte);

		// ObjectName_ + Mesh Name + _Idx
		// �տ� Object �� �ٿ��ִ� ������ ���� Object �� ������ Mesh ���Ͽ��� ��Ե�, Material �� ����� ���
		// A Object �� Material �� �����ߴµ�, B Object �� ���� Material ������ A Object �� ���� Material ���ϰ� ���Ƽ�
		// A Object �� Material ��ȭ�� B Object �� Material ���Ͽ��� �ݿ��Ǵ� ���̴�.
		std::string SavedExtraMtrlName;
		SavedExtraMtrlName.reserve(MeshName.length() * 3); 
		SavedExtraMtrlName = m_Object->GetName() + "_" + MeshName + "_" + std::to_string(idx) + ".mtrl";

		strcat_s(MaterialBinPathMutlibyte, SavedExtraMtrlName.c_str());

		// Animation Mesh Component �� ���� Load �ؼ� ������� Material �� Name �� �����ϱ�
		int MtrlNameLength = (int)SavedExtraMtrlName.length();

		fwrite(&MtrlNameLength, sizeof(int), 1, File);

		fwrite(SavedExtraMtrlName.c_str(), sizeof(char), MtrlNameLength, File);

		m_vecMaterialSlot[idx]->SaveFullPath(MaterialBinPathMutlibyte);
	}

	CSceneComponent::Save(File);

	return true;
}

bool CAnimationMeshComponent::Load(FILE* File)
{
	char	MeshName[256] = {};

	int	Length = 0;

	fread(&Length, sizeof(int), 1, File);
	fread(MeshName, sizeof(char), Length, File);

	size_t	 SeqFileNameLength;
	fread(&SeqFileNameLength, sizeof(size_t), 1, File);

	char SeqFileName[MAX_PATH] = {};
	fread(SeqFileName, sizeof(char), SeqFileNameLength, File);

	// Animation File Name �̿��ؼ� anim File �� Load �� ���̴�.
	char AnimSavedFileName[MAX_PATH] = {};
	size_t	 AnimSavedFileNameLength = 0;
	fread(&AnimSavedFileNameLength, sizeof(size_t), 1, File);
	fread(AnimSavedFileName,  sizeof(char), AnimSavedFileNameLength, File);

	LoadAnimationInstance<CAnimationSequenceInstance>();

	// RESOURCE_ANIMATION_PATH �� ���ؼ� .anim File Load
	if (!m_Animation->LoadAnimation(AnimSavedFileName))
		return false;

	const std::pair<bool, std::string>& result = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

	if (!result.first)
		return false;

	// result.second ���� ������ �������� Mesh �̸��� ����ִ�.
	SetMesh(result.second);

	// Animation �� �ʿ��� ���� Start �Լ��� ȣ���Ͽ� ����
	m_Animation->Start();

	// ���ο� Test Code
	int	MaterialSlotCount = -999;

	fread(&MaterialSlotCount, sizeof(int), 1, File);

	const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_ANIMCOMPONENT_PATH);

	m_vecMaterialSlot.clear();
	m_vecMaterialSlot.resize(MaterialSlotCount);

	for (int idx = 0; idx < MaterialSlotCount; ++idx)
	{
		m_vecMaterialSlot[idx] = new CMaterial;

		// Material �� Bin/Material Path �� �����ϱ�
		char MaterialBinPathMutlibyte[MAX_PATH] = {};

		if (MaterialPath)
			strcpy_s(MaterialBinPathMutlibyte, MaterialPath->PathMultibyte);

		// Mesh Name + _Idx 
		char SavedExtraName[MAX_PATH] = {};

		// Animation Mesh Component �� ���� Load �ؼ� ������� Material �� Name �� �����ϱ�
		int MtrlNameLength = -999;

		fread(&MtrlNameLength, sizeof(int), 1, File);

		fread(SavedExtraName, sizeof(char), MtrlNameLength, File);

		strcat_s(MaterialBinPathMutlibyte, SavedExtraName);

		m_vecMaterialSlot[idx]->LoadFullPath(MaterialBinPathMutlibyte);
	}

	// ���� Mesh�� �⺻ Material �������� Instancing Shader �� Parameter�� Instancing Check List�� �����Ǿ� �ֱ� ������
	// �ε��� ������ Material ������ ���� Instancing CheckList�� �������־�� ��
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	CGraphicShader* NoInstancingShader = nullptr;
	CGraphicShader* InstancingShader = nullptr;
	ShaderParams ShaderParams = {};

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				for (int i = 0; i < MaterialSlotCount; ++i)
				{
					NoInstancingShader = m_vecMaterialSlot[i]->GetShader();
					InstancingShader = CResourceManager::GetInst()->FindInstancingShader(NoInstancingShader);
					ShaderParams = m_vecMaterialSlot[i]->GetShaderParams();

					(*iter)->vecInstancingShader[i] = InstancingShader;
					(*iter)->vecShaderParams[i] = ShaderParams;
				}
			}
		}
	}

	CSceneComponent::Load(File);

	return true;
}

bool CAnimationMeshComponent::SaveFile(const char* FullPath)
{
	FILE* pFile;

	fopen_s(&pFile, FullPath, "wb");

	if (!pFile)
		return false;

	SaveOnly(pFile); 

	CSceneComponent::Save(pFile);

	fclose(pFile);

	return true;
}

bool CAnimationMeshComponent::LoadFile(const char* FullPath)
{
	FILE* File;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	LoadOnly(File);

	CSceneComponent::Load(File);

	fclose(File);

	return true;
}

bool CAnimationMeshComponent::LoadOnly(FILE* File)
{
	char	MeshName[256] = {};

	int	Length = 0;

	fread(&Length, sizeof(int), 1, File);
	fread(MeshName, sizeof(char), Length, File);

	size_t	 SeqFileNameLength;
	fread(&SeqFileNameLength, sizeof(size_t), 1, File);

	char SeqFileName[MAX_PATH] = {};
	fread(SeqFileName, sizeof(char), SeqFileNameLength, File);

	// Animation File Name �̿��ؼ� anim File �� Load �� ���̴�.
	char AnimSavedFileName[MAX_PATH] = {};
	size_t	 AnimSavedFileNameLength = 0;
	fread(&AnimSavedFileNameLength, sizeof(size_t), 1, File);
	fread(AnimSavedFileName, sizeof(char), AnimSavedFileNameLength, File);

	// ���⼭ Ȥ�� �𸣴� �ش� Mesh , Bne, Texture File ���� �����صд�
	// Mesh Name �� ���� File Name �� ��ġ�ϴ� �����̴�.
	// const std::pair<bool, std::string>& result = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(SeqFileName);
	// 
	// if (!result.first)
	// 	return false;

	// Animation Instance File �� ���� ����
	LoadAnimationInstance<CAnimationSequenceInstance>();

	// RESOURCE_ANIMATION_PATH �� ���ؼ� .anim File Load
	if (!m_Animation->LoadAnimation(AnimSavedFileName))
		return false;

	const std::pair<bool, std::string>& result = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

	if (!result.first)
		return false;

	// result.second ���� ������ �������� Mesh �̸��� ����ִ�.
	SetMesh(result.second);

	// Animation �� �ʿ��� ���� Start �Լ��� ȣ���Ͽ� ����
	m_Animation->Start();

	// ���ο� Test Code
	int	MaterialSlotCount = -999;

	fread(&MaterialSlotCount, sizeof(int), 1, File);

	const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_ANIMCOMPONENT_PATH);

	m_vecMaterialSlot.clear();

	m_vecMaterialSlot.resize(MaterialSlotCount);

	for (int idx = 0; idx < MaterialSlotCount; ++idx)
	{
		m_vecMaterialSlot[idx] = new CMaterial;

		// Material �� Bin/Material Path �� �����ϱ�
		char MaterialBinPathMutlibyte[MAX_PATH] = {};

		if (MaterialPath)
			strcpy_s(MaterialBinPathMutlibyte, MaterialPath->PathMultibyte);

		// Mesh Name + _Idx 
		char SavedExtraName[MAX_PATH] = {};

		// Animation Mesh Component �� ���� Load �ؼ� ������� Material �� Name �� �����ϱ�
		int MtrlNameLength = -999;

		fread(&MtrlNameLength, sizeof(int), 1, File);

		fread(SavedExtraName, sizeof(char), MtrlNameLength, File);

		strcat_s(MaterialBinPathMutlibyte, SavedExtraName);

		m_vecMaterialSlot[idx]->LoadFullPath(MaterialBinPathMutlibyte);
	}

	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	CGraphicShader* NoInstancingShader = nullptr;
	CGraphicShader* InstancingShader = nullptr;
	ShaderParams ShaderParams = {};

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				for (int i = 0; i < MaterialSlotCount; ++i)
				{
					NoInstancingShader = m_vecMaterialSlot[i]->GetShader();
					InstancingShader = CResourceManager::GetInst()->FindInstancingShader(NoInstancingShader);
					ShaderParams = m_vecMaterialSlot[i]->GetShaderParams();

					(*iter)->vecInstancingShader[i] = InstancingShader;
					(*iter)->vecShaderParams[i] = ShaderParams;
				}
			}
		}
	}

	return true;
}

bool CAnimationMeshComponent::SaveOnly(FILE* pFile)
{
	std::string	MeshName = m_Mesh->GetName();

	int	Length = (int)MeshName.length();

	fwrite(&Length, sizeof(int), 1, pFile);
	fwrite(MeshName.c_str(), sizeof(char), Length, pFile);

	// Animation Sqc ���� ���� Loading �Լ�
	// size_t AnimationSequenceLength = 
	size_t	 SeqFileNameLength = strlen(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFileName());
	fwrite(&SeqFileNameLength, sizeof(size_t), 1, pFile);
	fwrite(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFileName(), sizeof(char), SeqFileNameLength, pFile);

	// Animation File Name �̿��ؼ� anim File �� Load �� ���̴�.
	size_t	 AnimSavedFileNameLength = strlen(m_Animation->GetSavedFileName());
	fwrite(&AnimSavedFileNameLength, sizeof(size_t), 1, pFile);
	fwrite(m_Animation->GetSavedFileName(), sizeof(char), AnimSavedFileNameLength, pFile);

	// ���ο� Test Code
	int	MaterialSlotCount = (int)m_vecMaterialSlot.size();

	fwrite(&MaterialSlotCount, sizeof(int), 1, pFile);

	// const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_ANIMCOMPONENT_PATH);
	const PathInfo* MaterialPath = CPathManager::GetInst()->FindPath(MATERIAL_PATH);

	for (int idx = 0; idx < MaterialSlotCount; ++idx)
	{
		// Material �� Bin/Material Path �� �����ϱ�
		char MaterialBinPathMutlibyte[MAX_PATH] = {};

		if (MaterialPath)
			strcpy_s(MaterialBinPathMutlibyte, MaterialPath->PathMultibyte);

		// ObjectName_ + Mesh Name + _Idx
		// �տ� Object �� �ٿ��ִ� ������ ���� Object �� ������ Mesh ���Ͽ��� ��Ե�, Material �� ����� ���
		// A Object �� Material �� �����ߴµ�, B Object �� ���� Material ������ A Object �� ���� Material ���ϰ� ���Ƽ�
		// A Object �� Material ��ȭ�� B Object �� Material ���Ͽ��� �ݿ��Ǵ� ���̴�.
		std::string SavedExtraMtrlName;
		SavedExtraMtrlName.reserve(MeshName.length() * 3);
		SavedExtraMtrlName = m_Object->GetName() + "_" + MeshName + "_" + std::to_string(idx) + ".mtrl";

		strcat_s(MaterialBinPathMutlibyte, SavedExtraMtrlName.c_str());

		// Animation Mesh Component �� ���� Load �ؼ� ������� Material �� Name �� �����ϱ�
		int MtrlNameLength = (int)SavedExtraMtrlName.length();

		fwrite(&MtrlNameLength, sizeof(int), 1, pFile);

		fwrite(SavedExtraMtrlName.c_str(), sizeof(char), MtrlNameLength, pFile);

		m_vecMaterialSlot[idx]->SaveFullPath(MaterialBinPathMutlibyte);
	}

	fclose(pFile);

	return false;
}

void CAnimationMeshComponent::RenderAnimationEditor()
{
	CSceneComponent::RenderAnimationEditor();

	if (!m_Mesh)
		return;

	if (m_Animation)
		m_Animation->SetShader();

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

		m_vecMaterialSlot[i]->Render();

		m_Mesh->Render((int)i);

		m_vecMaterialSlot[i]->Reset();
	}

	if (m_Animation)
		m_Animation->ResetShader();
}

void CAnimationMeshComponent::DeleteAnimationInstance()
{
	SAFE_DELETE(m_Animation);
}

void CAnimationMeshComponent::ChangeInstancingLayer()
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	bool CheckCountExist = false;

	bool AddOnNewLayer = false;
	bool DeleteOnOldLayer = false;

	for (; iter != iterEnd;)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			// ���� ���̾��� ��� �߰��Ѵ�.
			if ((*iter)->LayerName == m_LayerName)
			{
				CheckCountExist = true;

				(*iter)->InstancingList.push_back(this);

				auto iterInst = (*iter)->InstancingList.begin();
				auto iterInstEnd = (*iter)->InstancingList.end();

				// �� ������Ʈ�� �߰��ϸ鼭 �ν��Ͻ����� ����ϰ� �Ǵ� ���
				if ((*iter)->InstancingList.size() == 10)
				{
					for (; iterInst != iterInstEnd; ++iterInst)
					{
						(*iterInst)->SetInstancing(true);
					}
				}
				// �̹� �ν��Ͻ� ������� ���
				else if ((*iter)->InstancingList.size() > 10)
				{
					SetInstancing(true);
				}
				// �ν��Ͻ� ���� �ʴ� ���
				else
				{
					SetInstancing(false);
				}

				AddOnNewLayer = true;
			}

			// Layer�� �ٸ� ��� �ش� ���̾�� ����
			else
			{
				// �� ������Ʈ�� �����鼭 �ν��Ͻ��� ���� �ʰ� ó���ؾ� �ϴ� ���
				bool InstancingOff = (*iter)->InstancingList.size() == 10;

				auto iterInst = (*iter)->InstancingList.begin();
				auto iterInstEnd = (*iter)->InstancingList.end();

				if (InstancingOff)
				{
					for (; iterInst != iterInstEnd;)
					{
						(*iterInst)->SetInstancing(false);

						if ((*iterInst) == this)
						{
							iterInst = (*iter)->InstancingList.erase(iterInst);
							continue;
						}

						++iterInst;
					}
				}
				else
				{
					for (; iterInst != iterInstEnd; ++iterInst)
					{
						if ((*iterInst) == this)
						{
							// ���� ���̾��� �ν��Ͻ� ����Ʈ���� ����
							(*iter)->InstancingList.erase(iterInst);
							break;
						}
					}
				}

				DeleteOnOldLayer = true;

				if ((*iter)->InstancingList.empty())
				{
					SAFE_DELETE(*iter);
					iter = m_InstancingCheckList.erase(iter);
					continue;
				}
			}
		}

		// �� ���̾ �ְ�, ���� ���̾�� ���� �۾� �Ϸ��� ��� ���� ����
		if (AddOnNewLayer && DeleteOnOldLayer)
		{
			break;
		}

		++iter;
	}

	// �� ������Ʈ�� ���� ���̾ ó�� �߰��Ǵ� ���
	if (!CheckCountExist)
	{
		OnCreateNewInstancingCheckCount();
	}
}

void CAnimationMeshComponent::OnChangeMaterialShader(int MatIndex, CGraphicShader* NewShader)
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				// Instancing Shader ��ü
				CGraphicShader* NewInstancingShader = CResourceManager::GetInst()->FindInstancingShader(NewShader);
				(*iter)->vecInstancingShader[MatIndex] = NewInstancingShader;
				break;
			}
		}
	}
}

void CAnimationMeshComponent::OnChangeMaterialShaderParams(int MatIndex, const ShaderParams& Params)
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				// Instancing Shader ��ü
				(*iter)->vecShaderParams[MatIndex] = Params;
				break;
			}
		}
	}
}

void CAnimationMeshComponent::OnCreateNewInstancingCheckCount()
{
	InstancingCheckCount* CheckCount = new InstancingCheckCount;

	m_InstancingCheckList.push_back(CheckCount);

	CheckCount->InstancingList.push_back(this);
	CheckCount->Mesh = m_Mesh;
	CheckCount->LayerName = m_LayerName;

	// Material���� � Instanicng Shader�� �����ؾ� �ϴ��� �޾ƿ´�.
	size_t SlotSize = m_Mesh->GetMaterialSlotCount();
	
	CGraphicShader* NoInstancingShader = nullptr;
	CGraphicShader* InstancingShader = nullptr;
	CMaterial* Mat = nullptr;
	ShaderParams MatShaderParams = {};

	CheckCount->vecInstancingShader.resize(SlotSize);
	CheckCount->vecShaderParams.resize(SlotSize);

	for (size_t i = 0; i < SlotSize; ++i)
	{
		// Material�� Instancing Shader, Shader Paramerter ���� ����
		Mat = m_vecMaterialSlot[i];
		NoInstancingShader = Mat->GetShader();
		InstancingShader = CResourceManager::GetInst()->FindInstancingShader(NoInstancingShader);
		MatShaderParams = Mat->GetShaderParams();

		CheckCount->vecInstancingShader[i] = InstancingShader;
		CheckCount->vecShaderParams[i] = MatShaderParams;
	}

	SetInstancing(false);
}

void CAnimationMeshComponent::AddChild(CSceneComponent* Child,
	const std::string& SocketName)
{
	CSceneComponent::AddChild(Child, SocketName);

	if (m_Skeleton && SocketName != "")
	{
		m_Socket = m_Skeleton->GetSocket(SocketName);

		Child->GetTransform()->SetSocket(m_Socket);
	}
}

void CAnimationMeshComponent::AddChild(CGameObject* Child,
	const std::string& SocketName)
{
	CSceneComponent::AddChild(Child, SocketName);

	if (m_Skeleton && SocketName != "")
	{
		m_Socket = m_Skeleton->GetSocket(SocketName);

		CSceneComponent* ChildComponent = Child->GetRootComponent();

		ChildComponent->GetTransform()->SetSocket(m_Socket);
	}
}
