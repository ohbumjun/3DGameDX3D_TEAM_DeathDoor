
#include "SceneComponent.h"
#include "../Render/RenderManager.h"
#include "../GameObject/GameObject.h"
#include "../Resource/Shader/Standard2DConstantBuffer.h"
#include "../Scene/SceneManager.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"

std::list<InstancingCheckCount*> CSceneComponent::m_InstancingCheckList;

CSceneComponent::CSceneComponent() :
	m_Culling(false)
{
	SetTypeID<CSceneComponent>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = false;

	m_Instancing = false;

	m_ReceiveDecal = true;

	m_Transform = new CTransform;

	m_Transform->m_Owner = this;
	m_Transform->Init();

	m_Parent = nullptr;

	m_DrawShadow = true;

	m_LayerName = "Default";

	m_Culling = false;
}

CSceneComponent::CSceneComponent(const CSceneComponent& com) :
	CComponent(com)
{
	*this = com;
	m_RefCount = 0;

	m_Transform = com.m_Transform->Clone();

	m_Transform->m_Parent = nullptr;
	m_Transform->m_vecChild.clear();

	m_Instancing = false;

	m_Transform->m_Owner = this;

	m_Parent = nullptr;

	m_Culling = false;

	m_vecChild.clear();

	size_t	Size = com.m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		CSceneComponent* CloneComponent = com.m_vecChild[i]->Clone();

		CloneComponent->m_Parent = this;

		m_vecChild.push_back(CloneComponent);

		CloneComponent->m_Transform->m_Parent = m_Transform;

		m_Transform->m_vecChild.push_back(CloneComponent->m_Transform);
	}
}

CSceneComponent::~CSceneComponent()
{
	if (m_OnDestroyCallBack)
	{
		m_OnDestroyCallBack(this);
	}

	SAFE_DELETE(m_Transform);
}

void CSceneComponent::SetInstancingInfo(Instancing3DInfo* Info)
{
	m_Transform->SetInstancingInfo(Info);
}

void CSceneComponent::SetInstancingShadowInfo(Instancing3DInfo* Info)
{
	m_Transform->SetInstancingShadowInfo(Info);
}

SphereInfo CSceneComponent::GetSphereInfoViewSpace() const
{
	SphereInfo Info;

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Info.Center = m_SphereInfo.Center * GetWorldScale() + GetWorldPos();
	
	// ���⼭ Radius �� ���ʺ��� ���� World ���� �󿡼��� Radius ũ�⸦ ������ ���̴�.
	Info.Radius = m_SphereInfo.Radius;

	Info.Center = Info.Center.TransformCoord(Camera->GetViewMatrix());

	return Info;
}

float CSceneComponent::GetViewZ() const
{
	Matrix matView = m_Scene->GetCameraManager()->GetCurrentCamera()->GetViewMatrix();
	float viewZ = GetWorldPos().TransformCoord(matView).z - m_SphereInfo.Radius;
	return viewZ;
}

void CSceneComponent::Enable(bool Enable)
{
	m_Render = Enable;

	m_Enable = Enable;

	size_t Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->Enable(Enable);
	}
}

void CSceneComponent::SetSceneComponent(CGameObject* Object)
{
	Object->AddSceneComponent(this);

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->SetSceneComponent(Object);
	}
}

void CSceneComponent::GetAllSceneComponentsName(std::vector<FindComponentName>& vecNames)
{
	FindComponentName	Name;

	Name.Name = m_Name;

	if (m_Parent)
		Name.ParentName = m_Parent->GetName();

	vecNames.push_back(Name);

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->GetAllSceneComponentsName(vecNames);
	}
}

void CSceneComponent::GetAllSceneComponentsPointer(std::vector<CSceneComponent*>& OutVecSceneComp)
{
	OutVecSceneComp.push_back(this);

	size_t Size = m_vecChild.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->GetAllSceneComponentsPointer(OutVecSceneComp);
	}
}

void CSceneComponent::Destroy()
{
	CComponent::Destroy();

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->Destroy();
	}
}

void CSceneComponent::DestroySingle()
{
	CComponent::Destroy();
}

void CSceneComponent::SetScene(CScene* Scene)
{
	CComponent::SetScene(Scene);

	m_Transform->m_Scene = Scene;

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->SetScene(Scene);
	}
}

void CSceneComponent::SetGameObject(CGameObject* Object)
{
	CComponent::SetGameObject(Object);

	m_Transform->m_Object = Object;

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->SetGameObject(Object);
	}
}

void CSceneComponent::AddChild(CSceneComponent* Child, const std::string& SocketName)
{
	Child->m_Parent = this;

	m_vecChild.push_back(Child);

	Child->m_Transform->m_Parent = m_Transform;

	m_Transform->m_vecChild.push_back(Child->m_Transform);

	// �ִϸ��̼� �޽� ������Ʈ�� ��쿡�� Socket�� ó���Ѵ�.
}

