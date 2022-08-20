#pragma once

#include "Component/Node/ConditionNode.h"

class CPlayerEnterZoneCheckNode :
    public CConditionNode
{
public:
    CPlayerEnterZoneCheckNode();
    CPlayerEnterZoneCheckNode(const CPlayerEnterZoneCheckNode& Node);
    virtual ~CPlayerEnterZoneCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

