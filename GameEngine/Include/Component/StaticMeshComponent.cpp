
#include "StaticMeshComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Render/RenderManager.h"
#include "../Resource/Shader/Standard2DConstantBuffer.h"
#include "../EngineUtil.h"

CStaticMeshComponent::CStaticMeshComponent()
{
	SetTypeID<CStaticMeshComponent>();
	m_Render = true;
	m_InstanceID = 0;
	//m_Transform->SetTransformState(Transform_State::Ground);
}

CStaticMeshComponent::CStaticMeshComponent(const CStaticMeshComponent& com) :
	CSceneComponent(com)
{
	m_Mesh = com.m_Mesh;

	m_vecMaterialSlot.clear();

	for (size_t i = 0; i < com.m_vecMaterialSlot.size(); ++i)
	{
		m_vecMaterialSlot.push_back(com.m_vecMaterialSlot[i]->Clone());
	}
}

CStaticMeshComponent::~CStaticMeshComponent()
{
	DeleteInstancingCheckList();
}

bool CStaticMeshComponent::IsTransparent() const
{
	if (m_vecMaterialSlot.size() == 0)
	{
		return false;
	}

	// 하나의 Material이라도 Transparent라면 모든 Material이 Transparent Layer에 있음
	return m_vecMaterialSlot[0]->IsTransparent();
}

void CStaticMeshComponent::SetMesh(const std::string& Name)
{
	if(m_Scene)
		m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(Name);

	if (!m_Mesh)
	{
		m_Mesh = (CStaticMesh*)CResourceManager::GetInst()->FindMesh(Name);

		if(!m_Mesh)
			return;
	}

	SetMesh(m_Mesh);
}

void CStaticMeshComponent::SetMesh(CStaticMesh* Mesh)
{
	m_Mesh = Mesh;

	m_vecMaterialSlot.clear();

	DeleteInstancingCheckList();

	const std::vector<CSharedPtr<CMaterial>>* pMaterialSlots =
		m_Mesh->GetMaterialSlots();

	std::vector<CSharedPtr<CMaterial>>::const_iterator	iter = pMaterialSlots->begin();
	std::vector<CSharedPtr<CMaterial>>::const_iterator	iterEnd = pMaterialSlots->end();

	for (; iter != iterEnd; ++iter)
	{
		m_vecMaterialSlot.push_back((*iter)->Clone());

		(*iter)->SetScene(m_Scene);
	}

	Vector3 Max = m_Mesh->GetMax();
	Vector3 Min = m_Mesh->GetMin();
	SetMeshSize(Max - Min);
	m_SphereInfo.Center = (Max + Min) / 2.f;

	auto	iter1 = m_InstancingCheckList.begin();
	auto	iter1End = m_InstancingCheckList.end();

	bool	Add = false;

	for (; iter1 != iter1End; ++iter1)
	{
		if ((*iter1)->Mesh == m_Mesh)
		{
			// 반투명 상태일 경우 다른 레이어의 InstancingCheckList로 생성되어야 한다.
			if (m_LayerName != (*iter1)->LayerName)
			{
				continue;
			}

			bool	InstancingEnable = (*iter1)->InstancingList.back()->GetInstancing();

			(*iter1)->InstancingList.push_back(this);
			Add = true;

			// 인스턴싱 개수를 판단한다.
			if (InstancingEnable)
				SetInstancing(InstancingEnable);

			else
			{
				if ((*iter1)->InstancingList.size() == 10)
				{
					auto	iter2 = (*iter1)->InstancingList.begin();
					auto	iter2End = (*iter1)->InstancingList.end();

					for (; iter2 != iter2End; ++iter2)
					{
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
		OnCreateNewInstancingCheckCount();
	}
}

void CStaticMeshComponent::SetMaterial(CMaterial* Material, int Index)
{
	m_vecMaterialSlot[Index] = Material->Clone();

	m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CStaticMeshComponent::AddMaterial(CMaterial* Material)
{
	m_vecMaterialSlot.push_back(Material->Clone());

	int Index = (int)m_vecMaterialSlot.size() - 1;
	m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CStaticMeshComponent::ClearMaterial()
{
	m_vecMaterialSlot.clear();

	auto	iter1 = m_InstancingCheckList.begin();
	auto	iter1End = m_InstancingCheckList.end();

	for (; iter1 != iter1End; ++iter1)
	{
		if ((*iter1)->Mesh == m_Mesh)
		{
			// 반투명 상태일 경우 다른 레이어의 InstancingCheckList로 생성되어야 한다.
			if (m_LayerName != (*iter1)->LayerName)
			{
				continue;
			}

			(*iter1)->vecInstancingShader.clear();
			(*iter1)->vecShaderParams.clear();
		}
	}
}

void CStaticMeshComponent::SetMaterialShaderAll(const std::string& Name)
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

void CStaticMeshComponent::SetMaterialShader(int MatIndex, const std::string& ShaderName)
{
	CGraphicShader* Shader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader(ShaderName));

	SetMaterialShader(MatIndex, Shader);
}

void CStaticMeshComponent::SetMaterialShader(int MatIndex, CGraphicShader* Shader)
{
	if (!Shader)
	{
		return;
	}

	m_vecMaterialSlot[MatIndex]->SetShader(Shader);

	// Instancing Shader 변경
	OnChangeMaterialShader(MatIndex, Shader);
}

void CStaticMeshComponent::SetMaterialShaderParams(int MatIndex, const ShaderParams& Params)
{
	m_vecMaterialSlot[MatIndex]->SetShaderParams(Params);

	// Instancing Shader Parameter 변경
	OnChangeMaterialShaderParams(MatIndex, Params);
}

void CStaticMeshComponent::SetBaseColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetBaseColor(Color);
}

void CStaticMeshComponent::SetBaseColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetBaseColor(r, g, b, a);
}

void CStaticMeshComponent::SetAmbientColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetAmbientColor(Color);
}

void CStaticMeshComponent::SetAmbientColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetAmbientColor(r, g, b, a);
}

void CStaticMeshComponent::SetSpecularColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularColor(Color);
}

