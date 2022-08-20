
#include "IMGUITree.h"
#include "IMGUIManager.h"
#include "IMGUIUtil.h"
#include "IMGUIWindow.h"
#include "Scene/SceneManager.h"

CIMGUITree::CIMGUITree() :
	mParent(nullptr),
	m_GlobalID(-1),
	m_Selected(false),
	m_Enable(true),
	m_Open(false),
	m_DropType(TreeDropType::Normal),
	m_DataPtr(nullptr)
{
	m_Flag |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	m_DefaultFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
}

CIMGUITree::~CIMGUITree()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}

	size = mVecWidget.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecWidget[i]);
	}
}

CIMGUITree* CIMGUITree::GetNode(const int idx)
{
	if ((int)(mVecChild.size()) - 1 < idx)
		return nullptr;

	return mVecChild[idx];
}

CIMGUITree* CIMGUITree::FindChild(const std::string& Name)
{
	if (m_Name == Name)
	{
		return this;
	}

	if (mVecChild.empty())
	{
		return nullptr;
	}
	
	size_t ChildSize = mVecChild.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		CIMGUITree* Target = mVecChild[i]->FindChild(Name);

		if (Target)
			return Target;
	}

	return nullptr;
}

CIMGUITree* CIMGUITree::FindChild(const std::string& Name, void* DataPtr)
{
	if (m_Name == Name && m_DataPtr == DataPtr)
	{
		return this;
	}

	if (mVecChild.empty())
	{
		return nullptr;
	}

	size_t ChildSize = mVecChild.size();

	for (size_t i = 0; i < ChildSize; ++i)
	{
		CIMGUITree* Target = mVecChild[i]->FindChild(Name, DataPtr);

		if (Target)
			return Target;
	}

	return nullptr;
}

CIMGUITree* CIMGUITree::AddChild(const std::string& name, void* Data, const float width, const float height)
{
	CIMGUITree* tree = new CIMGUITree;

	tree->SetName(name);
	tree->SetOwner(m_Owner);
	tree->SetSize(width, height);
	tree->SetParent(this);

	if (!tree->Init())
	{
		SAFE_DELETE(tree);
		return nullptr;
	}

	if (Data)
	{
		tree->SetData(Data);
	}

	mVecChild.push_back(tree);
	mVecRender.push_back(tree);
	return tree;
}

CIMGUITree* CIMGUITree::AddChild(CIMGUITree* Tree, void* Data, const float width, const float height)
{
	Tree->SetName(Tree->m_Name);
	Tree->SetOwner(m_Owner);
	Tree->SetSize(width, height);
	Tree->SetParent(this);

	if (Data)
	{
		m_DataPtr = Data;
	}

	mVecChild.push_back(Tree);
	mVecRender.push_back(Tree);

	return Tree;
}

CIMGUITree* CIMGUITree::AddNewChild(CIMGUITree* Tree)
{
	Tree->SetOwner(m_Owner);
	Tree->SetParent(this);

	mVecChild.push_back(Tree);
	mVecRender.push_back(Tree);

	return Tree;
}

void CIMGUITree::DeleteChild(const std::string& Name)
{
	size_t Count = mVecChild.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (mVecChild[i]->m_Name == Name)
		{
			// mVecChild[i]->mParent = nullptr; -> 어차피 Delete 되는 녀석, 그냥 Erase -> 부모를 Parent 로 만들어버리면 부모 정보 상실

			auto iter = mVecChild.begin();

			std::advance(iter, i);
			mVecChild.erase(iter);
			break;
		}
	}

	Count = mVecRender.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (mVecRender[i]->m_Name == Name)
		{
			auto iter = mVecRender.begin();

			std::advance(iter, i);
			mVecRender.erase(iter);
			return;
		}
	}
}

CIMGUITree* CIMGUITree::DeleteHierarchy()
{
	size_t Count = mVecChild.size();

	for (size_t i = 0; i < Count; ++i)
	{
		mVecChild[i]->DeleteHierarchy();
		SAFE_DELETE(mVecChild[i]);
	}

	mVecChild.clear();
	mVecRender.clear();

	return this;
}

