#include "CollisionProfileEditor.h"
#include "Collision/CollisionManager.h"
#include "Resource/ResourceManager.h"
#include "Engine.h"
#include "PathManager.h"
#include "EngineUtil.h"
#include "../EditorUtil.h"
#include "IMGUIManager.h"
#include "IMGUIListBox.h"
#include "IMGUITextInput.h"
#include "IMGUICheckBox.h"
#include "IMGUIComboBox.h"
#include "IMGUIButton.h"
#include "IMGUIText.h"
#include "IMGUISeperator.h"
#include "IMGUISameLine.h"
#include "IMGUIChild.h"
#include "IMGUICollapsingHeader.h"
#include "IMGUIDummy.h"
#include "InspectorWindow.h"
#include "../Widget/ColliderComponentWidget.h"

CCollisionProfileEditor::CCollisionProfileEditor()	 :
	m_ProfileList(nullptr),
	m_ChannelList(nullptr),
	m_ProfileNameInput(nullptr),
	m_CreateProfileButton(nullptr),
	m_ProfileChannel(nullptr),
	m_ProfileName(nullptr),
	m_SaveButton(nullptr),
	m_LoadButton(nullptr),
	m_DeleteButton(nullptr)
{
}

CCollisionProfileEditor::~CCollisionProfileEditor()
{
}

bool CCollisionProfileEditor::Init()
{
	// 1. Widget
	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("Collision Profile Editor");
	AddWidget<CIMGUISeperator>("Sep");

	/// 1-1. Left
	CIMGUIChild* Left = AddWidget<CIMGUIChild>("Left", 300.f, 800.f);
	Text = Left->AddWidget<CIMGUIText>("Text");
	Text->SetText("Current Profile List");
	m_ProfileList = Left->AddWidget<CIMGUIListBox>("Profile", 280.f, 500.f);
	m_SaveButton = Left->AddWidget<CIMGUIButton>("Save", 0.f, 0.f);
	Left->AddWidget<CIMGUISameLine>("Line");
	m_LoadButton = Left->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);

	/// 1-2. Right
	AddWidget<CIMGUISameLine>("Line");
	CIMGUIChild* Right = AddWidget<CIMGUIChild>("Right", 300.f, 800.f);
	CIMGUICollapsingHeader* MakeBlock = Right->AddWidget<CIMGUICollapsingHeader>("Create New Profile");
	m_ChannelList = MakeBlock->AddWidget<CIMGUIComboBox>("Channel");
	m_ProfileNameInput = MakeBlock->AddWidget<CIMGUITextInput>("Profile Name");
	m_CreateProfileButton = MakeBlock->AddWidget<CIMGUIButton>("Create", 0.f, 0.f);
	Right->AddWidget<CIMGUISeperator>("Sep");

	Text = Right->AddWidget<CIMGUIText>("Text");
	Text->SetText("Currnet Profile Info");
	Right->AddWidget<CIMGUISameLine>("Line");
	Right->AddWidget<CIMGUIDummy>("Dummy", 100.f, 0.f);
	Right->AddWidget<CIMGUISameLine>("Line");
	m_DeleteButton = Right->AddWidget<CIMGUIButton>("Delete",0.f, 0.f);

	m_ProfileName = Right->AddWidget<CIMGUITextInput>("Current Profile Name");
	m_ProfileChannel = Right->AddWidget<CIMGUITextInput>("Profile Channel");
	Text = Right->AddWidget<CIMGUIText>("Text");
	Text->SetText("Interaction");
	std::string ChannelName;
	for (int i = 0; i < (int)Collision_Channel::Max; ++i)
	{
		ChannelName = CEngineUtil::CollisionChannelToString((Collision_Channel)i);
		m_InteractionEditArr[i] = Right->AddWidget<CIMGUIComboBox>(ChannelName, 100.f);
		m_InteractionEditArr[i]->SetSelectPointerCallback(this, &CCollisionProfileEditor::OnSelectInteraction);

		std::string InteractionName;
		for (int j = 0; j < 3; ++j)
		{
			InteractionName = CEngineUtil::CollisionInteractionToString(Collision_Interaction(j));
			m_InteractionEditArr[i]->AddItem(InteractionName);
		}
	}

	// Initial Value
	m_ProfileList->SetPageItemCount(25);
	m_ProfileList->SetHideName(true);
	m_ProfileName->ReadOnly(true);
	m_ProfileChannel->ReadOnly(true);

	// 현재 Profile
	std::vector<std::string> ProfileNames;
	CCollisionManager::GetInst()->GetProfileNames(ProfileNames);

	size_t Size = ProfileNames.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_ProfileList->AddItem(ProfileNames[i]);
	}

	for (size_t i = 0; i < (size_t)Collision_Channel::Max; ++i)
	{
		m_ChannelList->AddItem(CEngineUtil::CollisionChannelToString((Collision_Channel)i));
	}

	// CallBack
	m_ProfileList->SetSelectCallback(this, &CCollisionProfileEditor::OnSelectList);
	m_CreateProfileButton->SetClickCallback(this, &CCollisionProfileEditor::OnClickCreate);
	m_SaveButton->SetClickCallback(this, &CCollisionProfileEditor::OnClickSave);
	m_LoadButton->SetClickCallback(this, &CCollisionProfileEditor::OnClickLoad);
	m_DeleteButton->SetClickCallback(this, &CCollisionProfileEditor::OnClickDelete);

	return true;
}