void CSceneComponent::AddChild(CGameObject* Child, const std::string& SocketName)
{
	CSceneComponent* ChildComponent = Child->GetRootComponent();

	ChildComponent->m_Parent = this;

	m_vecChild.push_back(ChildComponent);

	ChildComponent->m_Transform->m_Parent = m_Transform;

	m_Transform->m_vecChild.push_back(ChildComponent->m_Transform);
}

bool CSceneComponent::DeleteChild(CSceneComponent* Child)
{
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecChild[i] == Child)
		{
			auto	iter = m_vecChild.begin() + i;

			m_vecChild.erase(iter);

			auto	iterTr = m_Transform->m_vecChild.begin() + i;

			m_Transform->m_vecChild.erase(iterTr);
			return true;
		}

		if (m_vecChild[i]->DeleteChild(Child))
			return true;
	}

	return false;
}

bool CSceneComponent::DeleteChild(const std::string& Name)
{
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecChild[i]->GetName() == Name)
		{
			auto	iter = m_vecChild.begin() + i;

			m_vecChild.erase(iter);

			auto	iterTr = m_Transform->m_vecChild.begin() + i;

			m_Transform->m_vecChild.erase(iterTr);
			return true;
		}

		if (m_vecChild[i]->DeleteChild(Name))
			return true;
	}

	return false;
}

bool CSceneComponent::ReplaceComponent(CSceneComponent* DestNode)
{
	// 1)  Parent Component ���� ��ü
	CSceneComponent* srcParent = m_Parent;
	CSceneComponent* destParent = DestNode->m_Parent;
	CTransform* srcTransformParent = m_Transform->m_Parent;
	CTransform* destTransformParent = DestNode->m_Transform->m_Parent;

	m_Parent = destParent;
	DestNode->m_Parent = srcParent;

	m_Transform->m_Parent = destTransformParent;
	DestNode->m_Transform->m_Parent = srcTransformParent;

	if (!srcParent)
	{
		// Parent �� ���ٸ� ���� ��尡 Root Node -> Root Component �� ����
		m_Object->SetRootComponent(DestNode);
	}

	// 2) �ڽ� Component, Transform ���� ��ü 
	std::vector<CSceneComponent*> vecSrcChilds = {};
	std::vector<CSceneComponent*> vecDestChilds = {};
	std::vector<CTransform*> vecSrcTransformChilds = {};
	std::vector<CTransform*> vecDestTransformChilds = {};

	// Src Child �� ��Ƶα�
	size_t ChildSize = m_vecChild.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		vecSrcChilds.push_back(m_vecChild[i]);
		vecSrcTransformChilds.push_back(m_vecChild[i]->m_Transform);
	}

	// Dest Child �� ��Ƶα�
	ChildSize = DestNode->m_vecChild.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		vecDestChilds.push_back(DestNode->m_vecChild[i]);
		vecDestTransformChilds.push_back(DestNode->m_vecChild[i]->m_Transform);
	}

	// DestNode �� Child �� ���� Child �� �ֱ�
	m_vecChild.clear();
	m_Transform->m_vecChild.clear();

	ChildSize = vecDestChilds.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		vecDestChilds[i]->m_Parent = this;
		m_vecChild.push_back(vecDestChilds[i]);

		vecDestChilds[i]->m_Transform->m_Parent = m_Transform;
		m_Transform->m_vecChild.push_back(vecDestChilds[i]->m_Transform);
	}

	// SrcNode �� Child �� DestNode �� Child �� �ֱ�
	DestNode->m_vecChild.clear();
	DestNode->m_Transform->m_vecChild.clear();

	ChildSize = vecSrcChilds.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		vecSrcChilds[i]->m_Parent = DestNode;
		DestNode->m_vecChild.push_back(vecSrcChilds[i]);

		vecSrcChilds[i]->m_Transform->m_Parent = DestNode->m_Transform;
		DestNode->m_Transform->m_vecChild.push_back(vecSrcChilds[i]->m_Transform);
	}

	return false;
}