CIMGUITree* CIMGUITree::Delete()
{
	mParent->DeleteChild(m_Name);

	if (mVecChild.empty())
	{
		delete this;

		return mParent;
	}

	// 부모 자식 관계 재정의
	CIMGUITree* FirstChild = mVecChild[0];

	mParent->AddNewChild(FirstChild);

	// 기존 자식들 FirstChild의 자식들로 추가하기
	size_t ChilderenSize = mVecChild.size();

	for (size_t i = 0; i < ChilderenSize; ++i)
	{
		if (mVecChild[i] == FirstChild)
			continue;

		FirstChild->AddNewChild(mVecChild[i]);
	}

	mVecChild.clear();
	mVecWidget.clear();
	mVecRender.clear();

	delete this;


	return FirstChild;
}

size_t CIMGUITree::GetChildCount() const
{
	return mVecChild.size();
}

CIMGUITree* CIMGUITree::GetRoot()
{
	CIMGUITree* Node = this;

	while (Node->mParent)
	{
		Node = Node->mParent;
	}

	return Node;
}

void CIMGUITree::DeselectAll()
{
	m_Selected = false;

	size_t Count = mVecChild.size();

	for (size_t i = 0; i < Count; ++i)
	{
		mVecChild[i]->DeselectAll();
	}
}

bool CIMGUITree::Init()
{
	m_GlobalID = CIMGUIManager::GetInst()->GetGlobalTreeID();

	// Tree 하나 만들 때 마다 고유한 ID를 갖게끔 ID 얻어오고 나서 1 증가 시켜주기
	CIMGUIManager::GetInst()->AddGloablTreeID(1);
	

	return true;
}

void CIMGUITree::Render()
{
	if (!m_Enable)
		return;

	ImGui::PushID(m_GlobalID);

	if (m_Selected)
		m_Flag |= ImGuiTreeNodeFlags_Selected;
	else
		m_Flag = m_DefaultFlag;
	
	// if문 안으로 들어가면 무조건 하나(ex 제일 위에 TreeNode)의 ID골라서 그거 Selected Flag 추가해주기
	if (ImGui::TreeNodeEx(m_Name.c_str(), m_Flag))
	{
		if (!m_Open)
		{
			m_Open = true;

			auto iter = m_OpenCallbackList.begin();
			auto iterEnd = m_OpenCallbackList.end();

			for (; iter != iterEnd; ++iter)
				(*iter)(this);
		}

		ApplyDragEffect();
		
		ApplyDropEffect();

		// TreeNode 클릭됐을 때
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			CIMGUITree* Root = GetRoot();

			Root->DeselectAll();
			m_Selected = true;

			auto iter = m_SelectCallbackList.begin();
			auto iterEnd = m_SelectCallbackList.end();

			for (; iter != iterEnd; ++iter)
			{
				(*iter)(this);
			}
		}

		size_t size = mVecRender.size();
		for (size_t i = 0; i < size; ++i)
		{
			mVecRender[i]->Render();
			// 위의 mVecRender[i]->Render() 하고 다시 (부모노드로)돌아왔을때 자식노드의 개수(=mVecRender.size())가
			// 달라져있을 수도 있기 때문에 다시 업데이트해줌
			size = mVecRender.size();
		}

		ImGui::PopID();

		ImGui::TreePop();
	}

	else
	{
		if (m_Open)
			m_Open = false;


		ApplyDragEffect();

		ApplyDropEffect();

		// TreeNode 클릭됐을 때
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			CIMGUITree* Root = GetRoot();

			Root->DeselectAll();
			m_Selected = true;

			auto iter = m_SelectCallbackList.begin();
			auto iterEnd = m_SelectCallbackList.end();

			for (; iter != iterEnd; ++iter)
			{
				(*iter)(this);
			}
		}

		ImGui::PopID();
	}
}

