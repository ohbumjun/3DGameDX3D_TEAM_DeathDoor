#pragma once

#include "../../GameInfo.h"
#include "../../GameObject/GameObject.h"


class CNode
{
	friend class CBehaviorTree;

public:
	CNode();
	CNode(const CNode& Node);
	virtual ~CNode() = 0;

protected:
	CNode* m_Parent;
	CGameObject* m_Object;
	std::string m_Name;
	size_t		m_TypeID;
	bool		m_IsEnd;
	class CBehaviorTree* m_Owner;
	// Start를 최초로 한번 이미 호출 했는지
	bool m_CallStart;
	Node_Type   m_NodeType;
	class CAnimationMeshComponent* m_AnimationMeshComp;
	Vector2 m_InWindowPos;

public:
	void SetAnimationMeshComponent(class CAnimationMeshComponent* AnimationMeshComp);
	class CAnimationMeshComponent* GetAnimationMeshComponent()  const;

public:
	virtual void Init();

public:
	const Vector2& GetInWindowPos()	const
	{
		return m_InWindowPos;
	}

	void SetInWindowPos(const Vector2& Pos)
	{
		m_InWindowPos = Pos;
	}

	Node_Type GetNodeType()	const
	{
		return m_NodeType;
	}

	bool IsCallStart()
	{
		return m_CallStart;
	}

	void SetCallStart(bool CallStart)
	{
		m_CallStart = CallStart;
	}

	void SetOwner(class CBehaviorTree* Owner);

	class CBehaviorTree* GetOwner();

	void SetEnd(bool End)
	{
		m_IsEnd = End;
	}

	bool IsEnd()	const
	{
		return m_IsEnd;
	}

	void SetTypeID(size_t ID)
	{
		m_TypeID = ID;
	}

	size_t GetTypeID()	const
	{
		return m_TypeID;
	}

	void SetObject(CGameObject* Object)
	{
		m_Object = Object;
	}

	CGameObject* GetGameObject()	const
	{
		return m_Object;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	std::string GetName()	const
	{
		return m_Name;
	}

	CNode* GetParent()	const
	{
		return m_Parent;
	}

	void SetParent(CNode* Parent)
	{
		m_Parent = Parent;
	}

public:
	virtual NodeResult Invoke(float DeltaTime) = 0;

public:
	virtual NodeResult OnStart(float DeltaTime) = 0;
	virtual NodeResult OnUpdate(float DeltaTime) = 0;
	virtual NodeResult OnEnd(float DeltaTime) = 0;

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File);
	virtual bool LoadOnly(FILE* File);
	virtual bool SavePos(FILE* File);
	virtual bool LoadPos(FILE* File);
};

