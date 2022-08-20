
#include "StateManager.h"
#include "State.h"


CStateManager::CStateManager()
{

}

CStateManager::~CStateManager()
{
	auto iter = m_mapState.begin();
	auto iterEnd = m_mapState.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}
}

void CStateManager::Init()
{

}

void CStateManager::AddState(const std::string& Name, CState* State)
{
	m_mapState.insert(std::make_pair(Name, State));
}

CState* CStateManager::FindState(const std::string& Name)
{
	auto iter = m_mapState.find(Name);

	if (iter == m_mapState.end())
		return nullptr;

	return iter->second;
}



