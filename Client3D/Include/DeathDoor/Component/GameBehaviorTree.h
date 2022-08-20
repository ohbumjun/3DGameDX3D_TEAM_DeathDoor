#pragma once

#include "Component\BehaviorTree.h"
#include "GameInfo.h"

class CGameBehaviorTree :
    public CBehaviorTree
{
    friend class CStateComponent;

public:
    CGameBehaviorTree();
    CGameBehaviorTree(const CGameBehaviorTree& Tree);
    ~CGameBehaviorTree();

public:
    virtual bool Init();
    virtual void Start();

public:
    class CGameStateComponent* GetOwner() const;

public:
    virtual CNode* LoadNode(CNode* Parent, size_t TypeID);
   CNode* LoadConditionNode(CNode* Parent, size_t TypeID);
   CNode* LoadActionNode(CNode* Parent, size_t TypeID);

public:
    template <typename T>
    T* MakeNode(CNode* Parent, class CGameObject* OwnerObject)
    {
        T* NewNode = new T;
        NewNode->SetParent(Parent);
        NewNode->SetOwner(this);
        NewNode->SetObject(OwnerObject);
        NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);
        return NewNode;
    }
};

