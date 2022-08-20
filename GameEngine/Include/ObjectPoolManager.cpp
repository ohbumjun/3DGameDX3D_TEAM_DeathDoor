
#include "ObjectPoolManager.h"
#include "GameObject/GameObject.h"

DEFINITION_SINGLE(CObjectPoolManager)

CObjectPoolManager::CObjectPoolManager() : 
	m_InitObjectCount(100)
{
}

CObjectPoolManager::~CObjectPoolManager()
{
	while (!m_ObjectVector.empty())
	{
		CGameObject* tmp = m_ObjectVector.back();
		m_ObjectVector.pop_back();
		delete tmp;
	}
}

bool CObjectPoolManager::Init()
{
	// 생성자 호출될 때 인자만큼 stack에 미리 오브젝트 공간 reserve
	m_ObjectVector.reserve(m_InitObjectCount);
	for (int i = 0; i < m_InitObjectCount; ++i)
	{
		m_ObjectVector.emplace_back(new CGameObject);
	}

	return true;
}

CGameObject* CObjectPoolManager::Allocate()
{

	// 스택에 있는 모든 object들이 사용중일 때 다시 30개씩 채워넣기
	if (m_ObjectVector.empty())
	{
		for (int i = 0; i < 100; ++i)
		{
			m_ObjectVector.emplace_back(new CGameObject());
			//m_ObjectVector.push_back(new CObject());
		}
	}

	CGameObject* ret = m_ObjectVector.back();
	m_ObjectVector.pop_back();

	return ret;
}

void CObjectPoolManager::Deallocate(CGameObject* obj)
{
	m_ObjectVector.emplace_back(obj);

}
