
#include "CrowBossCutScenePlayNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"
#include "Scene/CameraManager.h"
#include "../CrowBossDataComponent.h"

CCrowBossCutScenePlayNode::CCrowBossCutScenePlayNode() :
	m_AccTime(0.f),
	m_CutScenePrevDelay(0.3f)
{
	SetTypeID(typeid(CCrowBossCutScenePlayNode).hash_code());
}

CCrowBossCutScenePlayNode::CCrowBossCutScenePlayNode(const CCrowBossCutScenePlayNode& Node) :
	CActionNode(Node)
{
}

CCrowBossCutScenePlayNode::~CCrowBossCutScenePlayNode()
{
}

void CCrowBossCutScenePlayNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	//AnimInst->AddNotify("CutScene", "CutSceneStart", 0, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnStartCutScene);
	AnimInst->AddNotify("CutScene", "CameraShake", 62, this, &CCrowBossCutScenePlayNode::CrowBossCutSceneCameraShake);

	//AnimInst->AddNotify("CutScene", "PlayerEnterZoneFalse", 0, (CMonsterDataComponent*)Data, &CMonsterDataComponent::SetPlayerEnterZoneFalse);
	AnimInst->SetEndFunction("CutScene", (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEndCutScene);
}

NodeResult CCrowBossCutScenePlayNode::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (m_AccTime >= m_CutScenePrevDelay)
	{
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("CutScene");

		Data->SetPlayerEnterZone(false);
		Data->SetCutScenePlaying(true);

		return NodeResult::Node_True;
	}

	else
	{
		if (m_AccTime == 0.f)
		{
			CGameObject* PlayerObj = m_Object->GetScene()->GetPlayerObject();

			// 플레이어 입력 받지 않게 하고, 애니메이션 강제 변화
			if (PlayerObj)
			{
				CGameStateComponent* PlayerState = PlayerObj->FindComponentFromType<CGameStateComponent>();
				CAnimationSequenceInstance* PlayerAnim = PlayerObj->FindComponentFromType<CAnimationMeshComponent>()->GetAnimationInstance();

				PlayerAnim->ChangeAnimation("Idle");
				PlayerState->SetTreeUpdate(false);
				Data->GetPlayerEnterZoneTrigger()->Enable(false);
			}

			m_Object->GetScene()->GetCameraManager()->KeepCamera();
			m_Object->GetScene()->GetCameraManager()->SetCurrentCamera(Data->GetCutSceneCam());
		}

		m_CallStart = false;
		m_AccTime += DeltaTime;

		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossCutScenePlayNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossCutScenePlayNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

void CCrowBossCutScenePlayNode::CrowBossCutSceneCameraShake()
{
	m_Object->GetScene()->GetCameraManager()->ShakeCamera(1.f, 0.6f);
}
