#pragma once

#include "Component\Node\ActionNode.h"

class CChasePlayerNode :
    public CActionNode
{
public:
    CChasePlayerNode();
    CChasePlayerNode(const CChasePlayerNode& Node);
    virtual ~CChasePlayerNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    void FindPath();

private:
    class CMonsterDataComponent* m_DataComp;
    class CMonsterNavAgent* m_Nav;
    class CSceneComponent* m_RootComp;
    class CGameObject* m_PlayerObj;
    float m_MoveSpeed;
    float m_RotationSpeed;
};

