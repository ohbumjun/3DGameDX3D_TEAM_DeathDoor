
#include "GameBehaviorTree.h"
#include "Component/Node/SelectorNode.h"
#include "Component/Node/SequenceNode.h"

// Public Nodes
#include "Node/MoveNode.h"
#include "Node/IdleNode.h"
#include "Node/MoveInputCheckNode.h"
#include "Node/NoInterruptNode.h"
#include "Node/NormalAttack.h"
#include "Node/MouseLButtonCheckNode.h"
#include "Node/MouseRButtonCheckNode.h"
#include "Node/MouseRButtonUpCheckNode.h"
#include "Node/NegateNode.h"
#include "Node/RotateAttackDirectionNode.h"
#include "Node/ReadyToShoot.h"
#include "Node/ShootNode.h"
#include "Node/CancleShootNode.h"
#include "Node/AddFallingFloorCallbackNode.h"
#include "Node/Lockstone3TriggerBoxHitCheck.h"
#include "Node/CheckDetectRangeNode.h"
#include "Node/FindPathNode.h"
#include "Node/Lockstone3TriggerBoxAction.h"
#include "Node/ClearPathListNode.h"
#include "Node/PathFindEnableCheck.h"
#include "Node/HitBackNode.h"
#include "Node/HitCheckNode.h"
#include "Node/RollInputCheck.h"
#include "Node/PlayerRoll.h"
#include "Node/ClimbDown.h"
#include "Node/ClimbDownStart.h"
#include "Node/ClimbDownEnd.h"
#include "Node/ClimbUp.h"
#include "Node/ClimbUpEnd.h"
#include "Node/ClimbUpStart.h"
#include "Node/StraightPathCheck.h"

#include "Node/IsClimbingCheck.h"
#include "Node/SKeyInputCheck.h"
#include "Node/WKeyInputCheck.h"
#include "Node/ClimbDownContinueCheck.h"
#include "Node/ClimbDownStartCheck.h"
#include "Node/ClimbDownEndCheck.h"
#include "Node/ClimbUpContinueCheck.h"
#include "Node/ClimbUpEndCheck.h"
#include "Node/ClimbingUpStartCheck.h"
#include "Node/ClimbPause.h"
#include "Node/ClimbKeyEnableCheck.h"

// Public Nodes
#include "Node/MeleeRangeCheckNode.h"
#include "Node/PostAttackDelayCheck.h"
#include "Node/AttackCoolTimeCheck.h"
#include "Node/DeathCheck.h"
#include "Node/IsCombatCheck.h"
#include "Node/DeathNode.h"
#include "Node/PlayerEnterZoneCheckNode.h"
#include "Node/PlayerEnterTriggerIsEnable.h"
#include "Node/ChasePlayerNode.h"
#include "Node/MonsterHitBack.h"
#include "Node/MeleeAttackNode.h"

// Boss - Knight
#include "Node/BossKnightContinueAttackNode.h"
#include "Node/BossKnightCutScenePlayNode.h"
#include "Node/BossKnightFinalAttackCheck.h"
#include "Node/BossKnightFinalAttackNode.h"
#include "Node/BossKnightJumpAttackRangeCheck.h"
#include "Node/BossKnightIdleNode.h"
#include "Node/BossKnightJumpAttackNode.h"
#include "Node/BossKnightMeleeAttackNode.h"
#include "Node/BossKnightSlamEnd.h"
#include "Node/BossKnightWalkNode.h"
#include "Node/BossKnightContinueAttackCheck.h"
#include "Node/BossKnightCutScenePlayCheck.h"
#include "Node/BossKnightFinalAttackCheck.h"
#include "Node/BossKnightJumpAttackRangeCheck.h"
#include "Node/BossKnightPlayerEnterZoneCheck.h"
#include "Node/UpdateInputQueue.h"
#include "Node/PathFindExecuteCheck.h"
#include "Node/PathListEmptyCheck.h"

