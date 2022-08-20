#pragma once

#include "../GameInfo.h"
#include "Node/Node.h"

class CBehaviorTree
{
    friend class CStateComponent;

public:
    CBehaviorTree();
    CBehaviorTree(const CBehaviorTree& Tree);
    virtual ~CBehaviorTree() = 0;

protected:
    class CNode* m_Root;
    class CStateComponent* m_Owner;
    std::list<class CNode*> m_NodeList;
    class CNode* m_CurrentNode; // 이번 프레임에는 Root가 아닌 CurrentNode부터 순회해야할때


    class CAnimationMeshComponent* m_AnimationMeshComp;
    
public:
    class CNode* GetCurrentNode()   const;
    void SetCurrentNode(class CNode* CurrentNode);
    void SetOwner(class CStateComponent* StateComp);
    class CStateComponent* GetOwner() const;

public:
    void SetAnimationMeshComponent(class CAnimationMeshComponent* Mesh);
    class CAnimationMeshComponent* GetAnimationMeshComp()   const;

    class CNode* GetRootNode()    const;
    void SetRoot(class CNode* Node);
    CNode* FindNode(const std::string& Name);

    template <typename T>
    CNode* FindNodeByType()
    {
        auto iter = m_NodeList.begin();
        auto iterEnd = m_NodeList.end();

        for (; iter != iterEnd; ++iter)
        {
            if ((*iter)->GetTypeID() == typeid(T).hash_code())
                return (*iter);
        }

        return nullptr;
    }

    void AddNode(class CNode* Node);
    void DeleteNode(class CNode* Node);

    size_t GetNodeCount()   const
    {
        return m_NodeList.size();
    }

    CNode* GetNode(int Idx)
    {
        auto iter = m_NodeList.begin();
        std::advance(iter, Idx);

        return *iter;
    }

public:
   virtual bool Init();
   virtual void Start();
   virtual void Update(float DeltaTime);
   virtual void PostUpdate(float DeltaTime);
   virtual void PrevRender();
   virtual void Render();
   virtual void PostRender();
   virtual bool Save(FILE* File);
   virtual bool Load(FILE* File);
   virtual bool SaveOnly(FILE* File);
   virtual bool LoadOnly(FILE* File);
   
private:
    std::function<CNode* (CNode*, size_t)>   m_NodeCreateCallback;

public:
    virtual CNode* LoadNode(CNode* Parent, size_t TypeID);

    template <typename T>
    void SetCreateNodeCallback(T* Obj, CNode*(T::* Func)(CNode* Parent, size_t TypeID))
    {
        m_NodeCreateCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
    }



public:
    

   /* template <typename T>
    T* CreateNode(const std::string& Name)
    {
        T* Node = new T;

        Node->SetName(Name);

        if (m_Owner)
            Node->SetObject(m_Owner->GetGameObject());

        Node->SetOwner(this);

        if (!m_Root)
            m_Root = Node;

        m_NodeList.push_back(Node);

        return Node;
    }*/
};

