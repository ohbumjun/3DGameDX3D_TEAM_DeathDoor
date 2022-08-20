
#include "ObjectComponentWindow.h"
#include "IMGUIListBox.h"
#include "IMGUIButton.h"
#include "ObjectComponentCreateModal.h"
#include "../EditorInfo.h"
#include "ObjectHierarchyWindow.h"
#include "IMGUIManager.h"
#include "IMGUITree.h"
#include "IMGUISameLine.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Engine.h"
#include "PathManager.h"
#include "InspectorWindow.h"

CObjectComponentWindow::CObjectComponentWindow()	:
	m_ComponentCreatePopUpButton(nullptr),
	m_ComponentCreateModal(nullptr),
	m_SelectIndex(-1)
{
}

CObjectComponentWindow::~CObjectComponentWindow()
{
}

bool CObjectComponentWindow::Init()
{
	CIMGUIWindow::Init();

	// 숫자 하드 코딩하지 말고, Window크기 알아서 Window크기에 Offset줘서 맞추기
	m_ComponentListBox = AddWidget<CIMGUIListBox>("ObjectComponentList", 130.f, 200.f);
	m_ComponentListBox->SetHideName(true);
	m_ComponentListBox->SetSelectCallback<CObjectComponentWindow>(this, &CObjectComponentWindow::OnSelectComponent);

	m_ComponentCreatePopUpButton = AddWidget<CIMGUIButton>("Create", 50.f, 20.f);
	m_ComponentCreatePopUpButton->SetClickCallback<CObjectComponentWindow>(this, &CObjectComponentWindow::OnCreateComponentPopUp);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");

	m_ComponentDeleteButton = AddWidget<CIMGUIButton>("Delete", 50.f, 20.f);
	m_ComponentDeleteButton->SetClickCallback<CObjectComponentWindow>(this, &CObjectComponentWindow::OnDeleteComponent);

	return true;
}

void CObjectComponentWindow::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);
}

void CObjectComponentWindow::OnRenameComponent(const std::string& NewName, const std::string& PrevName)
{
	m_ComponentListBox->ChangeItem(NewName, PrevName);
}

void CObjectComponentWindow::OnCreateComponentPopUp()
{
	if (!m_ComponentCreateModal)
		m_ComponentCreateModal = AddWidget<CObjectComponentCreateModal>(OBJECTCOMPONENT_CREATE_POPUPMODAL);

	else
	{
		PopUpModalState State = m_ComponentCreateModal->GetPopUpModalState();

		if (State == PopUpModalState::Closed)
			m_ComponentCreateModal->SetPopUpModalState(PopUpModalState::Open);

		m_ComponentCreateModal->SetRender(true);
	}
}

int CObjectComponentWindow::OnCreateObjectComponent(const std::string& Name)
{
	int Count = m_ComponentListBox->GetItemCount();
	m_ComponentListBox->AddItem(Name);

	return Count;
}

void CObjectComponentWindow::OnSelectComponent(int Index, const char* Label)
{
	m_SelectIndex = Index;
	m_SelectLabel = Label;
}

void CObjectComponentWindow::Clear()
{
	m_SelectIndex = -1;
	m_SelectLabel = "";

	m_ComponentListBox->Clear();
}

void CObjectComponentWindow::OnDeleteComponent()
{
	// 선택되지 않았을 때
	if (m_SelectIndex == -1)
		return;

	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	CGameObject* Obj = Window->GetSelectObject();

	std::string SelectItem = m_ComponentListBox->GetSelectItem();
	int Index = m_ComponentListBox->GetSelectIndex();

	CObjectComponent* DeleteComp = (CObjectComponent*)Obj->FindComponent(SelectItem);

	if (!DeleteComp)
		return;

	Obj->DeleteObjectComponent(SelectItem);

	m_ComponentListBox->DeleteItem(Index);
}

void CObjectComponentWindow::OnRefreshObjectComponentList(CGameObject* Object)
{
	if (!Object)
	{
		return;
	}

	m_ComponentListBox->Clear();

	std::vector<CObjectComponent*> vecObjComp;
	Object->GetAllObjectComponentsPointer(vecObjComp);

	size_t Size = vecObjComp.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_ComponentListBox->AddItem(vecObjComp[i]->GetName());
	}
}

void CObjectComponentWindow::OnSaveComponent()
{
	if (m_SelectIndex == -1)
	{
		return;
	}

	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (!Window)
	{
		return;
	}

	CGameObject* SelectObject = Window->GetSelectGameObject();

	if (!SelectObject)
	{
		return;
	}

	std::string Name = m_ComponentListBox->GetItem(m_SelectIndex);
	CComponent* SaveComponent = SelectObject->FindComponent(Name);

	if (!SaveComponent)
	{
		MessageBox(nullptr, TEXT("선택된 컴포넌트 없음"), TEXT("Error"), MB_OK);
		return;
	}

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All File\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(COMPONENT_PATH)->Path;

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char FullPath[MAX_PATH] = {};

		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FullPath, Length, 0, 0);

		bool Ret = SelectObject->SaveOnly(SaveComponent, FullPath);

		if (!Ret)
		{
			MessageBox(nullptr, TEXT("컴포넌트 저장 실패"), TEXT("Error"), MB_OK);
			return;
		}

		MessageBox(nullptr, TEXT("컴포넌트 저장 성공"), TEXT("Success"), MB_OK);
	}
}

void CObjectComponentWindow::OnLoadComponent()
{
	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (!Window)
	{
		return;
	}

	CGameObject* SelectObject = Window->GetSelectGameObject();

	if (!SelectObject)
	{
		return;
	}

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All File\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(COMPONENT_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FullPath[MAX_PATH] = {};

		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FullPath, Length, 0, 0);

		CComponent* LoadComp = nullptr;
		bool Ret = SelectObject->LoadOnly(FullPath, LoadComp);

		if (!Ret)
		{
			MessageBox(nullptr, TEXT("컴포넌트 로드 실패"), TEXT("Error"), MB_OK);
			return;
		}

		// List Update
		m_ComponentListBox->AddItem(LoadComp->GetName());

		// Insepctor Update
		CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

		if (Inspector)
		{
			Inspector->OnCreateObjectComponent((CObjectComponent*)LoadComp);
		}

		MessageBox(nullptr, TEXT("컴포넌트 로드 성공"), TEXT("Success"), MB_OK);
	}
}