void CStaticMeshComponent::SetSpecularColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularColor(r, g, b, a);
}

void CStaticMeshComponent::SetEmissiveColor(const Vector4& Color, int Index)
{
	m_vecMaterialSlot[Index]->SetEmissiveColor(Color);
}

void CStaticMeshComponent::SetEmissiveColor(float r, float g, float b, float a, int Index)
{
	m_vecMaterialSlot[Index]->SetEmissiveColor(r, g, b, a);
}

void CStaticMeshComponent::SetSpecularPower(float Power, int Index)
{
	m_vecMaterialSlot[Index]->SetSpecularPower(Power);
}

void CStaticMeshComponent::SetRenderState(CRenderState* State, int Index)
{
	m_vecMaterialSlot[Index]->SetRenderState(State);
}

void CStaticMeshComponent::SetRenderState(const std::string& Name, int Index)
{
	m_vecMaterialSlot[Index]->SetRenderState(Name);
}

void CStaticMeshComponent::SetTransparencyAllMaterial(bool Enable)
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

		// 모든 Material의 Shader 교체
		SetMaterialShaderAll("Transparent3DShader");

		// 인스턴싱 레이어를 바꾼다.
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
			// 이전 쉐이더로 되돌림
			m_vecMaterialSlot[i]->SetTransparency(false);
			m_vecMaterialSlot[i]->RevertShader();
		}

		// 인스턴싱 레이어를 바꾼다.
		if (!AlreadyOpaque)
		{
			ChangeInstancingLayer();
		}
	}
}

void CStaticMeshComponent::SetOpacity(float Opacity, int Index)
{
	m_vecMaterialSlot[Index]->SetOpacity(Opacity);
}

void CStaticMeshComponent::AddOpacity(float Opacity, int Index)
{
	m_vecMaterialSlot[Index]->AddOpacity(Opacity);
}

void CStaticMeshComponent::SetMetallic(bool Metallic, int Index)
{
	m_vecMaterialSlot[Index]->SetMetallic(Metallic);
}

void CStaticMeshComponent::SetUVScale(const Vector2& UVScale, int Index)
{
	m_vecMaterialSlot[Index]->SetUVScale(UVScale);
}

void CStaticMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, Texture);
}

void CStaticMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, FileName, PathName);
}

void CStaticMeshComponent::AddTextureFullPath(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath)
{
	m_vecMaterialSlot[MaterialIndex]->AddTextureFullPath(Register, ShaderType, Name, FullPath);
}

void CStaticMeshComponent::AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->AddTexture(Register, ShaderType, Name, vecFileName, PathName);
}

void CStaticMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, Texture);
}

void CStaticMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, FileName, PathName);
}

void CStaticMeshComponent::SetTextureFullPath(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name,
	const TCHAR* FullPath)
{
	m_vecMaterialSlot[MaterialIndex]->SetTextureFullPath(Index, Register, ShaderType, Name, FullPath);
}

void CStaticMeshComponent::SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	m_vecMaterialSlot[MaterialIndex]->SetTexture(Index, Register, ShaderType, Name, vecFileName, PathName);
}

void CStaticMeshComponent::Start()
{
	CSceneComponent::Start();
}

