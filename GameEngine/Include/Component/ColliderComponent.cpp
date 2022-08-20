
#include "ColliderComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneCollision.h"
#include "../Collision/CollisionManager.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Resource/Shader/TransformConstantBuffer.h"

CColliderComponent::CColliderComponent()	:
	m_RigidCollisionIgnore(false)
{
	SetTypeID<CColliderComponent>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_CurrentSectionCheck = false;
	m_Profile = nullptr;
	m_MouseCollision = false;
	m_CBuffer = nullptr;
	m_LayerName = "Collider";
}

CColliderComponent::CColliderComponent(const CColliderComponent& com) :
	CSceneComponent(com)
{
	m_MouseCollision = false;
	m_CurrentSectionCheck = false;
	m_Render = com.m_Render;
	m_Profile = com.m_Profile;
	m_CBuffer = com.m_CBuffer->Clone();
	m_Mesh = com.m_Mesh;
	m_Shader = com.m_Shader;
}

CColliderComponent::~CColliderComponent()
{
	SAFE_DELETE(m_CBuffer);

	auto	iter = m_PrevCollisionList.begin();
	auto	iterEnd = m_PrevCollisionList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->DeletePrevCollision(this);

		if (m_Object->GetScene())
		{
			(*iter)->CallCollisionCallback(Collision_State::End);
			CallCollisionCallback(Collision_State::End);
		}
	}
}

void CColliderComponent::SetCollisionProfile(const std::string& Name)
{
	m_Profile = CCollisionManager::GetInst()->FindProfile(Name);
}

void CColliderComponent::CheckPrevColliderSection()
{
	auto	iter = m_PrevCollisionList.begin();
	auto	iterEnd = m_PrevCollisionList.end();

	// 이전프레임에 충돌되었던 충돌체들을 반복하며 영역이 겹치는지를 판단한다.
	// 만약 이전프레임에 충돌이 되었는데 현재 서로 다른 영역에 존재한다면 해당 충돌체와는 충돌 가능성이
	// 없으므로 충돌되었다가 떨어지는것으로 판단해주면 된다.
	for (; iter != iterEnd;)
	{
		bool	Check = false;

		// 충돌영역을 체크한다.
		size_t	Size = m_vecSectionIndex.size();

		for (size_t i = 0; i < Size; ++i)
		{
			size_t	DestSize = (*iter)->m_vecSectionIndex.size();

			for (size_t j = 0; j < DestSize; ++j)
			{
				if (m_vecSectionIndex[i] == (*iter)->m_vecSectionIndex[j])
				{
					Check = true;
					break;
				}
			}

			if (Check)
				break;
		}

		// 서로 겹치는 충돌영역이 없으므로 붙어있다가 떨어지는 것이다.
		if (!Check)
		{
			CallCollisionCallback(Collision_State::End);

			if((*iter)->IsEnable())
				(*iter)->CallCollisionCallback(Collision_State::End);

			// 서로 이전 충돌목록에서 제거해준다.
			(*iter)->DeletePrevCollision(this);
			
			iter = m_PrevCollisionList.erase(iter);
			iterEnd = m_PrevCollisionList.end();
			continue;
		}

		++iter;
	}
}

void CColliderComponent::AddPrevCollision(CColliderComponent* Collider)
{
	m_PrevCollisionList.push_back(Collider);
}

void CColliderComponent::DeletePrevCollision(CColliderComponent* Collider)
{
	auto	iter = m_PrevCollisionList.begin();
	auto	iterEnd = m_PrevCollisionList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Collider)
		{
			m_PrevCollisionList.erase(iter);
			return;
		}
	}
}

bool CColliderComponent::EmptyPrevCollision()
{
	return m_PrevCollisionList.empty();
}

bool CColliderComponent::CheckPrevCollision(CColliderComponent* Collider)
{
	auto	iter = m_PrevCollisionList.begin();
	auto	iterEnd = m_PrevCollisionList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Collider)
			return true;
	}

	return false;
}

bool CColliderComponent::CheckCurrentFrameCollision(CColliderComponent* Collider)
{
	auto	iter = m_CurrentCollisionList.begin();
	auto	iterEnd = m_CurrentCollisionList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Collider)
			return true;
	}

	return false;
}

void CColliderComponent::AddCurrentFrameCollision(CColliderComponent* Collider)
{
	if (!CheckCurrentFrameCollision(Collider))
		m_CurrentCollisionList.push_back(Collider);
}

void CColliderComponent::CallCollisionCallback(Collision_State State)
{
	auto	iter = m_CollisionCallback[(int)State].begin();
	auto	iterEnd = m_CollisionCallback[(int)State].end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)(m_Result);
	}
}

