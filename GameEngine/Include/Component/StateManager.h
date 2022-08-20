#pragma once

#include "../../GameInfo.h"

class CStateManager
{
friend class CSceneManager;

	CStateManager();
	~CStateManager();

private:
	std::unordered_map<std::string, class CState*>	m_mapState;

public:
	void AddState(const std::string& Name, class CState* State);
	class CState* FindState(const std::string& Name);

//
//public:
//	void Airborne(const NotifyParameter& Param);
//	void Restraint(const NotifyParameter& Param);
//	void Stun(const NotifyParameter& Param);
//	void KnockBack(const NotifyParameter& Param);

public:
	void CheckAirborneTarget(const NotifyParameter& Param);
	void FindRestraintTarget(const NotifyParameter& Param);
	void FindStunTarget(const NotifyParameter& Param);
	void FindKnockBackTarget(const NotifyParameter& Param);

};

