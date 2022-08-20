#include "ArrowCollisionFireCollider.h"
#include "TriggerFireLamp.h"
#include "Component/StaticMeshComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/LightComponent.h"
#include "ArrowComponent.h"
#include "GameObject/GameObject.h"
#include "../DDUtil.h"

CArrowCollisionFireCollider::CArrowCollisionFireCollider() :
	m_FirstManagedChildByTriggerLamp(false)
{
	SetTypeID<CArrowCollisionFireCollider>();
}

CArrowCollisionFireCollider::CArrowCollisionFireCollider(const CArrowCollisionFireCollider& com) :
	CColliderSphere(com)
{
	// ClearCollisionCallBack();
	AddCollisionCallback(Collision_State::Begin, this, &CArrowCollisionFireCollider::OnCollidePlayerArrow);
}

CArrowCollisionFireCollider::~CArrowCollisionFireCollider()
{
}

void CArrowCollisionFireCollider::Start()
{
	CColliderSphere::Start();

	// ClearCollisionCallBack();

	AddCollisionCallback(Collision_State::Begin, this, &CArrowCollisionFireCollider::OnCollidePlayerArrow);

	m_Info.Radius = 2.0f;

	SetRelativePos(0.f, 2.f, 0.f);

	// Collision Profile 의 경우 , Monster 로 세팅한다.
	SetCollisionProfile("Monster");

	// 모든 Particle Component 들을 Enable False 처리해둔다.
	// 단, 첫번째에 해당하는 불은, m_FirstManagedChildByTriggerLamp 를 true 로 세팅해줄 것이다.
	std::vector<CParticleComponent*> vecParticleComponents;
	m_Object->FindAllSceneComponentFromType<CParticleComponent>(vecParticleComponents);

	size_t ComponentSize = vecParticleComponents.size();

	for (size_t i = 0; i < ComponentSize; ++i)
	{
		// Disable New Alive 가 False 인 녀석들은 Enable False 처리해주어야 한다.
		vecParticleComponents[i]->CRef::Enable(m_FirstManagedChildByTriggerLamp);
		vecParticleComponents[i]->SetRelativePos(0.f, 1.f, 0.f);

		if (vecParticleComponents[i]->GetCBuffer()->IsDisableNewAlive() == 1)
			vecParticleComponents[i]->CRef::Enable(false);
	}

	// Light Component 또한 Enable False 처리 한다.
	CLightComponent* Light = m_Object->FindComponentFromType<CLightComponent>();

	if (Light)
		Light->CRef::Enable(m_FirstManagedChildByTriggerLamp);
}

void CArrowCollisionFireCollider::Update(float DeltaTime)
{
	CColliderSphere::Update(DeltaTime);
}

void CArrowCollisionFireCollider::OnCollidePlayerArrow(const CollisionResult& Result)
{
	// 1. Arrow 가 불이 붙지 않았다면
	// 1) 장작에 불이 붙어있지 않다면 아무일 X
	// 2) 장작에 불이 붙었다면, Arrow 에 불을 붙인다.

	// 2. Arrow 가 불이 붙어있다면 
	// 1) 장작에 불이 붙어있지 않다면, 장작에 불을 붙인다 (CArrowCollisionFireCollider 에서 해주는 일)aw1
	// 2) 장작에 불이 붙어 있다면, 아무 일도 없게 한다.

	if (Result.Dest->GetGameObject()->GetName() == "PlayerArrow")
	{
		// Arrow 가 불이 붙어있고, 내가 아직 Arrow 와 충돌하지 않았다면 (불이 붙어있지 않다면)
		// 나는 불을 붙일 것이다.
		CArrowComponent* PlayerArrow = Result.Dest->GetGameObject()->FindComponentFromType<CArrowComponent>();

		if (IsFireOnByArrow() == false && PlayerArrow->IsArrowOnFire())
		{
			// 모든 Particle Component 들을 Start 처리해준다.
			m_Object->StartParticle(m_Object->GetWorldPos());

			CLightComponent* Light = m_Object->FindComponentFromType<CLightComponent>();

			if (Light)
				Light->CRef::Enable(true);

			// 부모 Object 의 TriggerFireLamp Component 에게 하나 켜졌다고 알려준다.
			CGameObject* ParentObject = m_Object->GetParentObject();

			if (ParentObject)
			{
				CTriggerFireLamp* FireLampManager = ParentObject->FindComponentFromType<CTriggerFireLamp>();

				FireLampManager->IncActiveCount();
			}
		}
	}
}

bool CArrowCollisionFireCollider::IsFireOnByArrow() const
{
	CLightComponent* Light = m_Object->FindComponentFromType<CLightComponent>();

	if (Light)
		return Light->IsEnable();

	return false;
}
