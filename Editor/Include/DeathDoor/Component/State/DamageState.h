#pragma once

#include "Component/State/State.h"

// ���������� ���ݹ޾Ƽ� �������� ���� ����
class CDamageState :
    public CState
{
    friend class CGameObject;
    friend class CGameStateManager;

protected:
    CDamageState();
    CDamageState(const CDamageState& State);
    virtual ~CDamageState();

public:
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CDamageState* Clone();

private:
    int m_Damage;

public:
    void SetDamage(int Damage)
    {
        m_Damage = Damage;
    }

    int GetDamage() const
    {
        return m_Damage;
    }

public:
    virtual void OnStart();
    virtual void OnEnd();
};