bool CStaticMeshComponent::Init()
{
	//m_Mesh = (CSpriteMesh*)m_Scene->GetResource()->FindMesh("SpriteMesh");
	AddMaterial(m_Scene->GetResource()->FindMaterial("Color"));

	return true;
}

void CStaticMeshComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CStaticMeshComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CStaticMeshComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CStaticMeshComponent::Render()
{
	CSceneComponent::Render();

	if (!m_Mesh)
		return;

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

		m_vecMaterialSlot[i]->Render();

		m_Mesh->Render((int)i);

		m_vecMaterialSlot[i]->Reset();
	}
}

void CStaticMeshComponent::RenderShadowMap()
{
	CSceneComponent::RenderShadowMap();

	if (!m_Mesh)
	{
		return;
	}

	size_t Size = m_vecMaterialSlot.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->UpdateCBuffer();
		m_Mesh->Render((int)i);
	}
}

void CStaticMeshComponent::RenderParticleEffectEditor()
{
	CSceneComponent::RenderParticleEffectEditor();

	if (!m_Mesh)
		return;

	size_t	Size = m_vecMaterialSlot.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

		m_vecMaterialSlot[i]->Render();

		m_Mesh->Render((int)i);

		m_vecMaterialSlot[i]->Reset();
	}
}

void CStaticMeshComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CStaticMeshComponent* CStaticMeshComponent::Clone()
{
	return new CStaticMeshComponent(*this);
}

bool CStaticMeshComponent::Save(FILE* File)
{
	std::string	MeshName = m_Mesh->GetName();

	int	Length = (int)MeshName.length();

	fwrite(&Length, sizeof(int), 1, File);
	fwrite(MeshName.c_str(), sizeof(char), Length, File);

	// Mesh Path 존재하는지
	bool FullPathExist = false;
	char MeshPath[MAX_PATH] = {};
	strcpy_s(MeshPath, m_Mesh->GetFullPath());

	Length = (int)strlen(MeshPath);

	FullPathExist = Length > 0 ? true : false;
	fwrite(&FullPathExist, sizeof(bool), 1, File);

	// Mesh Path 존재하는 경우 저장
	if (FullPathExist)
	{
		fwrite(&Length, sizeof(int), 1, File);
		fwrite(MeshPath, sizeof(char), Length, File);
	}

	int	MaterialSlotCount = (int)m_vecMaterialSlot.size();

	fwrite(&MaterialSlotCount, sizeof(int), 1, File);

	for (int i = 0; i < MaterialSlotCount; ++i)
	{
		m_vecMaterialSlot[i]->Save(File);
	}

	CSceneComponent::Save(File);

	return true;
}

bool CStaticMeshComponent::Load(FILE* File)
{
	char	MeshName[256] = {};

	int	Length = 0;

	fread(&Length, sizeof(int), 1, File);
	fread(MeshName, sizeof(char), Length, File);

	bool MeshFullPathExist = false;
	fread(&MeshFullPathExist, sizeof(bool), 1, File);

	// Mesh FullPath가 존재하면 로드
	if (MeshFullPathExist)
	{
		char MeshFullPath[MAX_PATH] = {};
		fread(&Length, sizeof(int), 1, File);
		fread(&MeshFullPath, sizeof(char), Length, File);

		if(m_Scene)
			m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, MeshName, MeshFullPath);
		else
			CResourceManager::GetInst()->LoadMeshFullPathMultibyte(Mesh_Type::Static, MeshName, MeshFullPath);

		CMesh* LoadCheck = nullptr;
		if(m_Scene)
			LoadCheck = m_Scene->GetResource()->FindMesh(MeshName);
		else
			LoadCheck = CResourceManager::GetInst()->FindMesh(MeshName);

		if (!LoadCheck)
		{
			std::string FileName = CEngineUtil::ExtractFilePathFromFullPath(MeshFullPath, MESH_PATH);

			char FileNameMB[MAX_PATH] = {};
			TCHAR FileNameTCHAR[MAX_PATH] = {};
			strcpy_s(FileNameMB, FileName.c_str());

			MultiByteToWideChar(CP_ACP, 0, FileNameMB, (int)FileName.length(), FileNameTCHAR, (int)FileName.length());

			if(m_Scene)
				m_Scene->GetResource()->LoadMesh(Mesh_Type::Static, MeshName, FileNameTCHAR, MESH_PATH);
			else
				CResourceManager::GetInst()->LoadMesh(Mesh_Type::Static, MeshName, FileNameTCHAR, MESH_PATH);
		}
	}

	SetMesh(MeshName);

	int	MaterialSlotCount = 0;

	fread(&MaterialSlotCount, sizeof(int), 1, File);

	m_vecMaterialSlot.clear();

	m_vecMaterialSlot.resize(MaterialSlotCount);

	for (int i = 0; i < MaterialSlotCount; ++i)
	{
		m_vecMaterialSlot[i] = new CMaterial;

		m_vecMaterialSlot[i]->Load(File);
	}

	// 현재 Mesh의 기본 Material 기준으로 Instancing Shader 및 Parameter가 Instancing Check List에 생성되어 있기 때문에
	// 로드한 고유한 Material 정보에 따라 Instancing CheckList를 갱신해주어야 함
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

