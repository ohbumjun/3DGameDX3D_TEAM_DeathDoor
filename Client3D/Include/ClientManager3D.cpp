#include "ClientManager3D.h"
#include "Engine.h"
#include "resource.h"
#include "Scene/SceneManager.h"
#include "Input.h"
#include "Resource/ResourceManager.h"
#include "Collision/CollisionManager.h"
#include "DeathDoor/UI/UIManager.h"

// Component
#include "Component/CameraComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/LandScape.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderHalfLine.h"
#include "Component/ColliderRay.h"
#include "Component/NavMeshComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Component/WaterComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Component/Arm.h"
#include "DeathDoor/Component/ObjectDataComponent.h"
#include "DeathDoor/Component/PlayerDataComponent.h"
#include "DeathDoor/Component\GameStateComponent.h"
#include "DeathDoor\Component/PlayerNormalAttackCheckCollider.h"
#include "DeathDoor\Component/EyeLaserComponent.h"
#include "DeathDoor\Component/PlayerHookComponent.h"
#include "DeathDoor\Component/KnightDataComponent.h"
#include "DeathDoor\Component/BossBettyDataComponent.h"
#include "DeathDoor\Component/GruntCommonDataComponent.h"
#include "DeathDoor\Component/CrowBossDataComponent.h"
#include "DeathDoor\Component/LurkerDataComponent.h"
#include "DeathDoor\Component/MonsterNavAgent.h"
#include "DeathDoor\Component/PlayerBowComponent.h"
#include "DeathDoor\Component/ProjectileComponent.h"
#include "DeathDoor\Component/ArrowComponent.h"
#include "DeathDoor\Component/TinyCrowDataComponent.h"
#include "DeathDoor\Component/PlayerBombComponent.h"
#include "DeathDoor\Component/LadderCollider.h"
#include "DeathDoor\Component/SporeBoomerDataComponent.h"
#include "DeathDoor\Component/CrackedBlockCollider.h"
#include "DeathDoor\Component/ArrowCollisionFireCollider.h"
#include "DeathDoor\Component/FirePlantDataComponent.h"
#include "DeathDoor\Component/MonsterBulletData.h"
#include "DeathDoor\Component/HeadRollerDataComponent.h"
#include "DeathDoor\Component/DodgerDataComponent.h"
#include "DeathDoor\Component/PlagueKnightDataComponent.h"
#include "DeathDoor\Component/DodgerDataComponent.h"
#include "DeathDoor\Component/TriggerBoxData.h"
#include "DeathDoor\Component/TriggerFireLamp.h"
#include "DeathDoor\Component/BatDataComponent.h"

// SceneMode
#include "DeathDoor\Scene\DDSceneMode.h"
#include "DeathDoor\Scene\DDBossSceneMode.h"
#include "DeathDoor\Scene\DDInstanceSceneMode.h"
#include "DeathDoor\Scene\DDPuzzleSceneMode.h"
#include "DeathDoor\Scene\DDLogoScene.h"
#include "DeathDoor\Scene\EndingScene.h"

#include "Scene/Navigation3DManager.h"
#include "DeathDoor\DataManager.h"
#include "ObjectPool.h"
#include <sstream>

DEFINITION_SINGLE(CClientManager3D)

CClientManager3D::CClientManager3D()
{
}

CClientManager3D::~CClientManager3D()
{
	CEngine::DestroyInst();
	CDataManager::DestroyInst();
	CObjectPool::DestroyInst();
}

