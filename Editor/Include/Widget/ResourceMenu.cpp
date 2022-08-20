#include "ResourceMenu.h"
#include "IMGUIMenuItem.h"
#include "../Window/EffectEditor.h"
#include "../Window/EffectDisplayWindow.h"
#include "../Window/MaterialEditor.h"
#include "../Window/AnimationEditor.h"
#include "../Window/AnimationDisplayWindow.h"
#include "../Window/ResourceDisplayWindow.h"
#include "../Window/SocketWindow.h"
#include "../EditorManager.h"

CResourceMenu::CResourceMenu()
{
}

CResourceMenu::~CResourceMenu()
{
}

bool CResourceMenu::Init()
{
	// Scene
	// m_ToggleParticleEditorMenu = AddMenuItem("Open");

	m_ParticleEditorToggle = AddMenuItem("Particle");
	m_ParticleEditorToggle->SetClickCallBack<CResourceMenu>(this, &CResourceMenu::OnToggleParticleEditorCallback);

	m_AnimationEditorToggle = AddMenuItem("Animation");
	m_AnimationEditorToggle->SetClickCallBack<CResourceMenu>(this, &CResourceMenu::OnToggleAnimationEditorCallback);

	m_MaterialEditorToggle = AddMenuItem("Material");
	m_MaterialEditorToggle->SetClickCallBack<CResourceMenu>(this, &CResourceMenu::OnToggleMaterialEditorCallback);

	m_ResourceDisplayToggle = AddMenuItem("Display");
	m_ResourceDisplayToggle->SetClickCallBack<CResourceMenu>(this, &CResourceMenu::OnToggleResourceDisplayEditorCallback);

	m_SocketDisplayToggle = AddMenuItem("Socket");
	m_SocketDisplayToggle->SetClickCallBack<CResourceMenu>(this, &CResourceMenu::OnToggleSocketDisplayEditorCallback);

	return true;
}

void CResourceMenu::Render()
{
	CIMGUIBeginMenu::Render();
}

void CResourceMenu::OnToggleParticleEditorCallback()
{
	bool IsOpen = CEditorManager::GetInst()->GetEffectEditor()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetEffectEditor()->Close();
		CEditorManager::GetInst()->GetEffectDisplayWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetEffectEditor()->Open();
		CEditorManager::GetInst()->GetEffectDisplayWindow()->Open();
	}
}

void CResourceMenu::OnToggleAnimationEditorCallback()
{
	bool IsOpen = CEditorManager::GetInst()->GetAnimationEditor()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetAnimationEditor()->Close();
		CEditorManager::GetInst()->GetAnimationDisplayWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetAnimationEditor()->Open();
		CEditorManager::GetInst()->GetAnimationDisplayWindow()->Open();
	}
}

void CResourceMenu::OnToggleMaterialEditorCallback()
{
	bool IsOpen = CEditorManager::GetInst()->GetMaterialEditor()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetMaterialEditor()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetMaterialEditor()->Open();
	}
}

void CResourceMenu::OnToggleResourceDisplayEditorCallback()
{
	bool IsOpen = CEditorManager::GetInst()->GetResourceDisplayWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetResourceDisplayWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetResourceDisplayWindow()->Open();
	}
}

void CResourceMenu::OnToggleSocketDisplayEditorCallback()
{
	bool IsOpen = CEditorManager::GetInst()->GetSocketWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetSocketWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetSocketWindow()->Open();
	}
}