void CStaticMeshComponent::ChangeInstancingLayer()
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
			// 같은 레이어일 경우 추가한다.
			if ((*iter)->LayerName == m_LayerName)
			{
				CheckCountExist = true;

				(*iter)->InstancingList.push_back(this);

				auto iterInst = (*iter)->InstancingList.begin();
				auto iterInstEnd = (*iter)->InstancingList.end();

				// 이 컴포넌트를 추가하면서 인스턴싱으로 출력하게 되는 경우
				if ((*iter)->InstancingList.size() == 10)
				{
					for (; iterInst != iterInstEnd; ++iterInst)
					{
						(*iterInst)->SetInstancing(true);
					}
				}
				// 이미 인스턴싱 출력중인 경우
				else if ((*iter)->InstancingList.size() > 10)
				{
					SetInstancing(true);
				}
				// 인스턴싱 하지 않는 경우
				else
				{
					SetInstancing(false);
				}

				AddOnNewLayer = true;
			}

			// Layer가 다를 경우 해당 레이어에서 뺀다
			else
			{
				// 이 컴포넌트가 빠지면서 인스턴싱을 하지 않게 처리해야 하는 경우
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
							// 현재 레이어의 인스턴싱 리스트에서 제거
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

		// 새 레이어에 넣고, 이전 레이어에서 빼는 작업 완료한 경우 루프 종료
		if (AddOnNewLayer && DeleteOnOldLayer)
		{
			break;
		}

		++iter;
	}

	// 이 컴포넌트가 속한 레이어에 처음 추가되는 경우
	if (!CheckCountExist)
	{
		OnCreateNewInstancingCheckCount();
	}
}

bool CStaticMeshComponent::DeleteInstancingCheckList()
{
	bool Deleted = false;

	auto	iter = m_InstancingCheckList.begin();
	auto	iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			// 반투명 상태일 경우 다른 레이어의 InstancingCheckList로 생성되어야 한다.
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
							((CStaticMeshComponent*)(*iter2))->SetInstanceID(0);
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
							((CStaticMeshComponent*)(*iter2))->SetInstanceID(ID);
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

void CStaticMeshComponent::OnChangeMaterialShader(int MatIndex, CGraphicShader* NewShader)
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				// Instancing Shader 교체
				CGraphicShader* NewInstancingShader = CResourceManager::GetInst()->FindInstancingShader(NewShader);

				if (NewInstancingShader)
				{
					(*iter)->vecInstancingShader[MatIndex] = NewInstancingShader;
				}
				break;
			}
		}
	}
}

void CStaticMeshComponent::OnChangeMaterialShaderParams(int MatIndex, const ShaderParams& Params)
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_Mesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				(*iter)->vecShaderParams[MatIndex] = Params;
				break;
			}
		}
	}
}

void CStaticMeshComponent::OnCreateNewInstancingCheckCount()
{
	InstancingCheckCount* CheckCount = new InstancingCheckCount;

	m_InstancingCheckList.push_back(CheckCount);

	CheckCount->InstancingList.push_back(this);
	CheckCount->Mesh = m_Mesh;
	CheckCount->LayerName = m_LayerName;

	// Material별로 어떤 Instanicng Shader로 렌더해야 하는지 받아온다.
	size_t SlotSize = m_Mesh->GetMaterialSlotCount();
	
	CGraphicShader* NoInstancingShader = nullptr;
	CGraphicShader* InstancingShader = nullptr;
	CMaterial* Mat = nullptr;
	ShaderParams MatShaderParams = {};

	CheckCount->vecInstancingShader.resize(SlotSize);
	CheckCount->vecShaderParams.resize(SlotSize);

	for (size_t i = 0; i < SlotSize; ++i)
	{
		// Material별 Instancing Shader, Shader Paramerter 정보 저장
		Mat = m_Mesh->GetMaterial((int)i);
		NoInstancingShader = Mat->GetShader();
		InstancingShader = CResourceManager::GetInst()->FindInstancingShader(NoInstancingShader);
		MatShaderParams = Mat->GetShaderParams();

		CheckCount->vecInstancingShader[i] = InstancingShader;
		CheckCount->vecShaderParams[i] = MatShaderParams;
	}

	SetInstancing(false);
}
