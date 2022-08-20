
#include "State.h"
#include "../../GameObject/GameObject.h"

CState::CState()	:
	m_OwnerObject(nullptr),
	m_SourceObject(nullptr)
{
}

CState::CState(const CState& state)
{
}

CState::~CState()
{
}


void CState::SetOwnerObject(CGameObject* Object)
{
	m_OwnerObject = Object;
}

CGameObject* CState::GetOwnerObject() const
{
	return m_OwnerObject;
}