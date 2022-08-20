#pragma once
#include "Component\Node\ActionNode.h"

class CGruntCommonSpawnNode :
    public CActionNode
{
public:
    CGruntCommonSpawnNode();
    CGruntCommonSpawnNode(const CGruntCommonSpawnNode& Node);
    virtual ~CGruntCommonSpawnNode();

public:
    virtual void Init() override;

private :
    bool m_SpawnComplete;
    void SetWorldPosYUp();
public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

