#pragma once

#include "GameInfo.h"

class CRef
{
public:
	CRef();
	virtual ~CRef();

protected:
	std::string	m_Name;
	int			m_RefCount;
	bool		m_Enable;
	bool		m_Active;
	size_t		m_TypeID;

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);

public:
	virtual void Enable(bool bEnable)
	{
		m_Enable = bEnable;
	}

	virtual void Destroy()
	{
		m_Active = false;
	}

	bool IsActive()	const
	{
		return m_Active;
	}

	bool IsEnable()	const
	{
		return m_Enable;
	}

	void AddRef()
	{
		if (m_Name == "MT_MushRoomInit-mat0-_MainTex-atlas-0")
		{
			int a = 0;
		}

		++m_RefCount;
	}

	int GetRefCount()	const
	{
		return m_RefCount;
	}

	int Release()
	{
		if (m_Name == "MT_MushRoomInit-mat0-_MainTex-atlas-0")
		{
			int a = 0;
		}

		--m_RefCount;

		if (m_RefCount <= 0)
		{
			delete	this;
			return 0;
		}

		return m_RefCount;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	const std::string& GetName()	const
	{
		return m_Name;
	}

	template <typename T>
	void SetTypeID()
	{
		m_TypeID = typeid(T).hash_code();
	}

	template <typename T>
	bool CheckType()
	{
		return m_TypeID == typeid(T).hash_code();
	}

	size_t GetTypeID()	const
	{
		return m_TypeID;
	}
};

