
#include "CrowBossFlyNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../CrowBossDataComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"

CCrowBossFlyNode::CCrowBossFlyNode()
{
	SetTypeID(typeid(CCrowBossFlyNode).hash_code());
}

CCrowBossFlyNode::CCrowBossFlyNode(const CCrowBossFlyNode& Node) :
	CActionNode(Node)
{
}

CCrowBossFlyNode::~CCrowBossFlyNode()
{
}

NodeResult CCrowBossFlyNode::OnStart(float DeltaTime)
{
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Sliding");

	m_CallStart = true;
	return NodeResult::Node_True;
}

NodeResult CCrowBossFlyNode::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	Data->GetMeleeAttackCollider()->Enable(true);

	Data->GetHitBox()->SetRigidCollisionIgnore(true);
	Data->Fly(Data->GetShootDir(), DeltaTime);


	return NodeResult::Node_True;
}

NodeResult CCrowBossFlyNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}