bool CClientManager3D::Init(HINSTANCE hInst)
{
	if (!CEngine::GetInst()->Init(hInst, TEXT("GameEngine"),
		1280, 720, IDI_ICON1))
	{
		CEngine::DestroyInst();
		return false;
	}

	// CallBack
	CSceneManager::GetInst()->SetCreateSceneModeFunction<CClientManager3D>(this, &CClientManager3D::CreateSceneMode);
	CSceneManager::GetInst()->SetCreateObjectFunction<CClientManager3D>(this, &CClientManager3D::CreateObject);
	CSceneManager::GetInst()->SetCreateComponentFunction<CClientManager3D>(this, &CClientManager3D::CreateComponent);

	// Collsion Profile Load
	CCollisionManager::GetInst()->LoadProfileFromCSV("DeathDoorCollsionProfile.csv");
	
	// Data Manager
	CDataManager::GetInst()->Init();

	// UI Manager
	CUIManager::GetInst()->Init();
	
	// Object Pool
	CObjectPool::GetInst()->Init();
	CObjectPool::GetInst()->LoadPoolData("ObjectPoolData.csv");
	CObjectPool::GetInst()->CreatePoolObject(OBJECT_PATH);

	CInput::GetInst()->CreateKey("DebugRender", '0');
	CInput::GetInst()->SetKeyCallback("DebugRender", KeyState_Down, this, &CClientManager3D::SetDebugRender);

	return true;
}

void CClientManager3D::CreateDefaultSceneMode()
{
}

void CClientManager3D::LoadInitialScene()
{
	// TODO : Initial Scene 교체
	CSceneManager::GetInst()->LoadNewScene("MapScene/Logo.scn", SCENE_PATH, true);
}

int CClientManager3D::Run()
{
	return CEngine::GetInst()->Run();
}

void CClientManager3D::CreateSceneMode(CScene* Scene, size_t Type)
{
	if (Type == typeid(CDDSceneMode).hash_code())
	{
		Scene->CreateSceneMode<CDDSceneMode>();
	}
	else if (Type == typeid(CDDBossSceneMode).hash_code())
	{
		Scene->CreateSceneMode<CDDBossSceneMode>();
	}
	else if (Type == typeid(CDDInstanceSceneMode).hash_code())
	{
		Scene->CreateSceneMode<CDDInstanceSceneMode>();
	}
	else if (Type == typeid(CDDPuzzleSceneMode).hash_code())
	{
		Scene->CreateSceneMode<CDDPuzzleSceneMode>();
	}
	else if (Type == typeid(CEndingScene).hash_code())
	{
		Scene->CreateSceneMode<CEndingScene>();
	}
	else if (Type == typeid(CDDLogoScene).hash_code())
	{
		Scene->CreateSceneMode<CDDLogoScene>();
	}
}

CGameObject* CClientManager3D::CreateObject(CScene* Scene, size_t Type)
{
	if (Type == typeid(CGameObject).hash_code())
	{
		CGameObject* Obj = Scene->LoadGameObject<CGameObject>();

		return Obj;
	}

	return nullptr;
}

