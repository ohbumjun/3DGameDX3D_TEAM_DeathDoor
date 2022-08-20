#include "DDBossSceneMode.h"
#include "Component/PaperBurnComponent.h"
#include "GameObject/GameObject.h"
#include "Component/ColliderBox3D.h"
#include "Render/RenderManager.h"

CDDBossSceneMode::CDDBossSceneMode()
{
	SetTypeID<CDDBossSceneMode>();
}

CDDBossSceneMode::~CDDBossSceneMode()
{
}

void CDDBossSceneMode::Start()
{
	CDDSceneMode::Start();

	if (m_ExitPointObj)
	{
		m_OriginExitPos = m_ExitPointObj->GetWorldPos();
		m_PortalPaperBurn = m_ExitPointObj->FindObjectComponentFromType<CPaperBurnComponent>();
	}

	if (m_ExitPointObj)
	{
		m_ExitPointObj->Enable(false);
	}
}

bool CDDBossSceneMode::SetExitPointObj(CGameObject* Obj)
{
	if (!CDDSceneMode::SetExitPointObj(Obj))
	{
		return false;
	}

	// 페이퍼번 컴포넌트가 없는 경우 Exit Point Object로 지정하지 못함
	m_PortalPaperBurn = Obj->FindObjectComponentFromType<CPaperBurnComponent>();

	if (!m_PortalPaperBurn)
	{
		return false;
	}

	return true;
}

void CDDBossSceneMode::OnFadeInStart()
{
	CDDSceneMode::OnFadeInStart();

	if (m_PortalPaperBurn)
	{
		// 잠깐 ExitPoint Object를 시작 지점으로 옮기고, 포탈 생성 효과를 준다.
		// Collider는 Start에서 false 된 상태
		// End Event를 Disable로 처리해 효과가 끝나면 오브젝트를 Disable처리함
		m_ExitPointObj->Enable(true);
		m_ExitPointCollider->Enable(false);
		m_ExitPointObj->SetWorldPos(m_EntryPoint);
		m_PortalPaperBurn->SetInverse(false);
		m_PortalPaperBurn->SetEndEvent(PaperBurnEndEvent::Disable);
		m_PortalPaperBurn->StartPaperBurn();
	}
}

void CDDBossSceneMode::OnClearDungeon()
{
	CDDSceneMode::OnClearDungeon();

	if (m_PortalPaperBurn)
	{
		// Exit Point로 다시 이동시키고, 포탈 생성 효과 재생, Collider를 켠다.
		m_ExitPointObj->SetWorldPos(m_OriginExitPos);
		m_ExitPointObj->Enable(true);
		m_PortalPaperBurn->SetInverse(true);
		m_PortalPaperBurn->SetEndEvent(PaperBurnEndEvent::Reset);
		m_PortalPaperBurn->StartPaperBurn();
	}
}
