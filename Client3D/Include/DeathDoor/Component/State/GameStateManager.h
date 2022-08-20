#pragma once

#include "Component/State/StateManager.h"

class CGameStateManager :
	public CStateManager
{
	friend class CSceneManager;
	friend class CEditorManager;

	CGameStateManager();
	~CGameStateManager();

public:
	void Init();

public:
	//void CheckAirborneTarget(const NotifyParameter& Param);
	//void GiveDamage(const NotifyParameter& Param);
	//void FindRestraintTarget(const NotifyParameter& Param);
	//void FindStunTarget(const NotifyParameter& Param);
	//void FindKnockBackTarget(const NotifyParameter& Param);
};