CComponent* CClientManager3D::CreateComponent(CGameObject* Obj, size_t Type)
{
	// TODO : Component 추가될 때 마다 추가
	if (Type == typeid(CSceneComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CSceneComponent>();
		return Component;
	}
	else if (Type == typeid(CSpriteComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CSpriteComponent>();
		return Component;
	}
	else if (Type == typeid(CArm).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CArm>();
		return Component;
	}
	else if (Type == typeid(CStaticMeshComponent).hash_code())
	{
		CComponent* Component = Obj->LoadComponent<CStaticMeshComponent>();
		return Component;
	}
	else if (Type == typeid(CTileMapComponent).hash_code())
	{
		CTileMapComponent* Component = Obj->LoadComponent<CTileMapComponent>();
		Component->EnableEditMode(true);
		return Component;
	}
	else if (Type == typeid(CParticleComponent).hash_code())
	{
		CParticleComponent* Component = Obj->LoadComponent<CParticleComponent>();
		return Component;
	}
	else if (Type == typeid(CAnimationMeshComponent).hash_code())
	{
		CAnimationMeshComponent* Component = Obj->LoadComponent<CAnimationMeshComponent>();
		return Component;
	}
	else if (Type == typeid(CLandScape).hash_code())
	{
		CLandScape* Component = Obj->LoadComponent<CLandScape>();
		CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->SetLandScape(Component);
		return Component;
	}

	else if (Type == typeid(CGameStateComponent).hash_code())
	{
		CGameStateComponent* Component = Obj->LoadObjectComponent<CGameStateComponent>();
		return Component;
	}

	else if (Type == typeid(CCameraComponent).hash_code())
	{
		CCameraComponent* Component = Obj->LoadComponent<CCameraComponent>();
		return Component;
	}

	else if (Type == typeid(CNavAgent).hash_code())
	{
		CNavAgent* Component = Obj->LoadObjectComponent<CNavAgent>();
		return Component;
	}

	else if (Type == typeid(CColliderBox3D).hash_code())
	{
		CColliderBox3D* Component = Obj->LoadComponent<CColliderBox3D>();
		return Component;
	}

	else if (Type == typeid(CColliderSphere).hash_code())
	{
		CColliderSphere* Component = Obj->LoadComponent<CColliderSphere>();
		return Component;
	}

	else if (Type == typeid(CColliderHalfLine).hash_code())
	{
		CColliderHalfLine* Component = Obj->LoadComponent<CColliderHalfLine>();
		return Component;
	}

	else if (Type == typeid(CColliderRay).hash_code())
	{
		CColliderRay* Component = Obj->LoadComponent<CColliderRay>();
		return Component;
	}

	else if (Type == typeid(CPlayerNormalAttackCheckCollider).hash_code())
	{
		CPlayerNormalAttackCheckCollider* Component = Obj->LoadComponent<CPlayerNormalAttackCheckCollider>();
		return Component;
	}

	else if (Type == typeid(CNavMeshComponent).hash_code())
	{
		CNavMeshComponent* Component = Obj->LoadComponent<CNavMeshComponent>();
		return Component;
	}

	else if (Type == typeid(CEyeLaserComponent).hash_code())
	{
		CEyeLaserComponent* Component = Obj->LoadComponent<CEyeLaserComponent>();
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CPlayerHookComponent).hash_code())
	{
		CPlayerHookComponent* Component = Obj->LoadComponent<CPlayerHookComponent>();
		// Component->EnableEditMode(true);
		return Component;
	}


	else if (Type == typeid(CLightComponent).hash_code())
	{
		CLightComponent* Component = Obj->LoadComponent<CLightComponent>();
		return Component;
	}

	else if (Type == typeid(CLadderCollider).hash_code())
	{
		CLadderCollider* Component = Obj->LoadComponent<CLadderCollider>();
		return Component;
	}

	else if (Type == typeid(CArrowCollisionFireCollider).hash_code())
	{
		CArrowCollisionFireCollider* Component = Obj->LoadComponent<CArrowCollisionFireCollider>();
		return Component;
	}

	// 아래로는 Object Data Component =================
	else if (Type == typeid(CObjectDataComponent).hash_code())
	{
		CObjectDataComponent* Component = Obj->LoadObjectComponent<CObjectDataComponent>();

	return Component;
	}
	else if (Type == typeid(CPlayerDataComponent).hash_code())
	{
		CPlayerDataComponent* Component = Obj->LoadObjectComponent<CPlayerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CPaperBurnComponent).hash_code())
	{
		CPaperBurnComponent* Component = Obj->LoadObjectComponent<CPaperBurnComponent>();
		return Component;
	}

	else if (Type == typeid(CWaterComponent).hash_code())
	{
		CWaterComponent* Component = Obj->LoadComponent<CWaterComponent>();
		return Component;
	}

	else if (Type == typeid(CPlayerBowComponent).hash_code())
	{
		CPlayerBowComponent* Component = Obj->LoadComponent<CPlayerBowComponent>();
		return Component;
	}

	else if (Type == typeid(CLurkerDataComponent).hash_code())
	{
		CLurkerDataComponent* Component = Obj->LoadObjectComponent<CLurkerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CMonsterNavAgent).hash_code())
	{
		CMonsterNavAgent* Component = Obj->LoadObjectComponent<CMonsterNavAgent>();
		return Component;
	}

	else if (Type == typeid(CKnightDataComponent).hash_code())
	{
		CKnightDataComponent* Component = Obj->LoadObjectComponent<CKnightDataComponent>();
		return Component;
	}

	else if (Type == typeid(CProjectileComponent).hash_code())
	{
		CProjectileComponent* Component = Obj->LoadObjectComponent<CProjectileComponent>();
		return Component;
	}

	else if (Type == typeid(CBossBettyDataComponent).hash_code())
		{
		CBossBettyDataComponent* Component = Obj->LoadObjectComponent<CBossBettyDataComponent>();
		return Component;
	}

	else if (Type == typeid(CCrowBossDataComponent).hash_code())
	{
		CCrowBossDataComponent* Component = Obj->LoadObjectComponent<CCrowBossDataComponent>();
		return Component;
	}

	else if (Type == typeid(CArrowComponent).hash_code())
	{
		CArrowComponent* Component = Obj->LoadObjectComponent<CArrowComponent>();
		return Component;
	}

	else if (Type == typeid(CTinyCrowDataComponent).hash_code())
	{
		CTinyCrowDataComponent* Component = Obj->LoadObjectComponent<CTinyCrowDataComponent>();
		return Component;
	}

	else if (Type == typeid(CPlayerBombComponent).hash_code())
	{
		CPlayerBombComponent* Component = Obj->LoadObjectComponent<CPlayerBombComponent>();
		return Component;
	}

	else if (Type == typeid(CSporeBoomerDataComponent).hash_code())
	{
		CSporeBoomerDataComponent* Component = Obj->LoadObjectComponent<CSporeBoomerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CCrackedBlockCollider).hash_code())
	{
		CCrackedBlockCollider* Component = Obj->LoadComponent<CCrackedBlockCollider>();
		return Component;
	}

	else if (Type == typeid(CFirePlantDataComponent).hash_code())
	{
		CFirePlantDataComponent* Component = Obj->LoadObjectComponent<CFirePlantDataComponent>();
		return Component;
	}

	else if (Type == typeid(CMonsterBulletData).hash_code())
	{
		CMonsterBulletData* Component = Obj->LoadObjectComponent<CMonsterBulletData>();
		return Component;
	}

	else if (Type == typeid(CHeadRollerDataComponent).hash_code())
	{
		CHeadRollerDataComponent* Component = Obj->LoadObjectComponent<CHeadRollerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CDodgerDataComponent).hash_code())
	{
		CDodgerDataComponent* Component = Obj->LoadObjectComponent<CDodgerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CTriggerBoxData).hash_code())
	{
		CTriggerBoxData* Component = Obj->LoadObjectComponent<CTriggerBoxData>();
		return Component;
	}

	else if (Type == typeid(CTriggerFireLamp).hash_code())
	{
		CTriggerFireLamp* Component = Obj->LoadObjectComponent<CTriggerFireLamp>();
		return Component;
	}

	else if (Type == typeid(CDodgerDataComponent).hash_code())
	{
		CDodgerDataComponent* Component = Obj->LoadObjectComponent<CDodgerDataComponent>();
		return Component;
	}

	else if (Type == typeid(CPlagueKnightDataComponent).hash_code())
	{
		CPlagueKnightDataComponent* Component = Obj->LoadObjectComponent<CPlagueKnightDataComponent>();
		return Component;
	}

	else if (Type == typeid(CBatDataComponent).hash_code())
	{
		CBatDataComponent* Component = Obj->LoadObjectComponent<CBatDataComponent>();
		return Component;
	}

	else if (Type == typeid(CGruntCommonDataComponent).hash_code())
	{
		CGruntCommonDataComponent* Component = Obj->LoadObjectComponent<CGruntCommonDataComponent>();
		return Component;
	}

	return nullptr;
}

void CClientManager3D::SetDebugRender(float DeltaTime)
{
	bool Debug = CRenderManager::GetInst()->GetDebugRender();

	if (Debug)
		CRenderManager::GetInst()->SetDebugRender(false);
	else
		CRenderManager::GetInst()->SetDebugRender(true);
}
