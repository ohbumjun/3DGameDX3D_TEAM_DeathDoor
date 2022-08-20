#include "GameObjectWidget.h"
#include "GameObject/GameObject.h"
#include "IMGUIManager.h"
#include "../EditorInfo.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIDummy.h"
#include "IMGUIComboBox.h"
// SceneComponent
#include "Component/StaticMeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/Arm.h"
#include "Component/LightComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/LandScape.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderHalfLine.h"
#include "Component/ColliderRay.h"
#include "Component/NavMeshComponent.h"
#include "Component/WaterComponent.h"
#include "../DeathDoor/Component/LadderCollider.h"
#include "../DeathDoor/Component/CrackedBlockCollider.h"
// ObjectComponent
#include "Component/PaperBurnComponent.h"
#include "../DeathDoor/Component/TriggerBoxData.h"
// SceneCompWidget
#include "../DeathDoor/Component/PlayerHookComponent.h"
#include "../DeathDoor/Component/EyeLaserComponent.h"
#include "../DeathDoor/Component/PlayerNormalAttackCheckCollider.h"
#include "../Widget/StaticMeshComponentWidget.h"
#include "../Widget/LightComponentWidget.h"
#include "../Widget/ObjectComponentWidget.h"
#include "../Widget/ParticleComponentWidget.h"
#include "../Widget/AnimationMeshWidget.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "../Widget/LandScapeWidget.h"
#include "../Widget/NavMeshComponentWidget.h"
#include "../Widget/ColliderComponentWidget.h"
#include "../Widget/ColliderSphereWidget.h"
#include "../Widget/WaterComponentWidget.h"
#include "../Widget/CameraWidget.h"
#include "../Widget/ArmComponentWidget.h"

// ObjCompWidget
#include "../Widget/PaperBurnWidget.h"
#include "../Widget/EyeLaserComponentWidget.h"
#include "../Widget/TriggerBoxDataWidget.h"
#include "IMGUIManager.h"
#include "../EditorInfo.h"

CGameObjectWidget::CGameObjectWidget()	:
	m_NameInput(nullptr),
	m_RenameButton(nullptr),
	m_EnableCheckBox(nullptr)
{
}

CGameObjectWidget::~CGameObjectWidget()
{
}

bool CGameObjectWidget::Init()
{
	// AddWidget
	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("GameObject Name");

	AddWidget<CIMGUISameLine>("Line");

	m_EnableCheckBox = AddWidget<CIMGUICheckBox>("ObjEnable");
	m_EnableCheckBox->AddCheckInfo("Enable");

	m_NameInput = AddWidget<CIMGUITextInput>("Name");
	AddWidget<CIMGUISameLine>("Line");
	m_RenameButton = AddWidget<CIMGUIButton>("Rename", 0.f, 0.f);

	m_EnemyCheckBox = AddWidget<CIMGUICheckBox>("IsEnemy");
	m_EnemyCheckBox->AddCheckInfo("IsEnemy");

	m_ObjectTypeCombo = AddWidget<CIMGUIComboBox>("Object Type");
	m_ObjectTypeCombo->SetHideName(true);
	m_ObjectTypeCombo->AddItem("Player");
	m_ObjectTypeCombo->AddItem("Monster");
	m_ObjectTypeCombo->AddItem("MapObject");
	m_ObjectTypeCombo->AddItem("Particle");
	m_ObjectTypeCombo->AddItem("Projectile");

	AddWidget<CIMGUISeperator>("Sep");

	// CallBack
	m_RenameButton->SetClickCallback(this, &CGameObjectWidget::OnClickRenameButton);
	m_EnableCheckBox->SetCallBackIdx(this, &CGameObjectWidget::OnCheckEnableCheckBox);
	m_EnemyCheckBox->SetCallBackIdx(this, &CGameObjectWidget::OnCheckEnemyCheckBox);
	m_ObjectTypeCombo->SetSelectCallback<CGameObjectWidget>(this, &CGameObjectWidget::OnSelectObjectType);

	return true;
}

