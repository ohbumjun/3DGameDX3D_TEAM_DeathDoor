#pragma once

#include "Component/Node/ActionNode.h"

class CPlayerRoll :
    public CActionNode
{
public:
    CPlayerRoll();
    CPlayerRoll(const CPlayerRoll& Node);
    virtual ~CPlayerRoll();

private:
    class CNavAgent* m_NavAgent;

    bool m_SoundPlay;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

