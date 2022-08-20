#pragma once

#include "Node.h"

class CDecoratorNode :
    public CNode
{
public:
    CDecoratorNode();
    CDecoratorNode(const CDecoratorNode& Node);
    virtual ~CDecoratorNode();

public:
    virtual NodeResult OnStart(float DeltaTime) = 0;
    virtual NodeResult OnUpdate(float DeltaTime) = 0;
    virtual NodeResult OnEnd(float DeltaTime) = 0;

protected:
    CNode* m_Child;

public:
    void SetChild(CNode* node)
    {
        node->SetParent(this);
        m_Child = node;
    }

    CNode* GetChild()   const
    {
        return m_Child;
    }

public:
    virtual NodeResult Invoke(float DeltaTime) = 0;

public:
    virtual bool Save(FILE* File);
    virtual bool Load(FILE* File);
    virtual bool SaveOnly(FILE* File);
    virtual bool LoadOnly(FILE* File);
};

