
#include "ParticleManager.h"

CParticleManager::CParticleManager()
{
}

CParticleManager::~CParticleManager()
{
	auto iter = m_mapParticle.begin();
	auto iterEnd = m_mapParticle.end();

	for (; iter != iterEnd;)
	{
		iter = m_mapParticle.erase(iter);
		iterEnd = m_mapParticle.end();
	}

	m_mapParticle.clear();
}

bool CParticleManager::Init()
{
	return true;
}

bool CParticleManager::CreateParticle(const std::string& Name)
{
	CParticle* Particle = new CParticle;

	Particle->SetName(Name);

	if (!Particle->Init())
	{
		SAFE_RELEASE(Particle);
		return false;
	}

	m_mapParticle.insert(std::make_pair(Name, Particle));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return true;
}

CParticle* CParticleManager::FindParticle(const std::string& Name)
{
	auto	iter = m_mapParticle.find(Name);

	if (iter == m_mapParticle.end())
		return nullptr;

	return iter->second;
}

void CParticleManager::ReleaseParticle(const std::string& Name)
{
	auto	iter = m_mapParticle.find(Name);

	if (iter != m_mapParticle.end())
	{
		if (iter->second->GetRefCount() == 1)
		{
			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}

			m_mapParticle.erase(iter);
		}
	}
}

void CParticleManager::AddParticle(CParticle* Particle)
{
	CParticle* FoundParticle = FindParticle(Particle->GetName());
	
	if (FoundParticle)
		return;

	m_mapParticle.insert(std::make_pair(Particle->GetName(), Particle));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}
}

void CParticleManager::ChangeParticleKeyName(const std::string& OldKeyName, const std::string& NewKeyName)
{
	if (OldKeyName == NewKeyName)
	{
		return;
	}

	CParticle* FoundParticle = FindParticle(OldKeyName);

	if (!FoundParticle)
		return;

	// 새로운 이름으로 insert
	m_mapParticle.insert(std::make_pair(NewKeyName, FoundParticle));

	// 기존 것은 지운다
	DeleteParticle(OldKeyName);

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}
}

bool CParticleManager::DeleteParticle(const std::string& ParticleName)
{
	auto iter = m_mapParticle.find(ParticleName);

	if (iter == m_mapParticle.end())
	{
		if (m_ChangeCallBack)
		{
			m_ChangeCallBack();
		}

		return false;
	}

	m_mapParticle.erase(iter);

	return true;
}