void CGameObjectWidget::ClearComponentWidget()
{
	auto iter = m_SceneComponentWidgetList.begin();
	auto iterEnd = m_SceneComponentWidgetList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}

	m_SceneComponentWidgetList.clear();

	auto iterO = m_ObjectComponentWidgetList.begin();
	auto iterOEnd = m_ObjectComponentWidgetList.end();

	for (; iterO != iterOEnd; ++iterO)
	{
		SAFE_DELETE(*iterO);
	}

	m_ObjectComponentWidgetList.clear();
	
	mVecChild.resize(9);
}

void CGameObjectWidget::SetGameObject(CGameObject* Obj)
{
	if (!Obj)
		return;

	m_Object = Obj;

	if (m_Object->IsEnemy())
		m_EnemyCheckBox->SetCheck(0, true);
	else
		m_EnemyCheckBox->SetCheck(0, false);

	if (Obj)
	{
		m_EnableCheckBox->SetCheck(0, m_Object->IsEnable());
		m_NameInput->SetText(m_Object->GetName().c_str());

		// Component Pointer들 받아와 Type에 맞는 Widget들 생성
		std::vector<CSceneComponent*> vecSceneComp;
		m_Object->GetAllSceneComponentsPointer(vecSceneComp);

		size_t Size = vecSceneComp.size();
		for (size_t i = 0; i < Size; ++i)
		{
			CreateSceneComponentWidget(vecSceneComp[i]);
		}

		std::vector<CObjectComponent*> vecObjComp;
		m_Object->GetAllObjectComponentsPointer(vecObjComp);

		Size = vecObjComp.size();
		for (size_t i = 0; i < Size; ++i)
		{
			CreateObjectComponentWidget(vecObjComp[i]);
		}

		Object_Type Type = Obj->GetObjectType();

		switch (Type)
		{
		case Object_Type::Player:
			m_ObjectTypeCombo->SetSelectIndex(0);
			break;
		case Object_Type::Monster:
			m_ObjectTypeCombo->SetSelectIndex(1);
			break;
		case Object_Type::MapObject:
			m_ObjectTypeCombo->SetSelectIndex(2);
			break;
		case Object_Type::Particle:
			m_ObjectTypeCombo->SetSelectIndex(3);
			break;
		case Object_Type::Projectile:
			m_ObjectTypeCombo->SetSelectIndex(4);
			break;
		}
	}
}

void CGameObjectWidget::CreateSceneComponentWidget(CSceneComponent* Com)
{
	size_t TypeID = Com->GetTypeID();

	CSceneComponentWidget* Widget = nullptr;

	// TODO : 컴포넌트별 위젯 추가
	if (TypeID == typeid(CStaticMeshComponent).hash_code() || TypeID == typeid(CPlayerHookComponent).hash_code())
	{
	 	Widget = AddWidget<CStaticMeshComponentWidget>("StaticMeshWidget");
	}
	else if (TypeID == typeid(CAnimationMeshComponent).hash_code())
	{
		Widget = AddWidget<CAnimationMeshWidget>("AnimationMeshWidget");
	}
	else if (TypeID == typeid(CCameraComponent).hash_code())
	{
		Widget = AddWidget<CCameraWidget>("CameraWidget");
	}
	else if (TypeID == typeid(CArm).hash_code())
	{
		Widget = AddWidget<CArmComponentWidget>("ArmWidget");
	}
	else if (TypeID == typeid(CLightComponent).hash_code())
	{
	 	Widget = AddWidget<CLightComponentWidget>("LightWidget");
	}
	else if (TypeID == typeid(CParticleComponent).hash_code())
	{
		Widget = AddWidget<CParticleComponentWidget>("ParticleWidget");
	}
	else if (TypeID == typeid(CLandScape).hash_code())
	{
		Widget = AddWidget<CLandScapeWidget>("LandScapeWidget");
	}
	else if (TypeID == typeid(CColliderBox3D).hash_code() ||
		TypeID == typeid(CColliderBox3D).hash_code() || 
		TypeID == typeid(CColliderHalfLine).hash_code() || 
		TypeID == typeid(CColliderRay).hash_code() ||
		TypeID == typeid(CLadderCollider).hash_code() ||
		TypeID == typeid(CCrackedBlockCollider).hash_code())
	{
		Widget = AddWidget<CColliderComponentWidget>("ColliderComponentWidget");
	}

	else if (TypeID == typeid(CColliderSphere).hash_code() ||  TypeID == typeid(CPlayerNormalAttackCheckCollider).hash_code())
	{
		Widget = AddWidget<CColliderSphereWidget>("ColliderSphereWidget");
	}
	else if (TypeID == typeid(CNavMeshComponent).hash_code())
	{
		Widget = AddWidget<CNavMeshComponentWidget>("NavMeshComponentWidget");
	}
	else if (TypeID == typeid(CWaterComponent).hash_code())
	{
		Widget = AddWidget<CWaterComponentWidget>("WaterComponentWidget");
	}
	else if (TypeID == typeid(CEyeLaserComponent).hash_code())
	{
		Widget = AddWidget<CEyeLaserComponentWidget>("EyeLaserComponentWidget");
	}

	else
	{
	 	Widget = AddWidget<CSceneComponentWidget>("SceneWidget");
	}

	// Component 넣어주면서 내부 Widget들 생성
	Widget->SetSceneComponent(Com);

	m_SceneComponentWidgetList.push_back(Widget);
}

