#pragma once

#include "../../GameInfo.h"
#include "../../GameObject/GameObject.h"

class CState
{
public:
	CState();
	CState(const CState& state);
	virtual ~CState() = 0;

public:
	virtual void Update(float deltaTime) = 0;
	virtual void PostUpdate(float deltaTime) = 0;
	virtual CState* Clone() = 0;

public:
	virtual void OnStart() = 0;
	virtual void OnEnd() = 0;

public:
	bool IsEnd() const
	{
		return m_IsEnd;
	}

	void SetOwner(class CStateComponent* com)
	{
		m_Owner = com;
	}

	const std::string& GetName()	const
	{
		return m_Name;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	void SetOwnerObject(CGameObject* Object);

	CGameObject* GetOwnerObject()	const;

	void SetSourceObject(CGameObject* Src)
	{
		m_SourceObject = Src;
	}

	CGameObject* GetSourceObject()	const
	{
		return m_SourceObject;
	}

protected:
	class CStateComponent* m_Owner;
	CGameObject* m_OwnerObject;
	CGameObject* m_SourceObject; // 내가 이 State를 갖도록 하게 만든 Object 
	bool m_IsEnd;
	std::string m_Name;
};