bool CSceneComponent::DeleteComponent()
{
	if (!m_Parent)
	{
		auto iter = m_Object->GetSceneComponents().begin();
		auto iterEnd = m_Object->GetSceneComponents().end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter) == this)
			{
				m_Object->DeleteSceneComponent(this);
				break;
			}
		}

		if (m_vecChild.size() > 0)
		{
			CSceneComponent* FirstChild = m_vecChild[0];
			
			auto iter = m_vecChild.begin();
			m_vecChild.erase(iter);

			m_Object->SetRootComponent(FirstChild);
			
			// ���� ������ Component�� ù��° �ڽ�(= FirstChild) ������
			// ������ Child���� FirstChild�� �ڽ����� �־��ش�
			size_t Count = m_vecChild.size();
			for (size_t i = 1; i < Count; ++i)
			{
				FirstChild->AddChild(m_vecChild[i]);
			}

			DestroySingle();
		}

		// ��Ʈ��带 ����µ� �ڽ��� �ϳ��� ���� ���
		else
		{
			Destroy();
			m_Object->SetRootComponent(nullptr);
		}
		return true;
	}

	// ����� �� ��� -> ���� ù��° �ڽ��� �ش� ��ġ�� �̵���Ų��.
	// �ڽ��� ���� ��� -> �θ𿡼� ���� SceneComponent ������ �����ϸ� �ȴ�.
	if (m_vecChild.empty())
	{
		m_Parent->DeleteChild(this);

		DestroySingle();

		return true;
	}

	// �θ� �ڽ� ���� ������
	CSceneComponent* FirstChild = m_vecChild[0];

	FirstChild->m_Parent = m_Parent;
	FirstChild->m_Transform->m_Parent = m_Parent->m_Transform;

	m_Parent->AddChild(FirstChild);
	m_Parent->m_Transform->m_vecChild.push_back(FirstChild->m_Transform);

	// ���� �ڽĵ��� FirsChild �� �ڽĵ�� �߰��ϱ�
	size_t ChildrenSize = m_vecChild.size();

	for (size_t i = 0; i < ChildrenSize; ++i)
	{
		if (m_vecChild[i] == FirstChild)
			continue;

		m_vecChild[i]->m_Parent = FirstChild;
		FirstChild->m_vecChild.push_back(m_vecChild[i]);

		m_vecChild[i]->m_Transform->m_Parent = FirstChild->m_Transform;
		FirstChild->m_Transform->m_vecChild.push_back(m_vecChild[i]->m_Transform);
	}

	DestroySingle();

	return true;
}

size_t CSceneComponent::GetChildCount() const
{
	return m_vecChild.size();
}

CSceneComponent* CSceneComponent::GetChild(int Idx) const
{
	if (Idx < 0 || Idx >= m_vecChild.size())
	{
		return nullptr;
	}

	return m_vecChild[Idx];
}

CSceneComponent* CSceneComponent::FindComponent(const std::string& Name)
{
	if (m_Name == Name)
		return this;

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		CSceneComponent* Find = m_vecChild[i]->FindComponent(Name);

		if (Find)
			return Find;
	}

	return nullptr;
}

void CSceneComponent::ClearParent()
{
	if (m_Parent)
	{
		m_Parent = nullptr;
	}

	/*
	Child->m_Parent = this;

	m_vecChild.push_back(Child);

	Child->m_Transform->m_Parent = m_Transform;

	m_Transform->m_vecChild.push_back(Child->m_Transform);
	*/
}

void CSceneComponent::Start()
{
	CComponent::Start();

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->Start();
	}
}

bool CSceneComponent::Init()
{
	return true;
}

void CSceneComponent::Update(float DeltaTime)
{
	m_Transform->Update(DeltaTime);

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		// (OBJ�� �߰�)
		if (m_vecChild[i]->IsEnable() == false)
			continue;

		m_vecChild[i]->Update(DeltaTime);
	}
}

void CSceneComponent::PostUpdate(float DeltaTime)
{
	m_Transform->PostUpdate(DeltaTime);

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		// (OBJ�� �߰�)
		if (m_vecChild[i]->IsEnable() == false)
			continue;

		m_vecChild[i]->PostUpdate(DeltaTime);
	}
}

void CSceneComponent::CheckCollision()
{
	if (m_Render)
	{
		SphereInfo Info;

		Info.Center = m_SphereInfo.Center.TransformCoord(GetWorldMatrix());

		Vector3 Radius;
		Radius.x = GetMeshSize().Length();
		Radius.y = GetMeshSize().Length();
		Radius.z = GetMeshSize().Length();

		// (OBJ ����)
		// ����
		// Radius = Radius.TransformCoord(GetWorldMatrix());
		Radius = Radius * GetWorldScale();

		Info.Radius = Radius.x > Radius.y ? Radius.x : Radius.y;
		Info.Radius = Info.Radius > Radius.z ? Info.Radius : Radius.z;
		Info.Radius /= 2.f;

		if (Info.Radius < 0.f)
			assert(false);

		m_SphereInfo.Radius = Info.Radius;

		CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

		m_Culling = Camera->FrustumInSphere(Info);
	}

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->CheckCollision();
	}
}