void CColliderComponent::CallCollisionMouseCallback(Collision_State State)
{
	if (State == Collision_State::End)
		m_MouseCollision = false;

	auto	iter = m_CollisionMouseCallback[(int)State].begin();
	auto	iterEnd = m_CollisionMouseCallback[(int)State].end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)(m_MouseResult);
	}
}

void CColliderComponent::ClearFrame()
{
	if (!m_Enable)
	{
		m_PrevCollisionList.clear();
	}
	m_vecSectionIndex.clear();
	m_CurrentCollisionList.clear();
	m_CurrentSectionCheck = false;
}

void CColliderComponent::Start()
{
	CSceneComponent::Start();
}

bool CColliderComponent::Init()
{
	if (!CSceneComponent::Init())
		return false;

	SetCollisionProfile("Object");

	m_CBuffer = new CColliderConstantBuffer;

	m_CBuffer->Init();

	m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	m_Shader = CResourceManager::GetInst()->FindShader("ColliderShader");


	return true;
}

void CColliderComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	// if (m_EnablePossibleTime > 0.f && IsEnable())
	// {
	// 	m_EnablePossibleTime -= DeltaTime;
	// 
	// 	if (m_EnablePossibleTime <= 0.f)
	// 	{
	// 		CRef::Enable(false);
	// 
	// 		m_EnablePossibleTime = 0.f;
	// 	}
	// }
}

void CColliderComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

	if (m_Enable)
	{
		m_Scene->GetCollision()->AddCollider(this);
	}
}

void CColliderComponent::CheckCollision()
{
	CSceneComponent::CheckCollision();
}

void CColliderComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CColliderComponent::Render()
{

	CSceneComponent::Render();
}

void CColliderComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CColliderComponent* CColliderComponent::Clone()
{
	return nullptr;
}

bool CColliderComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);

	fwrite(&m_ColliderType, sizeof(Collider_Type), 1, File);
	fwrite(&m_Offset, sizeof(Vector3), 1, File);
	fwrite(&m_Min, sizeof(Vector3), 1, File);
	fwrite(&m_Max, sizeof(Vector3), 1, File);

	char MeshName[256] = {};

	int Length = (int)m_Mesh->GetName().length();
	strcpy_s(MeshName, m_Mesh->GetName().c_str());

	fwrite(&Length, sizeof(int), 1, File);

	fwrite(MeshName, sizeof(char), Length, File);

	Length = (int)m_Shader->GetName().length(); 

	char ShaderName[256] = {};
	strcpy_s(ShaderName, m_Shader->GetName().c_str());

	fwrite(&Length, sizeof(int), 1, File);
	fwrite(ShaderName, sizeof(char), Length, File);

	Length = (int)m_Profile->Name.length();
	fwrite(&Length, sizeof(int), 1, File);

	char ProfileName[256] = {};
	strcpy_s(ProfileName, m_Profile->Name.c_str());
	fwrite(ProfileName, sizeof(char), Length, File);


	return true;
}

bool CColliderComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);

	fread(&m_ColliderType, sizeof(Collider_Type), 1, File);
	fread(&m_Offset, sizeof(Vector3), 1, File);
	fread(&m_Min, sizeof(Vector3), 1, File);
	fread(&m_Max, sizeof(Vector3), 1, File);

	char MeshName[256] = {};

	int Length = 0;
	fread(&Length, sizeof(int), 1, File);
	fread(MeshName, sizeof(char), Length, File);

	m_Mesh = CResourceManager::GetInst()->FindMesh(MeshName);
	m_Mesh->SetName(MeshName);

	Length = 0;
	char ShaderName[256] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(ShaderName, sizeof(char), Length, File);

	m_Shader = CResourceManager::GetInst()->FindShader(ShaderName);

	m_Shader->SetName(ShaderName);

	SetCollisionProfile("Object");

	m_CBuffer = new CColliderConstantBuffer;

	m_CBuffer->Init();

	m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	Length = 0;
	fread(&Length, sizeof(int), 1, File);

	char ProfileName[256] = {};
	fread(ProfileName, sizeof(char), Length, File);

	std::string strProfileName = ProfileName;

	SetCollisionProfile(strProfileName);

	return true;
}

bool CColliderComponent::CollisionRay(const Ray& Ray)
{
	return false;
}

void CColliderComponent::Reset()
{
	CSceneComponent::Reset();

	m_EnablePossibleTime = 0.f;

	Enable(false);
}

void CColliderComponent::ClearCollisionCallBack()
{
	m_CollisionCallback->clear();
}