void CCollisionProfileEditor::Refresh()
{
	m_ProfileList->Clear();
	m_ProfileName->ClearText();
	m_ProfileChannel->ClearText();

	std::vector<std::string> ProfileNames;
	CCollisionManager::GetInst()->GetProfileNames(ProfileNames);

	size_t Size = ProfileNames.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_ProfileList->AddItem(ProfileNames[i]);
	}

	for (size_t i = 0; i < (size_t)Collision_Channel::Max; ++i)
	{
		m_InteractionEditArr[i]->SetSelectIndex(0);
	}
}

void CCollisionProfileEditor::OnSelectList(int Idx, const char* Label)
{
	CollisionProfile* Profile = CCollisionManager::GetInst()->FindProfile(Label);

	std::string ChannelName = CEngineUtil::CollisionChannelToString(Profile->Channel);
	m_ProfileName->SetText(Label);
	m_ProfileChannel->SetText(ChannelName.c_str());

	size_t Size = Profile->vecInteraction.size();
	for (size_t i = 0; i < Size; ++i)
	{
		Collision_Interaction Interaction = Profile->vecInteraction[i];

		m_InteractionEditArr[i]->SetSelectIndex((int)Interaction);
	}
}

void CCollisionProfileEditor::OnClickCreate()
{
	if (m_ProfileNameInput->Empty() || m_ChannelList->GetSelectIndex() == -1)
	{
		return;
	}

	std::string ProfileName = m_ProfileNameInput->GetTextMultibyte();
	Collision_Channel Channel = (Collision_Channel)m_ChannelList->GetSelectIndex();

	bool Success = CCollisionManager::GetInst()->CreateProfile(ProfileName, Channel, true);

	if (Success)
	{
		m_ProfileList->AddItem(ProfileName);
	}
}

void CCollisionProfileEditor::OnSelectInteraction(int Idx, const char* Label, class CIMGUIComboBox* Pointer)
{
	if (m_ProfileList->GetSelectIndex() == -1)
	{
		return;
	}

	Collision_Interaction Interaction = (Collision_Interaction)Idx;

	std::string CurProfileName = m_ProfileList->GetSelectItem();

	int SelectChannel = -1;
	for (int i = 0; i < (int)Collision_Channel::Max; ++i)
	{
		if (Pointer == m_InteractionEditArr[i])
		{
			SelectChannel = i;
			break;
		}
	}

	Collision_Channel CurChannel = (Collision_Channel)SelectChannel;

	CCollisionManager::GetInst()->SetCollisionState(CurProfileName, CurChannel, Interaction);
}

void CCollisionProfileEditor::OnClickSave()
{
	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("csv File(.csv)\0*.csv\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(EXCEL_PATH)->Path;

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char ConvertFullPath[MAX_PATH] = {};
		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, ConvertFullPath, Length, 0, 0);

		bool Success = CCollisionManager::GetInst()->SaveAsCSVFullPath(ConvertFullPath);

		if (!Success)
		{
			MessageBox(nullptr, TEXT("저장 실패"), TEXT("Error"), MB_OK);
			return;
		}

		MessageBox(nullptr, TEXT("저장 성공"), TEXT("Error"), MB_OK);
	}
}

void CCollisionProfileEditor::OnClickLoad()
{
	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("csv File(.csv)\0*.csv\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(EXCEL_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char ConvertFullPath[MAX_PATH] = {};
		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, ConvertFullPath, Length, 0, 0);

		bool Success = CCollisionManager::GetInst()->LoadProfileFromCSVFullPath(ConvertFullPath);

		if (!Success)
		{
			MessageBox(nullptr, TEXT("로드 실패"), TEXT("Error"), MB_OK);
			Refresh();
			return;
		}

		MessageBox(nullptr, TEXT("로드 성공"), TEXT("Error"), MB_OK);
		Refresh();

		CInspectorWindow* Window = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

		if (Window)
		{
			CColliderComponentWidget* Widget = (CColliderComponentWidget*)Window->FindWidget("ColliderComponentWidget");

			if (Widget)
				Widget->RefreshCollisionProfile();
		}
	}
}

void CCollisionProfileEditor::OnClickDelete()
{
	int SelectIndex = m_ProfileList->GetSelectIndex();

	if (SelectIndex == -1)
	{
		return;
	}

	std::string DeleteProfileName = m_ProfileList->GetSelectItem();

	bool Success = CCollisionManager::GetInst()->DeleteProfile(DeleteProfileName);

	if (!Success)
	{
		MessageBox(nullptr, TEXT("삭제 실패 : 해당 프로파일을 사용하는 충돌체가 존재함"), TEXT("삭제 실패"), MB_OK);
		return;
	}

	Refresh();
}
