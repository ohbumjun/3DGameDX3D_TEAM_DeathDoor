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

	// �����۹� ������Ʈ�� ���� ��� Exit Point Object�� �������� ����
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
		// ��� ExitPoint Object�� ���� �������� �ű��, ��Ż ���� ȿ���� �ش�.
		// Collider�� Start���� false �� ����
		// End Event�� Disable�� ó���� ȿ���� ������ ������Ʈ�� Disableó����
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
		// Exit Point�� �ٽ� �̵���Ű��, ��Ż ���� ȿ�� ���, Collider�� �Ҵ�.
		m_ExitPointObj->SetWorldPos(m_OriginExitPos);
		m_ExitPointObj->Enable(true);
		m_PortalPaperBurn->SetInverse(true);
		m_PortalPaperBurn->SetEndEvent(PaperBurnEndEvent::Reset);
		m_PortalPaperBurn->StartPaperBurn();
	}
}