void CSceneComponent::PrevRender()
{
	if (m_Render && !m_Instancing)
	{
		CRenderManager::GetInst()->AddRenderList(this);
	}
 //	if (m_Render && !m_Culling && !m_Instancing)
 //	{
 //		CRenderManager::GetInst()->AddRenderList(this);
 //	}

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->PrevRender();
	}
}

void CSceneComponent::Render()
{
	m_Transform->SetTransform();

	CRenderManager::GetInst()->GetStandard2DCBuffer()->SetAnimation2DEnable(false);
	CRenderManager::GetInst()->GetStandard2DCBuffer()->UpdateCBuffer();
}

void CSceneComponent::RenderShadowMap()
{
	m_Transform->SetTransformShadow();
}

void CSceneComponent::RenderDebug()
{
	m_Transform->SetTransform();

	CRenderManager::GetInst()->GetStandard2DCBuffer()->SetAnimation2DEnable(false);
	CRenderManager::GetInst()->GetStandard2DCBuffer()->UpdateCBuffer();
}

void CSceneComponent::PostRender()
{
}

void CSceneComponent::Reset()
{
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->Reset();
	}
}

CSceneComponent* CSceneComponent::Clone()
{
	return new CSceneComponent(*this);
}

bool CSceneComponent::Save(FILE* File)
{
	CComponent::Save(File);

	fwrite(&m_Render, sizeof(bool), 1, File);

	int	Length = (int)m_LayerName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_LayerName.c_str(), sizeof(char), Length, File);

	m_Transform->Save(File);

	int	ChildCount = (int)m_vecChild.size();

	fwrite(&ChildCount, sizeof(int), 1, File);

	for (int i = 0; i < ChildCount; ++i)
	{
		size_t	TypeID = m_vecChild[i]->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_vecChild[i]->Save(File);
	}

	return true;
}

bool CSceneComponent::Load(FILE* File)
{
	CComponent::Load(File);

	fread(&m_Render, sizeof(bool), 1, File);

	int	Length = 0;
	char	LayerName[256] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(LayerName, sizeof(char), Length, File);

	m_LayerName = LayerName;

	m_Transform->Load(File);

	int	ChildCount = 0;

	fread(&ChildCount, sizeof(int), 1, File);

	for (int i = 0; i < ChildCount; ++i)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		CComponent* Component = CSceneManager::GetInst()->CallCreateComponent(m_Object, TypeID);

		Component->Load(File);

		m_vecChild.push_back((CSceneComponent*)Component);
		((CSceneComponent*)Component)->m_Parent = this;

		// m_Transform������ �������� �߰�
		m_Transform->m_vecChild.push_back(((CSceneComponent*)Component)->m_Transform);
		((CSceneComponent*)Component)->m_Transform->m_Parent = m_Transform;
	}

	return true;
}

bool CSceneComponent::SaveOnly(FILE* File)
{
	CComponent::SaveOnly(File);

	fwrite(&m_Render, sizeof(bool), 1, File);

	int	Length = (int)m_LayerName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_LayerName.c_str(), sizeof(char), Length, File);

	m_Transform->Save(File);

	return true;
}

bool CSceneComponent::LoadOnly(FILE* File)
{
	CComponent::LoadOnly(File);

	fread(&m_Render, sizeof(bool), 1, File);

	int	Length = 0;
	char	LayerName[256] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(LayerName, sizeof(char), Length, File);

	m_LayerName = LayerName;

	m_Transform->Load(File);

	return true;
}

void CSceneComponent::RenderAnimationEditor()
{
	m_Transform->SetAnimationTransform();

	CRenderManager::GetInst()->GetStandard2DCBuffer()->SetAnimation2DEnable(false);
	CRenderManager::GetInst()->GetStandard2DCBuffer()->UpdateCBuffer();
}

void CSceneComponent::SetUpdateByMat(bool UpdateByMat)
{
	m_Transform->SetUpdateByMat(UpdateByMat);
}

void CSceneComponent::DecomposeWorld()
{
	m_Transform->DecomposeWorld();
}

void CSceneComponent::AddWorldPosByLocalAxis(AXIS Axis, float Amount)
{
	m_Transform->AddWorldPosByLocalAxis(Axis, Amount);
}

void CSceneComponent::AddWorldPosByLocalAxis(const Vector3& Pos)
{
	m_Transform->AddWorldPosByLocalAxis(Pos);
}

void CSceneComponent::SetTransformByWorldMatrix(const Matrix& matTRS)
{
	m_Transform->SetTransformByWorldMatrix(matTRS);
}

void CSceneComponent::RenderParticleEffectEditor()
{
	m_Transform->SetParticleEffectEditorTransform();

	CRenderManager::GetInst()->GetStandard2DCBuffer()->SetAnimation2DEnable(false);
	CRenderManager::GetInst()->GetStandard2DCBuffer()->UpdateCBuffer();
}

