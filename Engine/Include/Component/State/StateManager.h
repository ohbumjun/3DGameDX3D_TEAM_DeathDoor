#pragma once

#include "../../GameInfo.h"

class CStateManager
{
	friend class CSceneManager;

protected:
	CStateManager();
	virtual ~CStateManager() = 0;

private:
	std::unordered_map<std::string, class CState*>	m_mapState;

public:
	void Init();

public:
	void AddState(const std::string& Name, class CState* State);
	class CState* FindState(const std::string& Name);


};

