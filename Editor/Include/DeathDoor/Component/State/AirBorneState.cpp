
#include "AirBorneState.h"
#include "Component/StateComponent.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

CAirBorneState::CAirBorneState() :
	m_AccTime(0.f)
{
}

CAirBorneState::CAirBorneState(const CAirBorneState& State)
{
}

CAirBorneState::~CAirBorneState()
{
}

void CAirBorneState::Update(float DeltaTime)
{
	m_AccTime += DeltaTime;

	m_OwnerObject->AddWorldPos(0.f, 3.f - GRAVITY * m_AccTime * 3.f, 0.f);

	Vector3 CurrentPos = m_OwnerObject->GetWorldPos();
	float Height = m_OwnerObject->GetScene()->GetNavigation3DManager()->GetY(CurrentPos);

	if (CurrentPos.y < Height)
		m_IsEnd = true;
}

void CAirBorneState::PostUpdate(float DeltaTime)
{
}

CAirBorneState* CAirBorneState::Clone()
{
	return new CAirBorneState(*this);
}

void CAirBorneState::OnStart()
{
	m_OwnerObject->GetRootComponent()->GetTransform()->SetState(Transform_State::Falling);
	m_OwnerObject->SetNoInterrupt(true);
}

void CAirBorneState::OnEnd()
{
	m_OwnerObject->GetRootComponent()->GetTransform()->SetState(Transform_State::Ground);
	m_OwnerObject->SetNoInterrupt(false);
}