// Boss - Betty
#include "Node/BossBettyAngryAttackNode.h"
#include "Node/BossBettyChangeAttackDirNode.h"
#include "Node/BossBettyCloseAttackNode.h"
#include "Node/BossBettyJumpAttackNode.h"
#include "Node/BossBettySpinAttackNode.h"
#include "Node/BossBettyThrowNode.h"
#include "Node/BossBettyIntroNode.h"
#include "Node/BossBettyIdleBeastNode.h"

#include "Node/BossBettyChangeDirCheck.h"
#include "Node/BossBettyFarAttackTypeCheck.h"
#include "Node/BossBettyHPStateCheck.h"
#include "Node/BossBettyThrowAttackCheck.h"
#include "Node/BossBettyIdleAnimCheck.h"
#include "Node/BossBettyIntroAnimCheck.h"

// Boss - CrowBoss
#include "Node/CrowBossCutScenePlayCheck.h"
#include "Node/CrowBossCutScenePlayNode.h"
#include "Node/CrowBossSpinNode.h"
#include "Node/CrowBossRunNode.h"
#include "Node/CrowBossPhasePickNode.h"
#include "Node/CrowBossBypassCheck.h"
#include "Node/CrowBossJumpStartCheck.h"
#include "Node/CrowBossJump.h"
#include "Node/CrowBossLandingCheck.h"
#include "Node/CrowBossJumpAttackRangeCheck.h"
#include "Node/CrowBossDoneCheck.h"
#include "Node/CrowBossFirstShootCheck.h"
#include "Node/CrowBossShootCheck.h"
#include "Node/CrowBossShootEndCheck.h"
#include "Node/CrowBossShootReadyCheck.h"
#include "Node/CrowBossFlyNode.h"
#include "Node/CrowBossMoveShootSpot.h"
#include "Node/CrowBossResetShootState.h"
#include "Node/CrowBossShootNode.h"
#include "Node/CrowBossSlide.h"
#include "Node/CrowBossSpitting.h"

// Monster - SporeBoomer
#include "Node/SporeBoomerShootNode.h"

// Monster - HeadRoller 
#include "Node/HeadRollerStunCheck.h"
#include "Node/HeadRollerRollNode.h"
#include "Node/HeadRollerStunNode.h"

// Monster - Dodger
#include "Node/DodgerDashCheck.h"
#include "Node/DodgerDashNode.h"
#include "Node/DodgerFirstAttackNode.h"
#include "Node/DodgerPostAttackNode.h"
#include "Node/DodgerSecondAttackCheck.h"
#include "Node/DodgerSecondAttackNode.h"

// Monster - Plauge Knight
#include "Node/PlagueKnightShootNode.h"

// Monster - Bat
#include "Node/BatRecognizeStartCheck.h"
#include "Node/BatRecognizeEndCheck.h"
#include "Node/BatRecognize.h"

// Monster - Lurker
#include "Node/LurkerHopStartCheck.h"
#include "Node/LurkerHop.h"
#include "Node/LurkerMeleeAttackPrep.h"

// Monster - Grunt - Condition
#include "Node/GruntCommonMoveStartCheckNode.h"
#include "Node/GruntCommonSpawnCheckNode.h"

// Monster - Grunt - Action
#include "Node/GruntCommonAttackNode.h"
#include "Node/GruntCommonHitNode.h"
#include "Node/GruntCommonSpawnNode.h"
#include "Node/GruntCommonTraceNode.h"
#include "Node/GruntCommonWalkNode.h"

#include "GameStateComponent.h"

CGameBehaviorTree::CGameBehaviorTree()
{
}

CGameBehaviorTree::CGameBehaviorTree(const CGameBehaviorTree& Tree) :
	CBehaviorTree(Tree)
{
}

CGameBehaviorTree::~CGameBehaviorTree()
{
}

bool CGameBehaviorTree::Init()
{
	CBehaviorTree::Init();
	return true;
}

