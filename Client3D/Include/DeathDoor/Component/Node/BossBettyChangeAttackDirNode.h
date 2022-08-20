#pragma once
#include "Component\Node\ActionNode.h"
class CBossBettyChangeAttackDirNode :
    public CActionNode
{
public:
    CBossBettyChangeAttackDirNode();
    CBossBettyChangeAttackDirNode(const CBossBettyChangeAttackDirNode& Node);
    virtual ~CBossBettyChangeAttackDirNode();
public:
    virtual void Init() override;
private :
    void OnResetOriginalRotSpeed();
protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

};

