#pragma once
#include "Component\Node\ActionNode.h"

// ´« µ¢ÀÌ ´øÁö´Â Attack Node
class CBossBettyThrowNode :
    public CActionNode
{
public:
    CBossBettyThrowNode();
    CBossBettyThrowNode(const CBossBettyThrowNode& Node);
    virtual ~CBossBettyThrowNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
private :
    // ThrowBall
    class CGameObject* m_CurrentThrowBall;
    // Bazier
    float m_BazierMoveTargetDist;
    float m_BazierMoveCurDist;
    bool m_BazierMoveEffect;
    // Particle Move
    Vector3 m_ParticleNextMovePos;
    Vector3 m_ParticleMoveDir;
    float m_ParticleMoveAccTime;
    float m_ParticleMoveSpeed;
    float m_ParticleMoveInitSpeed;
    std::queue<Vector3> m_queueBazierMovePos;
private:
    void MakeSnowBallAttackObj();
    void ThrowSnowBallAttackObj();
};

