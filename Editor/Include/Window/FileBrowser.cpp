
#include "FileBrowser.h"
#include "PathManager.h"
#include "IMGUIText.h"
#include "IMGUIImageButton.h"
#include "IMGUISameline.h"
#include "../EditorUtil.h"
#include "../EditorInfo.h"
#include "Resource/ResourceManager.h"
#include "IMGUIManager.h"
#include "FileBrowserTree.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

CFileBrowser::CFileBrowser()	:
	m_UpdatePath(true),
	m_UpdateFullPath(false),
	m_SingleLineIconNum(5)
{
	m_InitialPath = ROOT_PATH;
}

CFileBrowser::~CFileBrowser()
{
	//ClearWidget();
}

bool CFileBrowser::Init()
{
	CIMGUIWindow::Init();

	return true;
}

void CFileBrowser::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);

	size_t Num = (int)(GetWindowSize().x / ICON_SIZEX);
	bool Update = false;


	// 윈도우 사이즈 변경되면 SingleLineIcon 개수 바꿔서 새로 그려주기
	if (m_SingleLineIconNum != Num)
	{
		m_SingleLineIconNum = Num;

		if (m_InitialPath.length() > 0)
		{
			m_UpdatePath = true;
		}

		else
		{
			m_UpdateFullPath = true;
		}
	}

	if (m_UpdatePath || m_UpdateFullPath)
	{
		ClearWidget();

		if(m_UpdatePath)
			CEditorUtil::GetAllFilenames(m_InitialPath, m_vecFileName, m_vecDirName);

		else if(m_UpdateFullPath)
			CEditorUtil::GetAllFilenamesFullPath(m_InitialFullPath, m_vecFileName, m_vecDirName);

		size_t Count = m_vecDirName.size();

		ImVec2 Size = ImGui::GetWindowSize();
		
		int TotalCount = 0;

		for (size_t i = 0; i < Count; ++i, ++TotalCount)
		{
			std::string Name = m_vecDirName[i] + "ICon";
			CIMGUIChild* Child = AddWidget<CIMGUIChild>(Name, ICON_SIZEX, ICON_SIZEY);

			CIMGUIImageButton* DirImage = Child->AddWidget<CIMGUIImageButton>(m_vecDirName[i]);

			CTexture* Texture = CResourceManager::GetInst()->FindTexture(DIRECTORY_IMAGE);
			DirImage->SetTexture(Texture);
			DirImage->SetSize((float)Texture->GetWidth(), (float)Texture->GetHeight());

			CIMGUIText* DirName = Child->AddWidget<CIMGUIText>("DirName");
			DirName->SetText(m_vecDirName[i].c_str());

			DirImage->SetDoubleClickCallback<CFileBrowser>(this, &CFileBrowser::SetInitialPath);

			if (TotalCount < (int)Num - 1)
				Child->SetSameLine(true);
			else
				TotalCount = -1;

			m_vecChild.push_back(Child);
		}

		Count = m_vecFileName.size();

		for (size_t i = 0; i < Count; ++i, ++TotalCount)
		{
			std::string Name = m_vecFileName[i] + "ICon";
			CIMGUIChild* Child = AddWidget<CIMGUIChild>(Name, ICON_SIZEX, ICON_SIZEY);

			CIMGUIImageButton* FileImage = Child->AddWidget<CIMGUIImageButton>(m_vecFileName[i]);

			CTexture* Texture = CResourceManager::GetInst()->FindTexture(FILE_IMAGE);
			FileImage->SetTexture(Texture);
			FileImage->SetSize((float)Texture->GetWidth(), (float)Texture->GetHeight());
			FileImage->SetDragSrcCallback<CFileBrowser>(this, &CFileBrowser::OnDragSrcImageButton);

			CIMGUIText* FileName = Child->AddWidget<CIMGUIText>("FileName");
			FileName->SetText(m_vecFileName[i].c_str());

			if (TotalCount < (int)Num - 1)
				Child->SetSameLine(true);
			else
				TotalCount = -1;

			m_vecChild.push_back(Child);
		}

		m_UpdatePath = false;
		m_UpdateFullPath = false;
	}
	//m_vecWidget.clear();
}

void CFileBrowser::SetInitialPath(const std::string& Path)
{
	m_InitialPath = Path;
	m_InitialFullPath.clear();
	m_UpdatePath = true;
}

void CFileBrowser::SetInitialFullPath(const std::string& FullPath)
{
	m_InitialFullPath = FullPath;
	m_InitialPath.clear();
	m_UpdateFullPath = true;
}

void CFileBrowser::ClearWidget()
{
	size_t size = m_vecWidget.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_vecWidget[i]);
	}


	m_vecChild.clear();
	m_vecDirName.clear();
	m_vecFileName.clear();
	m_vecWidget.clear();
}

void CFileBrowser::FileClickCallback(CIMGUIImage* Image)
{

}

void CFileBrowser::OnDragSrcImageButton(const std::string& FileName)
{
	bool IsMeshFile = CEditorUtil::CompareExt(FileName.c_str(), ".msh");

	if (!IsMeshFile)
		return;

	CFileBrowserTree* FileBrowserTree = (CFileBrowserTree*)CIMGUIManager::GetInst()->FindIMGUIWindow(FILE_BROWSERTREE);

	std::string CurrentFullPath = FileBrowserTree->GetCurrentFullPath();
}
