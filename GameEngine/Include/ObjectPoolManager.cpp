
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
	// ������ ȣ��� �� ���ڸ�ŭ stack�� �̸� ������Ʈ ���� reserve
	m_ObjectVector.reserve(m_InitObjectCount);
	for (int i = 0; i < m_InitObjectCount; ++i)
	{
		m_ObjectVector.emplace_back(new CGameObject);
	}

	return true;
}

CGameObject* CObjectPoolManager::Allocate()
{

	// ���ÿ� �ִ� ��� object���� ������� �� �ٽ� 30���� ä���ֱ�
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