void CGameBehaviorTree::Start()
{
	CBehaviorTree::Start();
}

CGameStateComponent* CGameBehaviorTree::GetOwner() const
{
	return dynamic_cast<CGameStateComponent*>(m_Owner);
}


CNode* CGameBehaviorTree::LoadNode(CNode* Parent, size_t TypeID)
{
	CNode* NewNode = nullptr;
	CGameObject* OwnerObject = m_Owner->GetGameObject();

	// TODO : Node 종류 추가될 때 마다 추가해주기
	// Public Node 이곳에 작성
	if (TypeID == typeid(CSelectorNode).hash_code())
	{
		NewNode = MakeNode<CSelectorNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CSequenceNode).hash_code())
	{
		NewNode = MakeNode<CSequenceNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CNoInterruptNode).hash_code())
	{
		NewNode = MakeNode<CNoInterruptNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CIdleNode).hash_code())
	{
		NewNode = MakeNode<CIdleNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMoveNode).hash_code())
	{
		CMoveNode* NewNode = new CMoveNode;
		NewNode->SetParent(Parent);
		NewNode->SetOwner(this);
		NewNode->SetObject(m_Owner->GetGameObject());
		NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

		CNavAgent* Agent = NewNode->GetGameObject()->FindObjectComponentFromType<CNavAgent>();

		if (Agent)
			NewNode->SetNavAgent(Agent);

		return NewNode;
	}

	else if (TypeID == typeid(CMoveInputCheckNode).hash_code())
	{
		NewNode = MakeNode<CMoveInputCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMouseLButtonCheckNode).hash_code())
	{
		NewNode = MakeNode<CMouseLButtonCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMouseRButtonCheckNode).hash_code())
	{
		NewNode = MakeNode<CMouseRButtonCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMouseRButtonUpCheckNode).hash_code())
	{
		NewNode = MakeNode<CMouseRButtonUpCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CReadyToShoot).hash_code())
	{
		NewNode = MakeNode<CReadyToShoot>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CShootNode).hash_code())
	{
		NewNode = MakeNode<CShootNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CCancleShootNode).hash_code())
	{
		NewNode = MakeNode<CCancleShootNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CNormalAttack).hash_code())
	{
		NewNode = MakeNode<CNormalAttack>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CRotateAttackDirectionNode).hash_code())
	{
		NewNode = MakeNode<CRotateAttackDirectionNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CNegateNode).hash_code())
	{
		NewNode = MakeNode<CNegateNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CAddFallingFloorCallbackNode).hash_code())
	{
		NewNode = MakeNode<CAddFallingFloorCallbackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CLockstone3TriggerBoxHitCheck).hash_code())
	{
		NewNode = MakeNode<CLockstone3TriggerBoxHitCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CLockstone3TriggerBoxAction).hash_code())
	{
		NewNode = MakeNode<CLockstone3TriggerBoxAction>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CCheckDetectRangeNode).hash_code())
	{
		NewNode = MakeNode<CCheckDetectRangeNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CFindPathNode).hash_code())
	{
		NewNode = MakeNode<CFindPathNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMeleeRangeCheckNode).hash_code())
	{
		NewNode = MakeNode<CMeleeRangeCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPostAttackDelayCheck).hash_code())
	{
		NewNode = MakeNode<CPostAttackDelayCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CAttackCoolTimeCheck).hash_code())
	{
		NewNode = MakeNode<CAttackCoolTimeCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CIsCombatCheck).hash_code())
	{
		NewNode = MakeNode<CIsCombatCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CDeathCheck).hash_code())
	{
		NewNode = MakeNode<CDeathCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CDeathNode).hash_code())
	{
		NewNode = MakeNode<CDeathNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClearPathListNode).hash_code())
	{
		NewNode = MakeNode<CClearPathListNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPathFindEnableCheck).hash_code())
	{
		NewNode = MakeNode<CPathFindEnableCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CHitCheckNode).hash_code())
	{
		NewNode = MakeNode<CHitCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CHitBackNode).hash_code())
	{
		NewNode = MakeNode<CHitBackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CRollInputCheck).hash_code())
	{
		NewNode = MakeNode<CRollInputCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CUpdateInputQueue).hash_code())
	{
		NewNode = MakeNode<CUpdateInputQueue>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPathFindExecuteCheck).hash_code())
	{
		NewNode = MakeNode<CPathFindExecuteCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPathListEmptyCheck).hash_code())
	{
		NewNode = MakeNode<CPathListEmptyCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPlayerRoll).hash_code())
	{
		NewNode = MakeNode<CPlayerRoll>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPlayerEnterZoneCheckNode).hash_code())
	{
		NewNode = MakeNode<CPlayerEnterZoneCheckNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CPlayerEnterTriggerIsEnable).hash_code())
	{
		NewNode = MakeNode<CPlayerEnterTriggerIsEnable>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CIsClimbingCheck).hash_code())
	{
		NewNode = MakeNode<CIsClimbingCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CSKeyInputCheck).hash_code())
	{
		NewNode = MakeNode<CSKeyInputCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CWKeyInputCheck).hash_code())
	{
		NewNode = MakeNode<CWKeyInputCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbDownContinueCheck).hash_code())
	{
		NewNode = MakeNode<CClimbDownContinueCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbDownStartCheck).hash_code())
	{
		NewNode = MakeNode<CClimbDownStartCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbDownEndCheck).hash_code())
	{
		NewNode = MakeNode<CClimbDownEndCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbUpContinueCheck).hash_code())
	{
		NewNode = MakeNode<CClimbUpContinueCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbingUpStartCheck).hash_code())
	{
		NewNode = MakeNode<CClimbingUpStartCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbUpEndCheck).hash_code())
	{
		NewNode = MakeNode<CClimbUpEndCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbPause).hash_code())
	{
		NewNode = MakeNode<CClimbPause>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CClimbKeyEnableCheck).hash_code())
	{
		NewNode = MakeNode<CClimbKeyEnableCheck>(Parent, OwnerObject);
	}

	//
	else if (TypeID == typeid(CClimbUp).hash_code())
	{
		NewNode = MakeNode<CClimbUp>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClimbUpStart).hash_code())
	{
		NewNode = MakeNode<CClimbUpStart>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClimbUpEnd).hash_code())
	{
		NewNode = MakeNode<CClimbUpEnd>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClimbDown).hash_code())
	{
		NewNode = MakeNode<CClimbDown>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClimbDownStart).hash_code())
	{
		NewNode = MakeNode<CClimbDownStart>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CClimbDownEnd).hash_code())
	{
		NewNode = MakeNode<CClimbDownEnd>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CStraightPathCheck).hash_code())
	{
		NewNode = MakeNode<CStraightPathCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CChasePlayerNode).hash_code())
	{
		NewNode = MakeNode<CChasePlayerNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMonsterHitBack).hash_code())
	{
	NewNode = MakeNode<CMonsterHitBack>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CMeleeAttackNode).hash_code())
	{
		NewNode = MakeNode<CMeleeAttackNode>(Parent, OwnerObject);
	}

	// Boss Knight - Action
	else if (TypeID == typeid(CBossKnightContinueAttackNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightContinueAttackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightCutScenePlayNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightCutScenePlayNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightFinalAttackNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightFinalAttackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightIdleNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightIdleNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightJumpAttackNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightJumpAttackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightMeleeAttackNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightMeleeAttackNode>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightSlamEnd).hash_code())
	{
		NewNode = MakeNode<CBossKnightSlamEnd>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightWalkNode).hash_code())
	{
		NewNode = MakeNode<CBossKnightWalkNode>(Parent, OwnerObject);
	}

	// Boss Knight - Condition
	else if (TypeID == typeid(CBossKnightContinueAttackCheck).hash_code())
	{
		NewNode = MakeNode<CBossKnightContinueAttackCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightCutScenePlayCheck).hash_code())
	{
		NewNode = MakeNode<CBossKnightCutScenePlayCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightFinalAttackCheck).hash_code())
	{
		NewNode = MakeNode<CBossKnightFinalAttackCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightJumpAttackRangeCheck).hash_code())
	{
		NewNode = MakeNode<CBossKnightJumpAttackRangeCheck>(Parent, OwnerObject);
	}

	else if (TypeID == typeid(CBossKnightPlayerEnterZoneCheck).hash_code())
	{
		NewNode = MakeNode<CBossKnightPlayerEnterZoneCheck>(Parent, OwnerObject);
	}
	
	// Boss Crow - Condition
	else if (TypeID == typeid(CCrowBossCutScenePlayCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossCutScenePlayCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossPhasePickNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossPhasePickNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossBypassCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossBypassCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossJumpStartCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossJumpStartCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossLandingCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossLandingCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossJumpAttackRangeCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossJumpAttackRangeCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossDoneCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossDoneCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossFirstShootCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossFirstShootCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossShootCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossShootCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossShootEndCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossShootEndCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossShootReadyCheck).hash_code())
	{
		NewNode = MakeNode<CCrowBossShootReadyCheck>(Parent, OwnerObject);
	}

	// Boss Crow - Action
	else if (TypeID == typeid(CCrowBossCutScenePlayNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossCutScenePlayNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossSpinNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossSpinNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossRunNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossRunNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossJump).hash_code())
	{
		NewNode = MakeNode<CCrowBossJump>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossFlyNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossFlyNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossMoveShootSpot).hash_code())
	{
		NewNode = MakeNode<CCrowBossMoveShootSpot>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossResetShootState).hash_code())
	{
		NewNode = MakeNode<CCrowBossResetShootState>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossShootNode).hash_code())
	{
		NewNode = MakeNode<CCrowBossShootNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossSlide).hash_code())
	{
		NewNode = MakeNode<CCrowBossSlide>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CCrowBossSpitting).hash_code())
	{
		NewNode = MakeNode<CCrowBossSpitting>(Parent, OwnerObject);
	}

	// SporeBoomer - Action
	else if (TypeID == typeid(CSporeBoomerShootNode).hash_code())
	{
		NewNode = MakeNode<CSporeBoomerShootNode>(Parent, OwnerObject);
	}

	// HeadRoller - Condition 
	else if (TypeID == typeid(CHeadRollerStunCheck).hash_code())
	{
		NewNode = MakeNode<CHeadRollerStunCheck>(Parent, OwnerObject);
	}
	// HeadRoller - Action
	else if (TypeID == typeid(CHeadRollerRollNode).hash_code())
	{
		NewNode = MakeNode<CHeadRollerRollNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CHeadRollerStunNode).hash_code())
	{
		NewNode = MakeNode<CHeadRollerStunNode>(Parent, OwnerObject);
	}

	// Dodger
	else if (TypeID == typeid(CDodgerDashCheck).hash_code())
	{
		NewNode = MakeNode<CDodgerDashCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CDodgerSecondAttackCheck).hash_code())
	{
		NewNode = MakeNode<CDodgerSecondAttackCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CDodgerDashNode).hash_code())
	{
		NewNode = MakeNode<CDodgerDashNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CDodgerFirstAttackNode).hash_code())
	{
		NewNode = MakeNode<CDodgerFirstAttackNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CDodgerPostAttackNode).hash_code())
	{
		NewNode = MakeNode<CDodgerPostAttackNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CDodgerSecondAttackNode).hash_code())
	{
		NewNode = MakeNode<CDodgerSecondAttackNode>(Parent, OwnerObject);
	}

	// Plague Knight
	else if (TypeID == typeid(CPlagueKnightShootNode).hash_code())
	{
		NewNode = MakeNode<CPlagueKnightShootNode>(Parent, OwnerObject);
	}

	// Bat
	else if (TypeID == typeid(CBatRecognizeStartCheck).hash_code())
	{
		NewNode = MakeNode<CBatRecognizeStartCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CBatRecognizeEndCheck).hash_code())
	{
		NewNode = MakeNode<CBatRecognizeEndCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CBatRecognize).hash_code())
	{
		NewNode = MakeNode<CBatRecognize>(Parent, OwnerObject);
	}

	// Lurker
	else if (TypeID == typeid(CLurkerHopStartCheck).hash_code())
	{
		NewNode = MakeNode<CLurkerHopStartCheck>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CLurkerHop).hash_code())
	{
		NewNode = MakeNode<CLurkerHop>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CLurkerMeleeAttackPrep).hash_code())
	{
		NewNode = MakeNode<CLurkerMeleeAttackPrep>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CLurkerMeleeAttackPrep).hash_code())
	{
	NewNode = MakeNode<CLurkerMeleeAttackPrep>(Parent, OwnerObject);
	}

	// BossBetty
	{
		// Boss Betty Action
		if (TypeID == typeid(CBossBettyAngryAttackNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyAngryAttackNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyChangeAttackDirNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyChangeAttackDirNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyCloseAttackNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyCloseAttackNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyJumpAttackNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyJumpAttackNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettySpinAttackNode).hash_code())
		{
		NewNode = MakeNode<CBossBettySpinAttackNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyThrowNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyThrowNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyIntroNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyIntroNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyIdleBeastNode).hash_code())
		{
		NewNode = MakeNode<CBossBettyIdleBeastNode>(Parent, OwnerObject);
		}
		// Boss Betty Condition
		else if (TypeID == typeid(CBossBettyChangeDirCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyChangeDirCheck>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyFarAttackTypeCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyFarAttackTypeCheck>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyHPStateCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyHPStateCheck>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyThrowAttackCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyThrowAttackCheck>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyIdleAnimCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyIdleAnimCheck>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CBossBettyIntroAnimCheck).hash_code())
		{
		NewNode = MakeNode<CBossBettyIntroAnimCheck>(Parent, OwnerObject);
		}
	}
	// Grunt Normal
	{
		if (TypeID == typeid(CGruntCommonSpawnCheckNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonSpawnCheckNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonMoveStartCheckNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonMoveStartCheckNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonAttackNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonAttackNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonHitNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonHitNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonSpawnNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonSpawnNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonTraceNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonTraceNode>(Parent, OwnerObject);
		}
		else if (TypeID == typeid(CGruntCommonWalkNode).hash_code())
		{
			NewNode = MakeNode<CGruntCommonWalkNode>(Parent, OwnerObject);
		}
	}

	return NewNode;
 }

	// GruntNormal
	//else if (TypeID == typeid(CGruntCommonMoveStartCheckNode).hash_code())
	//{
	//NewNode = MakeNode<CGruntCommonMoveStartCheckNode>(Parent, OwnerObject);
	//}
	/*
	else if (TypeID == typeid(CGruntCommonSpawnCheckNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonSpawnCheckNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CGruntCommonAttackNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonAttackNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CGruntCommonHitNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonHitNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CGruntCommonSpawnNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonSpawnNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CGruntCommonTraceNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonTraceNode>(Parent, OwnerObject);
	}
	else if (TypeID == typeid(CGruntCommonWalkNode).hash_code())
	{
	NewNode = MakeNode<CGruntCommonWalkNode>(Parent, OwnerObject);
	}
	*/

CNode* CGameBehaviorTree::LoadConditionNode(CNode* Parent, size_t TypeID)
{
	return nullptr;
}

CNode* CGameBehaviorTree::LoadActionNode(CNode* Parent, size_t TypeID)
{
	return nullptr;
}
