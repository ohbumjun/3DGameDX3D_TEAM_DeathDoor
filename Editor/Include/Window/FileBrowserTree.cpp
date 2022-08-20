
#include "FileBrowserTree.h"
#include "IMGUIText.h"
#include "IMGUITree.h"
#include "IMGUIDummy.h"
#include "../EditorUtil.h"
#include "PathManager.h"
#include "FileBrowser.h"
#include "IMGUIManager.h"


CFileBrowserTree::CFileBrowserTree()
{
	m_CurrentPath = ROOT_PATH;
}

CFileBrowserTree::~CFileBrowserTree()
{
}

bool CFileBrowserTree::Init()
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(m_CurrentPath);
	std::string FullPath = Info->PathMultibyte;

	m_CurrentPathText = AddWidget<CIMGUIText>(m_CurrentPath);
	m_CurrentPathText->SetText(FullPath.c_str());

	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 20.f, 10.f);

	m_Root = AddWidget<CIMGUITree>("Root");
	std::vector<std::string>	Tmp; // 사용하지 않을 벡터
	std::vector<std::string> vecCurrentPathDir;

	CEditorUtil::GetAllFilenames(m_CurrentPath, Tmp, vecCurrentPathDir);

	size_t Count = vecCurrentPathDir.size();
	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* ChildTreeNode = m_Root->AddChild(vecCurrentPathDir[i]);
		ChildTreeNode->AddOpenCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnOpenBrowserTree);
		ChildTreeNode->AddSelectCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnShowFileBrowser);
	}

	return true;
}

void CFileBrowserTree::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);
}

void CFileBrowserTree::OnOpenBrowserTree(CIMGUITree* Tree)
{
	// (OBJ) 기존 코드 => 매번 해당 폴더 클릭할 때마다, 다시 매번 새롭게 Load 해주고 세팅해주기 위해 주석
	// if (Tree->GetChildCount() > 0)
	// 	return;

	// open한 디렉토리의 경로안에 있는 디렉토리 목록들을 다시 받아와서 Tree의 Child로 넣어준다
	const PathInfo* Info = CPathManager::GetInst()->FindPath(Tree->GetName());

	if (Info)
	{
		m_CurrentFullPath = Info->PathMultibyte;
		m_CurrentPath = Tree->GetName();
	}

	else
	{
		std::list<std::string>	AllDirNames;

		CEditorUtil::GetFullPathDirectory(Tree, AllDirNames);

		// 위에 AllDirNames라는 리스트에서 얻어온 폴더들로 이루어진 경로들을 Root 풀 경로와 결합해준다
		m_CurrentFullPath = CEditorUtil::MergeFullPath(AllDirNames);
		m_CurrentPath.clear();
	}

	std::vector<std::string> vecFileName;
	std::vector<std::string> vecDirName;

	CEditorUtil::GetAllFilenamesFullPath(m_CurrentFullPath, vecFileName, vecDirName);

	// 기존 Child 목록은 비워주기 
	Tree->DeleteHierarchy();

	// 폴더 목록을 먼저 Add
	size_t Count = vecDirName.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* ChildTreeNode = Tree->AddChild(vecDirName[i]);
		ChildTreeNode->AddOpenCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnOpenBrowserTree);
		ChildTreeNode->AddSelectCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnShowFileBrowser);
	}
	
	// 그 다음, 파일 목록을 Add
	Count = vecFileName.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* ChildTreeNode = Tree->AddChild(vecFileName[i]);
		
		// (OBJ) 파일의 경우, File 이 가장 마지막 Tree (Widget) 이므로, Open 자체를 할 필요가 없다.
		// 해당 Callback 을 세팅하면 오히려 오류가 난다.
		// ChildTreeNode->AddOpenCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnOpenBrowserTree);
		ChildTreeNode->SetTextDropType();
		ChildTreeNode->AddSelectCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnShowFileBrowser);
	}
}

void CFileBrowserTree::OnShowFileBrowser(CIMGUITree* Tree)
{
	CFileBrowser* FileBrowser = (CFileBrowser*)CIMGUIManager::GetInst()->FindIMGUIWindow(FILE_BROWSER);

	std::string NewPath = Tree->GetName();

	const PathInfo* Info = CPathManager::GetInst()->FindPath(NewPath);


	// FileBrowser창 Path Update
	if (!Info)
	{
		FileBrowser->SetInitialFullPath(m_CurrentFullPath);
		return;
	}

	else
	{
		FileBrowser->SetInitialPath(NewPath);
	}
}


