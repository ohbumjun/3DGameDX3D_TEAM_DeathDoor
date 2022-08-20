#pragma once

#include "IMGUIWidget.h"

enum class TreeDropType
{
    Normal,
    Text
};

class CIMGUITree :
    public CIMGUIWidget
{
public:
    CIMGUITree();
    virtual ~CIMGUITree();
private :
    TreeDropType m_DropType;
    void* m_DataPtr; // Tree가 가리키는 Engine상에서 객체(ex. GameObject, SceneComponent 등등) -> 이름만으로 찾기에 위험하다고 판단되면 m_Data도 비교

public:
    void SetData(void* Data)
    {
        m_DataPtr = Data;
    }

    void* GetData() const
    {
        return m_DataPtr;
    }

    void SetParent(CIMGUITree* parent)
    {
        mParent = parent;
    }

    bool IsRootNode()
    {
        return mParent == nullptr;
    }

    bool IsEndNode()
    {
        return mVecChild.size() == 0;
    }

    CIMGUITree* GetParent() const
    {
        return mParent;
    }
    
public:
    CIMGUITree* GetNode(const int idx);

public:
    CIMGUITree* FindChild(const std::string& Name);
    CIMGUITree* FindChild(const std::string& Name, void* DataPtr);
    CIMGUITree* AddChild(const std::string& name, void* Data = nullptr, const float width = 100.f, const float height = 100.f);
    CIMGUITree* AddChild(CIMGUITree* Tree, void* Data = nullptr, const float width = 100.f, const float height = 100.f);
    CIMGUITree* AddNewChild(CIMGUITree* Tree);
    void DeleteChild(const std::string& Name);
    CIMGUITree* DeleteHierarchy(); // 자식 Tree 구조 전부 삭제
    CIMGUITree* Delete();
    size_t GetChildCount() const;
    CIMGUITree* GetRoot();

    // 자신부터 시작해서 아래로 내려가면서 m_Selected = false로 해주는 함수
    void DeselectAll();

public:
    void SetFlag(ImGuiTreeNodeFlags Flag)
    {
        m_Flag = Flag;
    }

    void AddFlag(ImGuiTreeNodeFlags Flag)
    {
        m_Flag |= Flag;
    }

    void SetSelected(bool Selected)
    {
        m_Selected = Selected;
    }

    bool IsSelected() const
    {
        return m_Selected;
    }

    // Drop 형태
    void SetTextDropType()
    {
        m_DropType = TreeDropType::Text;
    }

public:
    template <typename T>
    T* AddWidget(const std::string& name, const float width = 100.f, const float height = 100.f)
    {
		T* widget = new T;

		widget->SetName(name);
		widget->SetOwner(m_Owner);
		widget->SetSize(width, height);

		if (!widget->Init())
		{
			SAFE_DELETE(widget);
			assert(false);
			return nullptr;
		}
		mVecWidget.push_back(widget);
        mVecRender.push_back(widget);
        return widget;
    }


public:
    virtual bool Init();
    virtual void Render();

protected:
    CIMGUITree* mParent;
    std::vector<CIMGUITree*> mVecChild;
    std::vector<CIMGUIWidget*> mVecWidget;
    std::vector<CIMGUIWidget*> mVecRender; 

    int m_GlobalID;
    bool m_Selected;
    bool m_Open;
    // false면 Window에서 출력 안함
    bool m_Enable;
    ImGuiTreeNodeFlags  m_Flag;
    ImGuiTreeNodeFlags  m_DefaultFlag;

    std::function<void(class CIMGUITree*)> m_DragDropSourceCallback;
    std::function<void(class CIMGUITree*, const std::string&, const std::string&)> m_DragDropDestCallback;

    std::list<std::function<void(class CIMGUITree*)>> m_SelectCallbackList;
    std::list<std::function<void(class CIMGUITree*)>> m_OpenCallbackList;

public:
    bool IsOpen()   const
    {
        return m_Open;
    }

    void SetEnable(bool Enable)
    {
        m_Enable = Enable;
    }

    // 자신을 포함해서 밑에 자식까지 전부 m_Enable을 true
    void EnableAll()
    {
        m_Enable = true;

        size_t Count = mVecChild.size();

        for (size_t i = 0; i < Count; ++i)
        {
            mVecChild[i]->EnableAll();
        }
    }

    // 자신을 포함해서 밑에 자식까지 전부 m_Enable을 false
    void DisableAll()
    {
        m_Enable = false;

        size_t Count = mVecChild.size();

        for (size_t i = 0; i < Count; ++i)
        {
            mVecChild[i]->DisableAll();
        }
    }

    bool IsEnable() const
    {
        return m_Enable;
    }

public:
    template <typename T>
    void SetDragDropSourceCallback(T* Obj, void(T::* Func)(class CIMGUITree*))
    {
        m_DragDropSourceCallback = std::bind(Func, Obj, std::placeholders::_1);
    }

    template <typename T>
    void SetDragDropDestCallback(T* Obj, void(T::*Func)(class CIMGUITree*, const std::string&, const std::string&))
    {
        m_DragDropDestCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    template <typename T>
    void AddSelectCallback(T* Obj, void(T::* Func)(class CIMGUITree*))
    {
        m_SelectCallbackList.push_back(std::bind(Func, Obj, std::placeholders::_1));
    }

    template <typename T>
    void AddOpenCallback(T* Obj, void(T::* Func)(class CIMGUITree*))
    {
        m_OpenCallbackList.push_back(std::bind(Func, Obj, std::placeholders::_1));
    }
private :
    void ApplyDropEffect();
    void ApplyNormalDrop();
    void ApplyTextDrop();
private :
    void ApplyDragEffect();
    void ApplyNormalDrag();
    void ApplyTextDrag();
};

