#pragma once
#include "Component\Node\ActionNode.h"

enum class BossBettyCloseAttackType
{
    PunchLeft,
    PunchRight,
    SlashLeft,
    SlashRight,
    TwoSideFront,
    NotInCloseRange // �̶���, BackUp �� �ؼ�, ������ Player ������ �ٲٱ� 
};

// �ٰŸ� ����
// 5���� ����
// Slash Left, Slash Right, Left, Right, �׿� ������ (����� ����)
// Slash �� ���, �ش� �������� 45�� ȸ�� (������)
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

