#pragma once
#include "Component\Node\ActionNode.h"
class CBatRecognize :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBatRecognize();
    CBatRecognize(const CBatRecognize& Node);
    virtual ~CBatRecognize();


protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

};

