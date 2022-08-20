#include "EditorManager.h"
#include "Engine.h"
#include "resource.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/DefaultScene.h"
#include "Scene/CameraManager.h"
#include "Input.h"
#include "IMGUIManager.h"
#include "Animation/AnimationSequence2DInstance.h"
#include "Render/RenderManager.h"
#include "Collision/CollisionManager.h"
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

// Window
#include "Window/ObjectHierarchyWindow.h"
#include "Window/SceneComponentHierarchyWindow.h"
#include "Window/ObjectComponentWindow.h"
#include "Window/FileBrowser.h"
#include "Window/FileBrowserTree.h"
#include "Window/AnimationEditor.h"
#include "Window/FBXConvertWindow.h"
#include "Window/InspectorWindow.h"
#include "Window/EffectEditor.h"
#include "Window/EffectDisplayWindow.h"
#include "Window/ToolWindow.h"
#include "Window/BehaviorTreeMenuBar.h"
#include "Window/BaseMenuBar.h"
#include "Window/MaterialEditor.h"
#include "Window/AnimationDisplayWindow.h"
#include "Window/ResourceDisplayWindow.h"
#include "Window/CollisionProfileEditor.h"
#include "Window/SocketWindow.h"
#include "Window/SceneModeWindow.h"
// Object
#include "Object/DragObject.h"
#include "Object/SpriteEditObject.h"
#include "Object/Player2D.h"
#include "Object/3DCameraObject.h"

#include "DeathDoor\Component/State/GameStateManager.h"
#include "Scene/Navigation3DManager.h"
#include "DeathDoor\DataManager.h"
#include "ObjectPool.h"

// SceneMode
#include "DeathDoor\Scene\DDSceneMode.h"
#include "DeathDoor\Scene\DDBossSceneMode.h"
#include "DeathDoor\Scene\DDInstanceSceneMode.h"
#include "DeathDoor\Scene\DDPuzzleSceneMode.h"
#include "DeathDoor/Scene/EndingScene.h"
#include "DeathDoor\Scene\DDLogoScene.h"

#include "DeathDoor/UI/UIManager.h"

#include <sstream>

DEFINITION_SINGLE(CEditorManager)

CEditorManager::CEditorManager() :
	m_EditMode(EditMode::Scene),
	m_DragObj(nullptr),
	m_CameraMoveSpeed(1000.f),
	m_CameraObject(nullptr),
	m_StateManager(nullptr)
{
}

CEditorManager::~CEditorManager()
{
	CEngine::DestroyInst();

	CDataManager::DestroyInst();

	CObjectPool::DestroyInst();

	CUIManager::DestroyInst();

	SAFE_DELETE(m_StateManager);
}

CGameStateManager* CEditorManager::GetStateManager() const
{
	return m_StateManager;
}

void CEditorManager::SetEditMode(EditMode Mode)
{
	m_EditMode = Mode;

	switch (m_EditMode)
	{
	case EditMode::Scene:
		break;
	case EditMode::Sprite:
		if (m_DragObj)
			m_DragObj->Destroy();
		m_DragObj = CSceneManager::GetInst()->GetScene()->CreateGameObject<CDragObject>("DragObject");
		m_DragObj->SetWorldScale(0.f, 0.f, 1.f);
		break;
	case EditMode::TileMap:
		if (m_DragObj)
		{
			m_DragObj->Destroy();
			m_DragObj = nullptr;
		}
		break;
	}

	if (m_EditMode == EditMode::Sprite)
	{
	}
}

