#pragma once

#include "Node.h"

class CActionNode :
    public CNode
{
protected:
    CActionNode();
    CActionNode(const CActionNode& Node);
    virtual ~CActionNode() = 0;


public:
    virtual NodeResult OnStart(float DeltaTime) = 0;
    virtual NodeResult OnUpdate(float DeltaTime) = 0;
    virtual NodeResult OnEnd(float DeltaTime) = 0;

public:
    //virtual NodeResult OnStart(float DeltaTime);

//public:
//    void SetAnimationMeshComponent(class CAnimationMeshComponent* AnimationMeshComp);
//    class CAnimationMeshComponent* GetAnimationMeshComponent()  const;

public:
    virtual NodeResult Invoke(float DeltaTime) override;

};
