#include "BaseMenuBar.h"
#include "IMGUIWidget.h"
#include "IMGUIBeginMenu.h"
#include "../Widget/SaveLoadBeginMenu.h"
#include "../Widget/BehaviorTreeMenu.h"
#include "../Widget/ResourceMenu.h"
#include "../Widget/UtilMenuBar.h"
#include "../Widget/DataMenu.h"
#include "../Widget/SceneMenu.h"

CBaseMenuBar::CBaseMenuBar()
{
}

CBaseMenuBar::~CBaseMenuBar()
{
	size_t Size = m_vecBeginMenuChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecBeginMenuChild[i]);
	}
}

bool CBaseMenuBar::Init()
{
	// "File" -> Name 으로 세팅해준 것이, 실제 Menu Bar 에서 보여지는 이름이 된다.
	m_SaveLoadBeginMenu = AddWidget<CSaveLoadBeginMenu>("File");
	m_BehaviorTreeBeginMenu = AddWidget<CBehaviorTreeMenu>("BehaviorTree");
	m_ResourceBeginMenu = AddWidget<CResourceMenu>("Resource");
	m_UtilBeginMenu = AddWidget<CUtilMenuBar>("Util");
	m_DataBeginMenu = AddWidget<CDataMenu>("Data");
	m_SceneBeginMenu = AddWidget<CSceneMenu>("Scene");

	return true;
}

void CBaseMenuBar::Update(float DeltaTime)
{
	if (ImGui::BeginMainMenuBar())
	{
		mSize = m_vecBeginMenuChild.size();

		for (size_t i = 0; i < mSize; ++i)
		{
			m_vecBeginMenuChild[i]->Render();
		}

		ImGui::EndMainMenuBar();
	}
}