bool CEditorManager::Init(HINSTANCE hInst)
{
	CEngine::GetInst()->EnableEditMode();

	if (!CEngine::GetInst()->Init(hInst, TEXT("GameEngine"),
		1280, 720, IDI_ICON1))
	{
		CEngine::DestroyInst();
		return false;
	}

	// Collsion Profile Load
	CCollisionManager::GetInst()->LoadProfileFromCSV("DeathDoorCollsionProfile.csv");

	// 에디터 키 생성
	CreateKey();

	// 카메라 등 에디터 오브젝트 생성
	CreateEditorObjects();

	// 콜백 
	SetEditorSceneCallBack();
	
	// 리소스 로드
	LoadEditorResources();

	// 각종 윈도우 생성
	CreateWindows();

	//m_StateManager = new CGameStateManager;

	//m_StateManager->Init();

	//CSceneManager::GetInst()->SetStateManager(m_StateManager);

	CDataManager::GetInst()->Init();
	
	CObjectPool::GetInst()->Init();

	CUIManager::GetInst()->Init();

	CObjectPool::GetInst()->LoadPoolData("ObjectPoolData.csv");
	CObjectPool::GetInst()->CreatePoolObject(OBJECT_PATH);


	return true;
}

void CEditorManager::CreateDefaultSceneMode()
{
	CSceneManager::GetInst()->CreateSceneMode<CDefaultScene>();
}

int CEditorManager::Run()
{
	return CEngine::GetInst()->Run();
}

void CEditorManager::MouseLButtonDown(float DeltaTime)
{
	if (m_DragObj)
	{
		m_DragObj->SetStartPos(CInput::GetInst()->GetMousePos());
	}
}

void CEditorManager::MouseLButtonPush(float DeltaTime)
{
	m_MousePush = true;

	if (m_DragObj)
	{
		m_DragObj->SetEndPos(CInput::GetInst()->GetMousePos());
	}
}

void CEditorManager::MouseLButtonUp(float DeltaTime)
{
	m_MousePush = false;
}

void CEditorManager::KeyboardUp(float DeltaTime)
{
	float Bias = CRenderManager::GetInst()->GetShadowBias();

	CRenderManager::GetInst()->SetShadowBias(Bias + DeltaTime);
 //	if (m_DragObj)
 //		m_DragObj->AddWorldPos(0.f, 1.f, 0.f);
 //
 //	else
 //	{
 //		CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();
 //
 //		Camera->AddWorldPos(Vector3(0.f, m_CameraMoveSpeed * DeltaTime, 0.f));
 //	}
}

void CEditorManager::KeyboardDown(float DeltaTime)
{
	float Bias = CRenderManager::GetInst()->GetShadowBias();
}

void CEditorManager::KeyboardLeft(float DeltaTime)
{
	if (m_DragObj)
		m_DragObj->AddWorldPos(-1.f, 0.f, 0.f);

	else
	{
		CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

		Camera->AddWorldPos(Vector3(-m_CameraMoveSpeed * DeltaTime, 0.f, 0.f));
	}
}

void CEditorManager::KeyboardRight(float DeltaTime)
{
	if (m_DragObj)
		m_DragObj->AddWorldPos(1.f, 0.f, 0.f);

	else
	{
		CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

		Camera->AddWorldPos(Vector3(m_CameraMoveSpeed * DeltaTime, 0.f, 0.f));
	}
}

