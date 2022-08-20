
#include "SceneMode.h"
#include "../GameObject/GameObject.h"

CSceneMode::CSceneMode()	:
	m_Start(false)
{
	SetTypeID<CSceneMode>();
}

CSceneMode::~CSceneMode()
{
}

void CSceneMode::SetPlayerObject(CGameObject* Obj)
{
	m_PlayerObject = Obj;
}

void CSceneMode::Start()
{
	if (!m_Start)
	{
		m_Start = true;
	}
}

bool CSceneMode::Init()
{
	return true;
}

void CSceneMode::Update(float DeltaTime)
{
}

void CSceneMode::PostUpdate(float DeltaTime)
{
}