void CGameObjectWidget::CreateObjectComponentWidget(CObjectComponent* Com)
{
	size_t TypeID = Com->GetTypeID();

	CObjectComponentWidget* Widget = nullptr;

	if (TypeID == typeid(CPaperBurnComponent).hash_code())
	{
		Widget = AddWidget<CPaperBurnWidget>("PaperBurnWidget");
	}

	else if (TypeID == typeid(CTriggerBoxData).hash_code())
	{
		Widget = AddWidget<CTriggerBoxDataWidget>("TriggerBoxDataWidget");
	}

	else
	{
		Widget = AddWidget<CObjectComponentWidget>("DefaultObjCompWidget");
	}

	if (Widget)
	{
		// Component 넣어주면서 내부 Widget들 생성
		Widget->SetObjectComponent(Com);

		m_ObjectComponentWidgetList.push_back(Widget);
	}
}

void CGameObjectWidget::DeleteSceneComponentWidget(CSceneComponent* Com)
{
	auto iter = m_SceneComponentWidgetList.begin();
	auto iterEnd = m_SceneComponentWidgetList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsMyWidget(Com))
		{
			DeleteWidget((*iter));
			m_SceneComponentWidgetList.erase(iter);
			break;
		}
	}
}

bool CGameObjectWidget::FindSceneComponent(CSceneComponent* Com)
{
	auto iter = m_SceneComponentWidgetList.begin();
	auto iterEnd = m_SceneComponentWidgetList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetComponent() == Com)
			return true;
	}

	return false;
}

void CGameObjectWidget::OnClickRenameButton()
{
	m_Object->SetName(m_NameInput->GetTextMultibyte());

	// Hierachy 갱신
	CObjectHierarchyWindow* Hierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	Hierachy->OnRenameObject(m_NameInput->GetTextMultibyte());
}

void CGameObjectWidget::OnCheckEnableCheckBox(int Idx, bool Check)
{
	m_Object->Enable(Check);

	auto iter = m_SceneComponentWidgetList.begin();
	auto iterEnd = m_SceneComponentWidgetList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->OnGameObjectEnable(Check);
	}

	auto iterO = m_ObjectComponentWidgetList.begin();
	auto iterOEnd = m_ObjectComponentWidgetList.end();

	for (; iterO != iterOEnd; ++iterO)
	{
		(*iterO)->OnGameObjectEnable(Check);
	}
}

void CGameObjectWidget::OnCheckEnemyCheckBox(int Idx, bool Check)
{
	m_Object->SetEnemy(Check);
}

void CGameObjectWidget::OnSelectObjectType(int Idx, const char* label)
{
	const std::string& ObjectType = m_ObjectTypeCombo->GetItem(Idx);

	if (ObjectType == "Player")
	{
		m_Object->SetObjectType(Object_Type::Player);
	}

	else if (ObjectType == "Monster")
	{
		m_Object->SetObjectType(Object_Type::Monster);
	}

	else if (ObjectType == "MapObject")
	{
		m_Object->SetObjectType(Object_Type::MapObject);
	}

	else if (ObjectType == "Particle")
	{
		m_Object->SetObjectType(Object_Type::Particle);
	}

	else if (ObjectType == "Projectile")
	{
		m_Object->SetObjectType(Object_Type::Projectile);
	}
}