void CIMGUITree::ApplyDragEffect()
{
	switch (m_DropType)
	{
	case TreeDropType::Normal:
		ApplyNormalDrag();
		break;
	case TreeDropType::Text:
		ApplyTextDrag();
		break;
	}
}

void CIMGUITree::ApplyNormalDrag()
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// 기존에 도경님이 정의해둔 Drag Drop
		{
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &m_GlobalID, sizeof(int));

			// Drag Drop Src TreeNode의 콜백
			if (m_DragDropSourceCallback)
				m_DragDropSourceCallback(this);

			// Staging 용도로 자신을 저장해두기
			CIMGUIManager::GetInst()->SetStagingTree(this);

			ImGui::EndDragDropSource();
		}
	}
}

void CIMGUITree::ApplyTextDrag()
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// File Browser 상의 File 정보를 IMGUITextInput 에 Drop 하기 위한 함수 추가 (OBJ)
		{
			TCHAR DragText[MAX_PATH] = {};

			int ConvertLength = MultiByteToWideChar(CP_ACP, 0, m_Name.c_str(), -1, 0, 0);
			MultiByteToWideChar(CP_ACP, 0, m_Name.c_str(), -1, DragText, ConvertLength);

			ImGui::SetDragDropPayload(vIMGUIDragDropPayLoadName.TextDragDrop, DragText, sizeof(DragText));

			// Drag Drop Src TreeNode의 콜백
			if (m_DragDropSourceCallback)
				m_DragDropSourceCallback(this);

			// Staging 용도로 자신을 저장해두기
			CIMGUIManager::GetInst()->SetStagingTree(this);

			ImGui::EndDragDropSource();
		}
	}
}


void CIMGUITree::ApplyDropEffect()
{
	switch (m_DropType)
	{
	case TreeDropType::Normal:
		ApplyNormalDrop();
		break;
	case TreeDropType::Text:
		ApplyTextDrop();
		break;
	}
}

void CIMGUITree::ApplyNormalDrop()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payload_n = *(const int*)payload->Data;

			CIMGUITree* NewChild = CIMGUIManager::GetInst()->GetStagingTree();
			CIMGUITree* NewChildOriginParent = NewChild->mParent;

			// 내가 나가게 될 Tree의 부모에게 나를 삭제하라고 하기
			if (NewChildOriginParent)
				NewChildOriginParent->DeleteChild(NewChild->m_Name);
			// 내가 Tree의 루트라면 나를 갖고 있는 Window에서 나를 m_vecWidget에서 지워주기
			else
				m_Owner->EraseWidget(NewChild->m_Name);

			if (NewChild)
			{
				AddChild(NewChild);

				// DragDrop Dest TreeNode Callback
				if (m_DragDropDestCallback)
					m_DragDropDestCallback(this, m_Name, NewChild->m_Name);

				CIMGUIManager::GetInst()->SetStagingTree(nullptr);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void CIMGUITree::ApplyTextDrop()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(vIMGUIDragDropPayLoadName.TextDragDrop))
		{
			// char type 배수 
			size_t DroppedTextSize = payload->DataSize;
			size_t DetNoSize = sizeof(TCHAR);
			if (DroppedTextSize % DetNoSize != 0)
				return;

			CIMGUITree* NewChild = CIMGUIManager::GetInst()->GetStagingTree();
			CIMGUITree* NewChildOriginParent = NewChild->mParent;

			// 내가 나가게 될 Tree의 부모에게 나를 삭제하라고 하기
			if (NewChildOriginParent)
				NewChildOriginParent->DeleteChild(NewChild->m_Name);
			// 내가 Tree의 루트라면 나를 갖고 있는 Window에서 나를 m_vecWidget에서 지워주기
			else
				m_Owner->EraseWidget(NewChild->m_Name);

			if (NewChild)
			{
				AddChild(NewChild);

				// DragDrop Dest TreeNode Callback
				if (m_DragDropDestCallback)
					m_DragDropDestCallback(this, m_Name, NewChild->m_Name);

				CIMGUIManager::GetInst()->SetStagingTree(nullptr);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

