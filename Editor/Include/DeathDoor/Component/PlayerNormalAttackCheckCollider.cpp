
#include "PlayerNormalAttackCheckCollider.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "PlayerDataComponent.h"
#include "GameObject/GameObject.h"
#include "Input.h"
#include "ObjectDataComponent.h"
#include "Scene/Scene.h"

CPlayerNormalAttackCheckCollider::CPlayerNormalAttackCheckCollider()
{
	SetTypeID<CPlayerNormalAttackCheckCollider>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::Sphere;

	AddCollisionCallback<CPlayerNormalAttackCheckCollider>(Collision_State::Begin, this, &CPlayerNormalAttackCheckCollider::AttackSuccess);

	SetCollisionProfile("PlayerAttack");
}

CPlayerNormalAttackCheckCollider::CPlayerNormalAttackCheckCollider(const CPlayerNormalAttackCheckCollider& com) :
	CColliderSphere(com)
{
}

CPlayerNormalAttackCheckCollider::~CPlayerNormalAttackCheckCollider()
{
}

void CPlayerNormalAttackCheckCollider::Start()
{
	CColliderSphere::Start();
}

bool CPlayerNormalAttackCheckCollider::Init()
{
	if (!CColliderSphere::Init())
		return false;

	return true;
}

void CPlayerNormalAttackCheckCollider::Update(float DeltaTime)
{
	CColliderSphere::Update(DeltaTime);
}

void CPlayerNormalAttackCheckCollider::PostUpdate(float DeltaTime)
{
	CColliderSphere::PostUpdate(DeltaTime);
}

void CPlayerNormalAttackCheckCollider::PrevRender()
{
	CColliderSphere::PrevRender();
}

void CPlayerNormalAttackCheckCollider::Render()
{
	CColliderSphere::Render();
}

void CPlayerNormalAttackCheckCollider::PostRender()
{
	CColliderSphere::PostRender();
}

CPlayerNormalAttackCheckCollider* CPlayerNormalAttackCheckCollider::Clone()
{
	return new CPlayerNormalAttackCheckCollider(*this);
}

bool CPlayerNormalAttackCheckCollider::Save(FILE* File)
{
	return CColliderSphere::Save(File);
}

bool CPlayerNormalAttackCheckCollider::Load(FILE* File)
{
	return CColliderSphere::Load(File);
}

bool CPlayerNormalAttackCheckCollider::Collision(CColliderComponent* Dest)
{
	return CColliderSphere::Collision(Dest);
}

bool CPlayerNormalAttackCheckCollider::CollisionMouse(const Vector2& MousePos)
{
	return false;
}

void CPlayerNormalAttackCheckCollider::AttackSuccess(const CollisionResult& Result)
{
	CPlayerDataComponent* PlayerDataComp = m_Object->FindObjectComponentFromType<CPlayerDataComponent>();

	if (!PlayerDataComp)
		return;

	CGameObject* CollideObj = Result.Dest->GetGameObject();
	CObjectDataComponent* DataComp = dynamic_cast<CObjectDataComponent*>(CollideObj->FindComponent("ObjectData"));

	Vector3 Vec1 = (CollideObj->GetWorldPos() - m_Object->GetWorldPos());
	Vec1.Normalize();
	Vec1.y = 0.f;
	Vector3 Vec2 = m_Object->GetWorldAxis(AXIS_Z) * -1.f;
	Vec2.Normalize();
	Vec2.y = 0.f;

	// 공격 범위 안에 들어왔는지 판정
	if (Vec1.Dot(Vec2) > 0.f && PlayerDataComp->GetOnSlash())
	{
		if (DataComp)
		{
			// 이전 프레임들에서 충돌했던 경우 중복해서 넣지 않는다.
			auto iter = m_CollisionObjDataList.begin();
			auto iterEnd = m_CollisionObjDataList.end();

			bool InList = false;
			for (; iter != iterEnd; ++iter)
			{
				if (DataComp == (*iter))
				{
					InList = true;
					break;
				}
			}

			// TODO : Player Attack Collider : 데미지 처리
			if (!InList)
			{
				int Attack = PlayerDataComp->GetAttack();
				DataComp->DecreaseHP(Attack);
				DataComp->SetIsHit(true);
				m_CollisionObjDataList.push_back(DataComp);

				int Count = PlayerDataComp->GetConsecutiveAttackCount();
				switch (Count)
				{
				case 0:
				case 1:
					m_Object->GetScene()->GetResource()->SoundPlay("EnemyHit1");
					break;
				case 2:
					m_Object->GetScene()->GetResource()->SoundPlay("EnemyHit2");
					break;
				case 3:
					m_Object->GetScene()->GetResource()->SoundPlay("EnemyHit3");
					break;
				}
			}
		}
	}

 //	auto iter = m_PrevCollisionList.begin();
 //	auto iterEnd = m_PrevCollisionList.end();
 //
 //	for (; iter != iterEnd; ++iter)
 //	{
 //			CObjectDataComponent* Comp = dynamic_cast<CObjectDataComponent*>((*iter)->GetGameObject()->FindComponent("ObjectData"));
 //
 //			if (Comp)
 //			{
 //				bool AlreadyHit = Comp->GetIsHit();
 //				if (!AlreadyHit)
 //				{
 //					Comp->SetIsHit(true);
 //				}
 //			}
 // 		}
 //	}
}

void CPlayerNormalAttackCheckCollider::Enable(bool Enable)
{
	CColliderSphere::Enable(Enable);

	// Enable False 처리하는 경우, 이전 프레임들에서 충돌한 Object Data들의 Hit 상태를 false로 되돌림
	if (!Enable)
	{
		auto iter = m_CollisionObjDataList.begin();
		auto iterEnd = m_CollisionObjDataList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->SetIsHit(false);
		}

		m_CollisionObjDataList.clear();
	}
}

void CPlayerNormalAttackCheckCollider::DeleteObjectData(CObjectDataComponent* Comp)
{
	auto iter = m_CollisionObjDataList.begin();
	auto iterEnd = m_CollisionObjDataList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == Comp)
		{
			m_CollisionObjDataList.erase(iter);
			return;
		}
	}
}
