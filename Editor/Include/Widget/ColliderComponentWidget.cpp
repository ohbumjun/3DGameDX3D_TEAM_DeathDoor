
#include "ColliderComponentWidget.h"
#include "IMGUIText.h"
#include "IMGUITree.h"
#include "IMGUIInputFloat3.h"
#include "Component/ColliderBox3D.h"
#include "IMGUISeperator.h"
#include "IMGUIComboBox.h"
#include "Collision/CollisionManager.h"
#include "IMGUIComboBox.h"

CColliderComponentWidget::CColliderComponentWidget()
{
}

CColliderComponentWidget::~CColliderComponentWidget()
{
}

bool CColliderComponentWidget::Init()
{
	CSceneComponentWidget::Init();

	m_ComponentTypeText->SetText("ColliderComponent Widget");

	m_ColliderComponentTree = AddWidget<CIMGUITree>("Collider Info");

	m_OffsetInput = m_ColliderComponentTree->AddWidget<CIMGUIInputFloat3>("Offset");
	m_CollisionProfileCombo = m_ColliderComponentTree->AddWidget<CIMGUIComboBox>("Profile");
	m_CollisionProfileCombo->SetHideName(true);
	m_CollisionProfileCombo->Sort(false);

	std::vector<std::string> vecProfileNames;

	CCollisionManager::GetInst()->GetProfileNames(vecProfileNames);

	size_t Count = vecProfileNames.size();

	for (size_t i = 0; i < Count; ++i)
		m_CollisionProfileCombo->AddItem(vecProfileNames[i]);

	m_OffsetInput->SetCallBack(this, &CColliderComponentWidget::OnChangeOffset);
	m_CollisionProfileCombo->SetSelectCallback(this, &CColliderComponentWidget::OnChangeProfile);

	AddWidget<CIMGUISeperator>("Sep");

	return true;
}

void CColliderComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CColliderComponent* ColliderComp = ((CColliderComponent*)m_Component);

	Vector3 Offset = ColliderComp->GetOffset();

	m_OffsetInput->SetX(Offset.x);
	m_OffsetInput->SetY(Offset.y);
	m_OffsetInput->SetZ(Offset.z);

	CollisionProfile* Profile = ColliderComp->GetCollisionProfile();
	std::string ProfileName;

	if (!Profile)
		ProfileName = "Object";

	else
		ProfileName = ColliderComp->GetCollisionProfile()->Name;

	int Index = (int)(m_CollisionProfileCombo->GetItemIndex(ProfileName));
	m_CollisionProfileCombo->SetSelectIndex(Index);

	std::vector<std::string> vecProfileNames;

	CCollisionManager::GetInst()->GetProfileNames(vecProfileNames);

	size_t Count = vecProfileNames.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (vecProfileNames[i] == ProfileName)
		{
			ColliderComp->SetCollisionProfile(vecProfileNames[i]);
			break;
		}
	}

	// TODO : Collision Profile 추가될 때 마다 추가
	/*if (ProfileName == "Object")
	{
		ColliderComp->SetCollisionProfile("Object");
	}

	else if (ProfileName == "Player")
	{
		ColliderComp->SetCollisionProfile("Player");
	}
	else if (ProfileName == "Monster")
	{
		ColliderComp->SetCollisionProfile("Monster");
	}

	else if (ProfileName == "PlayerAttack")
	{
		ColliderComp->SetCollisionProfile("PlayerAttack");
	}

	else if (ProfileName == "MonsterAttack")
	{
		ColliderComp->SetCollisionProfile("MonsterAttack");
	}

	else if (ProfileName == "MinionDetect")
	{
		ColliderComp->SetCollisionProfile("MinionDetect");
	}

	else if (ProfileName == "MinionNormalAttack")
	{
		ColliderComp->SetCollisionProfile("MinionNormalAttack");
	}

	else if (ProfileName == "MapObject")
	{
		ColliderComp->SetCollisionProfile("MapObject");
	}*/

	m_CollisionProfileCombo->SetSelectIndex(Index);
}

void CColliderComponentWidget::OnChangeOffset(const Vector3& Offset)
{
	((CColliderComponent*)m_Component)->SetOffset(Offset);
}

void CColliderComponentWidget::OnChangeProfile(int Index, const char* Label)
{
	((CColliderComponent*)m_Component)->SetCollisionProfile(Label);
}

void CColliderComponentWidget::RefreshCollisionProfile()
{
	m_CollisionProfileCombo->Clear();

	std::vector<std::string> vecProfileNames;

	CCollisionManager::GetInst()->GetProfileNames(vecProfileNames);

	size_t Count = vecProfileNames.size();

	for (size_t i = 0; i < Count; ++i)
		m_CollisionProfileCombo->AddItem(vecProfileNames[i]);
}
