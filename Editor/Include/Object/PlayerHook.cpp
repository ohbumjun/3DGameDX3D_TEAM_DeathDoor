
#include "PlayerHook.h"
#include "Component/StaticMeshComponent.h"

CPlayerHook::CPlayerHook()	:
	m_Distance(30.f),
	m_AccDistance(0.f)
{
	SetTypeID<CPlayerHook>();
}

CPlayerHook::CPlayerHook(const CPlayerHook& obj)	:
	CGameObject(obj)
{
}

CPlayerHook::~CPlayerHook()
{
}

bool CPlayerHook::Init()
{
	if (!CGameObject::Init())
		return false;

	//for (int i = 0; i < 20; ++i)
	//{
	//	CStaticMeshComponent* HookBody = CreateComponent<CStaticMeshComponent>("CPlayerHookBody");
	//	HookBody->SetRender(false);
	//	m_HookBodyList.push_back(HookBody);
	//}

	return true;
}

void CPlayerHook::Start()
{
	CGameObject::Start();

	m_HookHeadMesh = (CStaticMeshComponent*)GetRootComponent();

	if (!m_HookHeadMesh)
		return;

	size_t Count = GetRootComponent()->GetChildCount();

	for (size_t i = 0; i < Count; ++i)
	{
		CSceneComponent* Child = GetRootComponent()->GetChild((int)i);
		Child->SetRender(false);
		m_HookBodyList.push_back(Child);
	}
}

void CPlayerHook::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CPlayerHook::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CPlayerHook* CPlayerHook::Clone()
{
	return new CPlayerHook(*this);
}

bool CPlayerHook::Move(const Vector3& Dir, float Speed, bool& Collision)
{
	Vector3 Dist = Vector3(Dir.x * Speed, 0.f, Dir.z * Speed);
	
	m_AccDistance += Dist.Length();
	m_AccUnitDistance += Dist.Length();

	if (m_AccDistance > m_Distance)
	{
		m_AccDistance = 0.f;
		return true;
	}

	m_RootComponent->AddWorldPos(Dist);

	if (m_AccUnitDistance > 2.f)
	{
		m_AccUnitDistance = 0.f;
		
		auto iter = m_HookBodyList.begin();
		(*iter)->SetWorldPos(m_RootComponent->GetWorldPos());
		(*iter)->SetRender(true);
	}

	// OBJ가 추가 (Relase Mode 컴파일 에러 -> 모든 경로 리턴값 반환)
	return false;
}

void CPlayerHook::DisableRenderAllComponent()
{

}

void CPlayerHook::EnableRenderAllComponent()
{
}
