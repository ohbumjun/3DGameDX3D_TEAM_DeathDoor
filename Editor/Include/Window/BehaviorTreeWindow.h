#pragma once

#include "IMGUIWindow.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "../GraphEditor.h"
#include "Component/Node/Node.h"
#include "Component/Node/SequenceNode.h"
#include "Component/Node/SelectorNode.h"
#include "Component/Node/CompositeNode.h"
#include "Component/Node/DecoratorNode.h"
#include "Component/BehaviorTree.h"

static ImVec2 operator+(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x + b.x, a.y + b.y);
}

static ImVec2 operator-(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x - b.x, a.y - b.y);
}

static ImVec2 operator*(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x * b.x, a.y * b.y);
}

static ImVec2 operator/(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x / b.x, a.y / b.y);
}

static ImVec2 operator/(const ImVec2& a, float b)
{
    return ImVec2(a.x / b, a.y / b);
}


static ImVec2 operator*(const ImVec2& a, const float b)
{
    return ImVec2(a.x * b, a.y * b);
}


template <typename T, std::size_t N>
struct Array
{
    T data[N];
    const size_t size() const { return N; }

    const T operator [] (size_t index) const { return data[index]; }
    operator T* () {
        T* p = new T[N];
        memcpy(p, data, sizeof(data));
        return p;
    }
};


template <typename T, typename ... U> Array(T, U...)->Array<T, 1 + sizeof...(U)>;

struct GraphEditorDelegate : public GraphEditor::Delegate
{
    ~GraphEditorDelegate()
    {
        size_t Count = mNodes.size();

        for (size_t i = 0; i < Count; ++i)
        {
            if (mNodes[i].name)
                delete[] mNodes[i].name;
        }
    }

    struct Node
    {
        char* name;
        GraphEditor::TemplateIndex templateIndex;
        float x, y;
        bool mSelected;
        CNode* BehaviorTreeNode;
        int NodeIndex;
    };

    bool m_IsCleared = false;

    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
    {
        return true;
    }

    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
    {
        mNodes[nodeIndex].mSelected = selected;
    }

    void MoveSelectedNodes(const ImVec2 delta) override
    {
        for (auto& node : mNodes)
        {
            if (!node.mSelected)
            {
                continue;
            }
            node.x += delta.x;
            node.y += delta.y;
        }
    }

    virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
    {
    }

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    {
        GraphEditorDelegate::Node SrcNode = GetDelegateNode((int)inputNodeIndex);
        GraphEditorDelegate::Node DestNode = GetDelegateNode((int)outputNodeIndex);

        // Action Node나 Condition Node가 부모가 되면 안됨
        if (SrcNode.templateIndex == 2 || SrcNode.templateIndex == 3)
            return;

        CBehaviorTree* Tree = DestNode.BehaviorTreeNode->GetOwner();

        CNode* ChildNode = Tree->FindNode(DestNode.name);
        //CNode* ChildNode = Tree->FindNode(DestNode);

        // SrcNode가 Selector이거나 Sequence일때
        if (SrcNode.templateIndex == 0 || SrcNode.templateIndex == 1)
        {
            CCompositeNode* ParentNode = (CCompositeNode*)Tree->FindNode(SrcNode.name);

            if (inputSlotIndex == 0)
                ParentNode->AddChildFront(ChildNode);
            else
                ParentNode->AddChild(ChildNode);
        }

        // SrcNode가 Decorator일 때
        else if (SrcNode.templateIndex == 4)
        {
            CDecoratorNode* ParentNode = (CDecoratorNode*)Tree->FindNode(SrcNode.name);
            ParentNode->SetChild(ChildNode);
        }

        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    }

