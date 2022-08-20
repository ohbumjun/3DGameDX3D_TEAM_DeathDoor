
#include "LadderCollider.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "PlayerDataComponent.h"

CLadderCollider::CLadderCollider()
{
	SetTypeID<CLadderCollider>();
	m_ComponentType = Component_Type::SceneComponent;
}

CLadderCollider::CLadderCollider(const CLadderCollider& com)	:
	CColliderBox3D(com)
{
}

CLadderCollider::~CLadderCollider()
{
}

void CLadderCollider::Start()
{
	CColliderBox3D::Start();

	AddCollisionCallback<CLadderCollider>(Collision_State::Begin, this, &CLadderCollider::OnPlayerLadderColliderBegin);
	AddCollisionCallback<CLadderCollider>(Collision_State::End, this, &CLadderCollider::OnPlayerLadderColliderEnd);
}

bool CLadderCollider::Init()
{
	if (!CColliderBox3D::Init())
		return false;

	return true;
}

void CLadderCollider::Update(float DeltaTime)
{
	CColliderBox3D::Update(DeltaTime);

	
}

void CLadderCollider::PostUpdate(float DeltaTime)
{
	CColliderBox3D::PostUpdate(DeltaTime);
}

void CLadderCollider::PrevRender()
{
	CColliderBox3D::PrevRender();
}

void CLadderCollider::Render()
{
	CColliderBox3D::Render();
}

void CLadderCollider::PostRender()
{
	CColliderBox3D::PostRender();
}

CLadderCollider* CLadderCollider::Clone()
{
	return new CLadderCollider(*this);
}

bool CLadderCollider::Save(FILE* File)
{
	return CColliderBox3D::Save(File);
}

bool CLadderCollider::Load(FILE* File)
{
	return CColliderBox3D::Load(File);
}

bool CLadderCollider::SaveOnly(FILE* File)
{
	return true;
}

bool CLadderCollider::LoadOnly(FILE* File)
{
	return true;
}

void CLadderCollider::OnPlayerLadderColliderBegin(const CollisionResult& Result)
{
	CGameObject* Player = m_Object->GetScene()->GetPlayerObject();
	
	Vector3 LadderPos = m_Object->GetWorldPos();
	Vector3 LadderColliderPos = GetWorldPos();
	Vector3 PlayerPos = Player->GetWorldPos();

	CPlayerDataComponent* Comp = Player->FindObjectComponentFromType<CPlayerDataComponent>();

	Comp->SetClimbingStartEnable(true);

	// 사다리에 타고 있지 않을때 LadderCollider와 충돌
	if (!Comp->IsClimbingLadder())
	{
		Comp->SetAdjLadder(m_Object);

		// 하단 Ladder Collider와 충돌
		if (LadderPos.y > LadderColliderPos.y)
		{
			Comp->SetLadderUpEnable(true);
		}

		// 상단 Ladder Collider와 충돌
		else
		{
			Comp->SetLadderDownEnable(true);
		}
	}

	// 사다리에 타고 있을때 LadderCollider와 충돌
	else
	{
		// 하단 Ladder Collider와 충돌
		if (LadderPos.y > LadderColliderPos.y)
		{
			Comp->SetLadderDownEnable(false);
		}
		// 상단 Ladder Collider와 충돌
		else if(LadderPos.y < LadderColliderPos.y)
		{
			//Comp->SetLadderUpEnable(false);
		}
	}
}

void CLadderCollider::OnPlayerLadderDisable(const CollisionResult& Result)
{
}

void CLadderCollider::OnPlayerLadderColliderEnd(const CollisionResult& Result)
{
	if (!m_Object)
		return;
	if (!m_Object->GetScene())
		return;
	if (!m_Object->GetScene()->GetSceneMode())
		return;

	CGameObject* Player = m_Object->GetScene()->GetPlayerObject();

	CPlayerDataComponent* Comp = Player->FindObjectComponentFromType<CPlayerDataComponent>();

	Comp->SetClimbingStartEnable(false);

	Vector3 LadderPos = m_Object->GetWorldPos();
	Vector3 LadderColliderPos = GetWorldPos();
	Vector3 PlayerPos = Player->GetWorldPos();

	if (Comp->IsClimbingLadder())
	{
		// 사다리에 타고 있는데 Lower Collider와 떨어졌다
		if (LadderPos.y > LadderColliderPos.y)
		{
			Comp->SetLadderDownEnable(true);
		}

		// 사다리에 타고 있는데 Upper Collider와 떨어졌다
		else if(LadderPos.y < LadderColliderPos.y)
		{
			// 플레이어가 Upper Collider보다 위에 있다
			if(PlayerPos.y > LadderColliderPos.y)
				Comp->SetLadderUpEnable(false);
			// 플레이어가 Upper Collider보다 아래에 있다
			else
				Comp->SetLadderUpEnable(true);
		}
	}

	else
	{
		// 사다리에 타고 있지 않은데 Lower Collider와 떨어졌다
		if (LadderPos.y > LadderColliderPos.y)
		{
			Comp->SetLadderUpEnable(false);
		}

		// 사다리에 타고 있지 않은데 Upper Collider와 떨어졌다
		else if (LadderPos.y < LadderColliderPos.y)
		{
			// 플레이어가 Upper Collider보다 위에 있다
			if (PlayerPos.y >= LadderColliderPos.y)
				Comp->SetLadderDownEnable(false);
		}

	}
}
