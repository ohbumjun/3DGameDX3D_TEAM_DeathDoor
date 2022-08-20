
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
	std::vector<std::string>	Tmp; // ������� ���� ����
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
	// (OBJ) ���� �ڵ� => �Ź� �ش� ���� Ŭ���� ������, �ٽ� �Ź� ���Ӱ� Load ���ְ� �������ֱ� ���� �ּ�
	// if (Tree->GetChildCount() > 0)
	// 	return;

	// open�� ���丮�� ��ξȿ� �ִ� ���丮 ��ϵ��� �ٽ� �޾ƿͼ� Tree�� Child�� �־��ش�
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

		// ���� AllDirNames��� ����Ʈ���� ���� ������� �̷���� ��ε��� Root Ǯ ��ο� �������ش�
		m_CurrentFullPath = CEditorUtil::MergeFullPath(AllDirNames);
		m_CurrentPath.clear();
	}

	std::vector<std::string> vecFileName;
	std::vector<std::string> vecDirName;

	CEditorUtil::GetAllFilenamesFullPath(m_CurrentFullPath, vecFileName, vecDirName);

	// ���� Child ����� ����ֱ� 
	Tree->DeleteHierarchy();

	// ���� ����� ���� Add
	size_t Count = vecDirName.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* ChildTreeNode = Tree->AddChild(vecDirName[i]);
		ChildTreeNode->AddOpenCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnOpenBrowserTree);
		ChildTreeNode->AddSelectCallback<CFileBrowserTree>(this, &CFileBrowserTree::OnShowFileBrowser);
	}
	
	// �� ����, ���� ����� Add
	Count = vecFileName.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* ChildTreeNode = Tree->AddChild(vecFileName[i]);
		
		// (OBJ) ������ ���, File �� ���� ������ Tree (Widget) �̹Ƿ�, Open ��ü�� �� �ʿ䰡 ����.
		// �ش� Callback �� �����ϸ� ������ ������ ����.
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


	// FileBrowserâ Path Update
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