void CEditorManager::CreateSceneMode(CScene* Scene, size_t Type)
{
	// TODO : DeathDoor Scene Mode Type 추가될 때마다 업데이트
	if (Type == typeid(CDefaultScene).hash_code())
	{
		Scene->CreateSceneMode<CDefaultScene>();
	}
	else if (Type == typeid(CDDSceneMode).hash_code())
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

CGameObject* CEditorManager::CreateObject(CScene* Scene, size_t Type)
{
	if (Type == typeid(CGameObject).hash_code())
	{
		CGameObject* Obj = Scene->LoadGameObject<CGameObject>();

		return Obj;
	}

	return nullptr;
}

CComponent* CEditorManager::CreateComponent(CGameObject* Obj, size_t Type)
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
		// Component->EnableEditMode(true);
		return Component;
	}
	else if (Type == typeid(CAnimationMeshComponent).hash_code())
	{
		CAnimationMeshComponent* Component = Obj->LoadComponent<CAnimationMeshComponent>();
		// Component->EnableEditMode(true);
		return Component;
	}
	else if (Type == typeid(CLandScape).hash_code())
	{
		CLandScape* Component = Obj->LoadComponent<CLandScape>();
		CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->SetLandScape(Component);
		// Component->EnableEditMode(true);
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
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CColliderSphere).hash_code())
	{
		CColliderSphere* Component = Obj->LoadComponent<CColliderSphere>();
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CColliderHalfLine).hash_code())
	{
		CColliderHalfLine* Component = Obj->LoadComponent<CColliderHalfLine>();
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CColliderRay).hash_code())
	{
		CColliderRay* Component = Obj->LoadComponent<CColliderRay>();
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CPlayerNormalAttackCheckCollider).hash_code())
	{
		CPlayerNormalAttackCheckCollider* Component = Obj->LoadComponent<CPlayerNormalAttackCheckCollider>();
		// Component->EnableEditMode(true);
		return Component;
	}

	else if (Type == typeid(CNavMeshComponent).hash_code())
	{
		CNavMeshComponent* Component = Obj->LoadComponent<CNavMeshComponent>();
		// Component->EnableEditMode(true);
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
		// Component->EnableEditMode(true);
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

void CEditorManager::CreateAnimInstance(CSpriteComponent* Sprite, size_t Type)
{
	if (Type == typeid(CAnimationSequence2DInstance).hash_code())
	{
		Sprite->LoadAnimationInstance<CAnimationSequence2DInstance>();
	}
}

void CEditorManager::CreateKey()
{
	CInput::GetInst()->CreateKey("MouseLButton", VK_LBUTTON);
	CInput::GetInst()->SetKeyCallback("MouseLButton", KeyState_Down, this, &CEditorManager::MouseLButtonDown);
	CInput::GetInst()->SetKeyCallback("MouseLButton", KeyState_Push, this, &CEditorManager::MouseLButtonPush);
	CInput::GetInst()->SetKeyCallback("MouseLButton", KeyState_Up, this, &CEditorManager::MouseLButtonUp);

	//CInput::GetInst()->CreateKey("Up", VK_UP);
	//CInput::GetInst()->CreateKey("Down", VK_DOWN);
	//CInput::GetInst()->CreateKey("Left", VK_LEFT);
	//CInput::GetInst()->CreateKey("Right", VK_RIGHT);

	//CInput::GetInst()->SetKeyCallback("Up", KeyState_Push, this, &CEditorManager::KeyboardUp);
	//CInput::GetInst()->SetKeyCallback("Down", KeyState_Push, this, &CEditorManager::KeyboardDown);
	//CInput::GetInst()->SetKeyCallback("Left", KeyState_Push, this, &CEditorManager::KeyboardLeft);
	//CInput::GetInst()->SetKeyCallback("Right", KeyState_Push, this, &CEditorManager::KeyboardRight);

	//CInput::GetInst()->CreateKey("SkillW", 'W');
	//CInput::GetInst()->CreateKey("SkillE", 'E');
	//CInput::GetInst()->CreateKey("SkillR", 'R');
	//CInput::GetInst()->CreateKey("SpellD", 'D');
	//CInput::GetInst()->CreateKey("SpellF", 'F');
}

void CEditorManager::CreateEditorObjects()
{
	m_CameraObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<C3DCameraObject>("EditorCamera");
	m_CameraObject->SetNoDestroyOnSceneChange(true);
}

void CEditorManager::SetEditorSceneCallBack()
{
	CSceneManager::GetInst()->SetCreateSceneModeFunction<CEditorManager>(this, &CEditorManager::CreateSceneMode);
	CSceneManager::GetInst()->SetCreateObjectFunction<CEditorManager>(this, &CEditorManager::CreateObject);
	//CSceneManager::GetInst()->SetObjectDataSetFunction<CDataManager>(m_DataManager, &CDataManager::SetObjectData);
	CSceneManager::GetInst()->SetCreateComponentFunction<CEditorManager>(this, &CEditorManager::CreateComponent);
	CSceneManager::GetInst()->SetCreateAnimInstanceFunction<CEditorManager>(this, &CEditorManager::CreateAnimInstance);
}

void CEditorManager::LoadEditorResources()
{
	CResourceManager::GetInst()->LoadTexture(DIRECTORY_IMAGE, TEXT("Directory.png"));
	CResourceManager::GetInst()->LoadTexture(FILE_IMAGE, TEXT("FileImage.png"));
}

void CEditorManager::CreateWindows()
{
	m_ObjectHierarchyWindow = CIMGUIManager::GetInst()->AddWindow<CObjectHierarchyWindow>(OBJECT_HIERARCHY);
	m_ComponentHierarchyWindow = CIMGUIManager::GetInst()->AddWindow<CSceneComponentHierarchyWindow>(SCENECOMPONENT_HIERARCHY);
	m_ObjectComponentWindow = CIMGUIManager::GetInst()->AddWindow<CObjectComponentWindow>(OBJECTCOMPONENT_LIST);

	m_InspectorWindow = CIMGUIManager::GetInst()->AddWindow<CInspectorWindow>(INSPECTOR);

	m_FileBrowserTree = CIMGUIManager::GetInst()->AddWindow<CFileBrowserTree>(FILE_BROWSERTREE);
	m_FileBrowserTree->Close();
	m_FileBrowser = CIMGUIManager::GetInst()->AddWindow<CFileBrowser>(FILE_BROWSER);
	m_FileBrowser->Close();
	
	m_AnimationEditor = CIMGUIManager::GetInst()->AddWindow<CAnimationEditor>(ANIMATION_EDITOR);
	m_AnimationEditor->Close();

	m_AnimationDisplayWindow = CIMGUIManager::GetInst()->AddWindow<CAnimationDisplayWindow>(ANIMATION_DISPLAYWINDW);
	m_AnimationDisplayWindow->Close();

	m_EffectEditor = CIMGUIManager::GetInst()->AddWindow<CEffectEditor>(PARTICLE_EDITOR);
	m_EffectEditor->Close();

	m_EffectDisplayWindow = CIMGUIManager::GetInst()->AddWindow<CEffectDisplayWindow>(PARTICLE_DISPLAYWINDOW);
	m_EffectDisplayWindow->Close();

	// (OBJ) 적어도 하나의 Window 는 계속 열려있어야 Error 발생 X
	m_ToolWindow = CIMGUIManager::GetInst()->AddWindow<CToolWindow>(TOOL);
	// m_ToolWindow->Close();

	m_FBXConvertWindow = CIMGUIManager::GetInst()->AddWindow<CFBXConvertWindow>(FBX_CONVERTOR);
	m_FBXConvertWindow->Close();

	m_BaseMenuBar = CIMGUIManager::GetInst()->AddWindow<CBaseMenuBar>("BehaviorTree");
	
	m_MaterialEditor = CIMGUIManager::GetInst()->AddWindow<CMaterialEditor>("MaterialEditor");
	m_MaterialEditor->Close();
	
	m_ResourceDisplayWindow = CIMGUIManager::GetInst()->AddWindow<CResourceDisplayWindow>("Resources");
	m_ResourceDisplayWindow->Close();

	m_CollisionProfileEditor = CIMGUIManager::GetInst()->AddWindow<CCollisionProfileEditor>(COLLISION_PROFILE);
	m_CollisionProfileEditor->Close();

	m_SocketWindow = CIMGUIManager::GetInst()->AddWindow<CSocketWindow>(SOCKET_WINDOW);
	m_SocketWindow->Close();

	m_SceneModeWindow = CIMGUIManager::GetInst()->AddWindow<CSceneModeWindow>(SCENEMODE_WINDOW);
	m_SceneModeWindow->Close();
}

void CEditorManager::CreateEditorCamera()
{

}
