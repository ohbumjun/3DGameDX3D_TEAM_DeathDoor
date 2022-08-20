#pragma once

#include "Particle.h"

class CParticleManager
{
	friend class CResourceManager;

private:
	CParticleManager();
	~CParticleManager();

private:
	std::unordered_map<std::string, CSharedPtr<CParticle>>	m_mapParticle;
	std::function<void()> m_ChangeCallBack;

public :
	const std::unordered_map<std::string, CSharedPtr<CParticle>>& GetMapParticle() const
	{
		return m_mapParticle;
	}
public:
	bool Init();
	bool CreateParticle(const std::string& Name);
	CParticle* FindParticle(const std::string& Name);
	void ReleaseParticle(const std::string& Name);
	void AddParticle(CParticle* Particle);
	void ChangeParticleKeyName(const std::string& OldKeyName , const std::string& NewKeyName);
	bool DeleteParticle(const std::string& ParticleName);

	template <typename T>
	T* CreateParticleEmpty()
	{
		T* Particle = new T;

		// Particle->SetConstantBuffer(m_CBuffer);

		return Particle;
	}

public:
	template <typename T>
	void AddResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ChangeCallBack = std::bind(Func, Obj);
	}
};

