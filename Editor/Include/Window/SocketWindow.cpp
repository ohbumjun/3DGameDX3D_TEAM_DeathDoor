
#include "SocketWindow.h"
#include "IMGUIListBox.h"
#include "IMGUIButton.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "PathManager.h"
#include "IMGUITextInput.h"
#include "IMGUISameLine.h"
#include "IMGUIDummy.h"
#include "Resource/Animation/SkeletonSokcet.h"

CSocketWindow::CSocketWindow()	:
	m_SelectSkelton(nullptr)
{
}

CSocketWindow::~CSocketWindow()
{
}

bool CSocketWindow::Init()
{
	m_SkeletonNameTextInput = AddWidget<CIMGUITextInput>("Skeleton Name");
	m_SkeletonNameTextInput->SetHintText("Enter Skeleton Name");
	m_SkeletonNameTextInput->SetHideName(true);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
	
	m_LoadBoneButton = AddWidget<CIMGUIButton>("Load Skeleton");
	m_LoadBoneButton->SetSize(80.f, 20.f);
	m_LoadBoneButton->SetClickCallback<CSocketWindow>(this, &CSocketWindow::OnLoadBoneFile);

	Line = AddWidget<CIMGUISameLine>("Line");

	m_SaveBoneFileButton = AddWidget<CIMGUIButton>("Save Skeleton");
	m_SaveBoneFileButton->SetSize(80.f, 20.f);
	m_SaveBoneFileButton->SetClickCallback<CSocketWindow>(this, &CSocketWindow::OnSaveBoneFile);

	std::vector<std::string> vecSkeletonNames;

	CSceneManager::GetInst()->GetScene()->GetResource()->GetAllSkeletonNames(vecSkeletonNames);
	m_LoadedSkeletonListBox = AddWidget<CIMGUIListBox>("Loaded Skeletons");
	m_LoadedSkeletonListBox->SetHideName(true);

	size_t Count = vecSkeletonNames.size();
	for (size_t i = 0; i < Count; ++i)
		m_LoadedSkeletonListBox->AddItem(vecSkeletonNames[i]);

	m_LoadedSkeletonListBox->SetSelectCallback<CSocketWindow>(this, &CSocketWindow::OnSelectSkeleton);

	Line = AddWidget<CIMGUISameLine>("Line");

	m_SelectedSkeletonBoneListBox = AddWidget<CIMGUIListBox>("Loaded Bones");
	m_SelectedSkeletonBoneListBox->SetHideName(true);
	m_SelectedSkeletonBoneListBox->SetSize(200.f, 40.f);

	m_SelectedSkeletonBoneListBox->SetSelectCallback<CSocketWindow>(this, &CSocketWindow::OnSelectBone);

	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 100.f, 30.f);

	m_SocketNameTextInput = AddWidget<CIMGUITextInput>("Socket Name");
	m_SocketNameTextInput->SetHideName(true);
	m_SocketNameTextInput->SetHintText("Enter Socket Name");

	Line = AddWidget<CIMGUISameLine>("Line");

	m_AddSokcetButton = AddWidget<CIMGUIButton>("Add Socket");
	m_AddSokcetButton->SetSize(80.f, 20.f);
	m_AddSokcetButton->SetClickCallback<CSocketWindow>(this, &CSocketWindow::OnAddSocket);


	m_SelectedSkeletonSocketListBox = AddWidget<CIMGUIListBox>("Skeleton Socket List");
	m_SelectedSkeletonSocketListBox->SetHideName(true);

	return true;
}

void CSocketWindow::OnLoadBoneFile()
{
	std::string SkeletonName = m_SkeletonNameTextInput->GetTextMultibyte();

	if (SkeletonName.length() < 1)
		return;

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("Bone File(.bne)\0*.bne\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MESH_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		std::string SkeletonName = m_SkeletonNameTextInput->GetTextMultibyte();

		bool Success = CSceneManager::GetInst()->GetScene()->GetResource()->LoadSkeletonFullPath(SkeletonName, FilePath);

		if (Success)
		{
			m_LoadedSkeletonListBox->AddItem(SkeletonName);
			m_SkeletonNameTextInput->ClearText();
			MessageBox(nullptr, TEXT("로드 완료"), TEXT("로드 완료"), MB_OK);
		}
	}
}

void CSocketWindow::OnSelectSkeleton(int Index, const char* Label)
{
	CSkeleton* Skeleton = CSceneManager::GetInst()->GetScene()->GetResource()->FindSkeleton(Label);

	if (Skeleton)
	{
		m_SelectSkelton = Skeleton;
		m_SelectedSkeletonBoneListBox->Clear();
		
		size_t Count = m_SelectSkelton->GetBoneCount();

		for (size_t i = 0; i < Count; ++i)
		{
			m_SelectedSkeletonBoneListBox->AddItem(m_SelectSkelton->GetBone((int)i)->strName);
		}
	}
}

void CSocketWindow::OnSelectBone(int Index, const char* Label)
{
	m_SelectBone = m_SelectSkelton->GetBone(Label);

	if (m_SelectSkelton->GetBoneSocketCount() > 0)
	{
		m_SelectedSkeletonSocketListBox->Clear();
		size_t Count = m_SelectSkelton->GetBoneSocketCount();

		for (size_t i = 0; i < Count; ++i)
		{
			if (m_SelectSkelton->GetSkeletonSocket(i)->GetBoneName() == m_SelectBone->strName)
				m_SelectedSkeletonSocketListBox->AddItem(m_SelectSkelton->GetSkeletonSocket(i)->GetName());
		}
	}
}

void CSocketWindow::OnAddSocket()
{
	if (!m_SelectSkelton || !m_SelectBone)
		return;

	bool Success = CSceneManager::GetInst()->GetScene()->GetResource()->AddSocket(m_SelectSkelton->GetName(), m_SelectBone->strName, m_SocketNameTextInput->GetTextMultibyte());

	if (Success)
		MessageBox(nullptr, TEXT("소켓 추가 성공"), TEXT("성공"), MB_OK);

	else
		MessageBox(nullptr, TEXT("소켓 추가 실패"), TEXT("실패"), MB_OK);

	m_SocketNameTextInput->ClearText();
}

void CSocketWindow::OnSaveBoneFile()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MESH_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("Bone File\0*.bne\0");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);
		bool Success = false;

		if (m_SelectSkelton)
		{
			Success = m_SelectSkelton->SaveSkeletonFullPath(FilePathMultibyte);
		}


		if (!Success)
		{
			MessageBox(nullptr, TEXT("Bone File 저장 실패"), TEXT("Error"), MB_OK);
			return;
		}

		MessageBox(nullptr, TEXT("Bone File 저장 성공"), TEXT("Success"), MB_OK);
	}

}
