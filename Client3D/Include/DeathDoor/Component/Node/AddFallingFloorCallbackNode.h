#pragma once

#include "Component/Node/ActionNode.h"

class CAddFallingFloorCallbackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CAddFallingFloorCallbackNode();
    CAddFallingFloorCallbackNode(const CAddFallingFloorCallbackNode& Node);
    virtual ~CAddFallingFloorCallbackNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    bool m_CollisionStart;
    bool m_PlayerFall;
    float m_AccTime;
    Vector3 m_OriginPos;
    class CColliderBox3D* m_BoxCollider;
    float m_PrevXVibFactor;
    float m_PrevZVibFactor;
    int m_FrameCount;
    bool m_VibrateSoundPlayed;
    bool m_FallingSoundPlayed;


public:
    virtual NodeResult Invoke(float DeltaTime) override;

public:
    void Trigger(const CollisionResult& Result);
    void ResetFallingBlock();
    void OnPaperburnEnd();
    void Vibrate(float DeltaTime);
};

