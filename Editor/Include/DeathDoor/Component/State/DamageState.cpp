
#include "DamageState.h"
#include "Component/StateComponent.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

CDamageState::CDamageState()
{
}

CDamageState::CDamageState(const CDamageState& State)	:
	CState(State)
{
}

CDamageState::~CDamageState()
{
}

void CDamageState::Update(float DeltaTime)
{

}

void CDamageState::PostUpdate(float DeltaTime)
{
}

CDamageState* CDamageState::Clone()
{
	return new CDamageState(*this);
}

void CDamageState::OnStart()
{
	//m_Owner->GetGameObject()->GetDamage(m_Damage);

	//m_IsEnd = true;
}

void CDamageState::OnEnd()
{
}
