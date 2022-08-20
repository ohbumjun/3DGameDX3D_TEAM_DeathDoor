#pragma once
#include "Component\Node\ActionNode.h"

enum class BossBettyCloseAttackType
{
    PunchLeft,
    PunchRight,
    SlashLeft,
    SlashRight,
    TwoSideFront,
    NotInCloseRange // 이때는, BackUp 을 해서, 방향을 Player 쪽으로 바꾸기 
};

// 근거리 공격
// 5가지 방향
// Slash Left, Slash Right, Left, Right, 그외 전방향 (양방향 공격)
// Slash 의 경우, 해당 방향으로 45도 회전 (서서히)
class CBossBettyCloseAttackNode :
    public CActionNode
{
public:
    CBossBettyCloseAttackNode();
    CBossBettyCloseAttackNode(const CBossBettyCloseAttackNode& Node);
    virtual ~CBossBettyCloseAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    BossBettyCloseAttackType m_CloseAttackType;
private :
    BossBettyCloseAttackType DetermineBettyCloseAttackType();
private :
    void OnBossBettySlashLeftEffect();
    void OnBossBettySlashRightEffect();
    void OnBossBettyCommonEndFunctionOfCloseAttack();
    void OnBossBettyCommonStartFunctionOfCloseAttack();
};

