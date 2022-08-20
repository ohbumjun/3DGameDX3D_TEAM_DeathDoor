#pragma once

#include "ObjectComponentWidget.h"

class CTriggerBoxDataWidget :
    public CObjectComponentWidget
{
public:
    CTriggerBoxDataWidget();
    virtual ~CTriggerBoxDataWidget();


public:
    virtual bool Init() override;

public:
    virtual void SetObjectComponent(class CObjectComponent* Com) override;

public:
    void OnSetCurrentActive(int Idx, bool Active);
    void OnSelectOrder(int Order, const char* Label);

private:
    class CTriggerBoxData* m_Data;

    class CIMGUICheckBox* m_CurrentActive;
    class CIMGUIComboBox* m_SelectOrder;
};