    // GameObject Load하고 맨 처음 Behavior Tree Window 열 때 호출
    void LoadLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex)
    {
        GraphEditorDelegate::Node SrcNode = GetDelegateNode((int)inputNodeIndex);
        GraphEditorDelegate::Node DestNode = GetDelegateNode((int)outputNodeIndex);

        // Action Node나 Condition Node가 부모가 되면 안됨
        if (SrcNode.templateIndex == 2 || SrcNode.templateIndex == 3)
            return;

        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    }


    void DelLink(GraphEditor::LinkIndex linkIndex) override
    {
        int ParentNodeIdx = (int)mLinks[linkIndex].mInputNodeIndex;
        int ChildNodeIdx = (int)mLinks[linkIndex].mOutputNodeIndex;

        CNode* ParentNode = mNodes[ParentNodeIdx].BehaviorTreeNode;
        CNode* ChildNode = mNodes[ChildNodeIdx].BehaviorTreeNode;

        if (ChildNode->GetTypeID() == typeid(CSequenceNode).hash_code() || ChildNode->GetTypeID() == typeid(CSelectorNode).hash_code())
        {
            std::list<CNode*> GrandChildList = ((CCompositeNode*)ChildNode)->GetChildrenList();

            auto iter = GrandChildList.begin();
            auto iterEnd = GrandChildList.end();

            for (; iter != iterEnd; ++iter)
            {
                (*iter)->SetParent(nullptr);
            }
        }
        

        if (ParentNode->GetNodeType() == Node_Type::Decorator)
            ((CDecoratorNode*)ParentNode)->SetChild(nullptr);

        else
            ((CCompositeNode*)ParentNode)->DeleteChild(ChildNode);

        mLinks.erase(mLinks.begin() + linkIndex);
    }

    void DelAllLink(GraphEditor::NodeIndex NodeIndex)
    {
        size_t LinkCount = mLinks.size();

        for (size_t i = 0; i < LinkCount; )
        {
            if (mLinks[i].mInputNodeIndex == NodeIndex || mLinks[i].mOutputNodeIndex == NodeIndex)
            {
                mLinks.erase(mLinks.begin() + i);
                LinkCount = mLinks.size();
                i = 0;
            }

            else
                ++i;
        }
    }

    void DeleteNode(GraphEditor::NodeIndex NodeIndex)
    {
        auto iter = mNodes.begin();
        std::advance(iter, NodeIndex);

        SAFE_DELETE_ARRAY((*iter).name);

        mNodes.erase(mNodes.begin() + NodeIndex);
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    {
        drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
        drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
    }

    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex, const std::string& Name) override
    {
        drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
        drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), Name.c_str());
    }

    const size_t GetTemplateCount() override
    {
        if (mTemplates == nullptr)
            return 0;

        return sizeof(mTemplates) / sizeof(GraphEditor::Template);
    }

    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
    {
        return mTemplates[index];
    }

    const size_t GetNodeCount() override
    {
        return mNodes.size();
    }

    GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    {
        const auto& myNode = mNodes[index];
        return GraphEditor::Node
        {
            myNode.name,
            myNode.templateIndex,
            ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 100, myNode.y + 100)), // 노드별 사이즈
            myNode.mSelected,
        };
    }

    GraphEditorDelegate::Node GetDelegateNode(int Index)
    {
        return mNodes[Index];
    }

    bool GetSelectedDelegateNode(GraphEditorDelegate::Node& Output)
    {
        size_t Count = mNodes.size();

        for (size_t i = 0; i < Count; ++i)
        {
            if (mNodes[i].mSelected)
            {
                Output = mNodes[i];
                return true;
            }
        }

        return false;
    }

    const size_t GetLinkCount() override
    {
        return mLinks.size();
    }

    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
    {
        return mLinks[index];
    }

    void AddNode(const std::string& Name, GraphEditor::TemplateIndex TemplateIndex, float PosX, float PosY, bool mSelected, CNode* TreeNode)
    {
        Node NewNode = {};

        NewNode.name = new char[256];

        size_t Length = Name.length();
        for(size_t i = 0 ; i < Length; ++i)
            NewNode.name[i] = Name[i];
        NewNode.name[Length] = '\0';

        NewNode.x = PosX;
        NewNode.y = PosY;
        NewNode.mSelected = mSelected;
        NewNode.BehaviorTreeNode = TreeNode;
        NewNode.templateIndex = TemplateIndex;

        size_t CurrentCount = mNodes.size();
        NewNode.NodeIndex = (int)CurrentCount;

        mNodes.push_back(NewNode);
    }

    Node& FindNode(const std::string& Name)
    {
        size_t Count = mNodes.size();

        for (size_t i = 0; i < Count; ++i)
        {
            if (mNodes[i].name == Name)
                return mNodes[i];
        }
    }

    Node& FindNode(CNode* Node)
    {
        size_t Count = mNodes.size();

        for (size_t i = 0; i < Count; ++i)
        {
            if (mNodes[i].BehaviorTreeNode == Node)
                return mNodes[i];
        }
    }

    //void AddTemplateChild(GraphEditor::TemplateIndex TemplateIndex)
    //{
    //    ++mTemplates[TemplateIndex].mOutputCount;
    //    mTemplates[TemplateIndex].mOutputColors = mTemplates[TemplateIndex - 1].mOutputColors;
    //    std::string Name = "Child";
    //    mTemplates[TemplateIndex].mOutputNames = Name.c_str();
    //}

    // Graph datas
    static inline GraphEditor::Template mTemplates[] =
    {
        // Sequence Node(4 Children) Template
        {
            IM_COL32(140, 200, 160, 255),
            IM_COL32(140, 200, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Array{"Parent"},
            nullptr,
            4,
            Array{"Child0", "Child1", "Child2", "Child3"},
            nullptr
        },

        // Selector Node(4 Children) Template
        {
            IM_COL32(100, 140, 190, 255),
            IM_COL32(100, 100, 180, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Array{"Parent"},
            nullptr,
            4,
            Array{"Child0", "Child1", "Child2", "Child3"},
            nullptr
        },

        // Action Node Template
        {
            IM_COL32(100, 160, 100, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            1,
            Array{"Parent"},
            nullptr,
            0,
            nullptr,
            nullptr
        },

        // Condition Node Templte
        {
            IM_COL32(180, 160, 160, 255),
            IM_COL32(140, 100, 100, 255),
            IM_COL32(150, 110, 110, 255),
            1,
            Array{"Parent"},
            nullptr,
            0,
            nullptr,
            nullptr,
        },

        // Negate Node Templte
        {
            IM_COL32(190, 190, 190, 255),
            IM_COL32(140, 170, 170, 255),
            IM_COL32(150, 100, 90, 255),
            1,
            Array{"Parent"},
            nullptr,
            1,
            Array{"Child0"},
            nullptr,
        }
    };

    std::vector<Node> mNodes = 
    {
        //{
        //    "Sequence",
        //    0,
        //    50, 50,
        //    false
        //},

        //{
        //    "Selector",
        //    1,
        //    50, 50,
        //    false
        //},

        //{
        //    "Action",
        //    2,
        //    50, 50,
        //    false
        //},

        //{
        //    "Condition",
        //    3,
        //    50, 50,
        //    false
        //},
        //{
        //    "My Node 1",
        //    0,
        //    400, 100,
        //    false
        //},

        //{
        //    "My Node 2",
        //    1,
        //    400, 400,
        //    false
        //}
    };

    std::vector<GraphEditor::Link> mLinks = { };

};

class CBehaviorTreeWindow :
    public CIMGUIWindow
{
public:
	CBehaviorTreeWindow();
	virtual ~CBehaviorTreeWindow();

private:
    GraphEditor::Options m_Option;
    GraphEditor::ViewState m_ViewState;
    ImGuiComboFlags m_ComboFlag;
    bool m_Select;
    int m_TypeSelectIndex;
    int m_ActionSelectIndex;
    std::vector<std::string>	m_vecNodeType;
    std::vector<std::string>	m_vecNodeAction;
    std::string m_PrevViewName;
    std::string m_LeafNodePrevViewName;
    std::string m_InputNodeName; 

    class CStateComponent* m_StateComponent;

    char		m_Text[1024];
    wchar_t		m_wText[1024];
    char		m_TextUTF8[1024];

    char		m_HintText[1024];
    wchar_t		m_wHintText[1024];
    char		m_HintTextUTF8[1024];
    ImGuiInputTextFlags	m_TextInputFlag;

public:
    GraphEditorDelegate m_Delegate;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
    void SetStateComponent(class CStateComponent* Com);
    class CStateComponent* GetStateComponent() const;

private:
    void OnAddNodeButton(const char* Name, int TypeIndex, int ActionIndex);
    void OnDeleteNodeButton(const char* Name);

    void SetHintText(const char* Text)
    {
        memset(m_wHintText, 0, sizeof(wchar_t) * 1024);
        memset(m_HintText, 0, sizeof(char) * 1024);
        memset(m_HintTextUTF8, 0, sizeof(char) * 1024);
        int	Length = MultiByteToWideChar(CP_ACP, 0, Text, -1, 0, 0);
        MultiByteToWideChar(CP_ACP, 0, Text, -1, m_wHintText, Length);

        strcpy_s(m_HintText, Text);

        // UTF8로 변환한다.
        Length = WideCharToMultiByte(CP_UTF8, 0, m_wHintText, -1, 0, 0, 0, 0);
        WideCharToMultiByte(CP_UTF8, 0, m_wHintText, -1, m_HintTextUTF8, Length, 0, 0);
    }

private:
    void PrintActionNodes();
    void PrintConditionNodes();

public:
    // GameObject가 로드됐을때 Graph Editor상에 노드들 갱신
    void UpdateLoadNode(CCompositeNode* RootNode);
    void UpdateLoadNodeLink(class CBehaviorTree* Tree);
    void UpdateLoadNodeRecursive(CNode* Node);
    void Clear();
};

