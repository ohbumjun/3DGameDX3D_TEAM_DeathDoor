#pragma once

#include "IMGUIWidget.h"

// MainMenuBar Ŭ������ ���� �޴�, ���ٿ��� �ٷ� ���̴� �޴����� �����ϴ� ����
// MenuItem�� ���� �������� ������ ����

class CIMGUIBeginMenu :
    public CIMGUIWidget
{
public:
    CIMGUIBeginMenu();
    virtual ~CIMGUIBeginMenu();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    class CIMGUIMenuItem* AddMenuItem(const std::string& name, const float width = 100.f, const float height = 100.f);

protected:
    // std::vector<class CIMGUIWidget*> m_vecChild;
    std::vector<class CIMGUIMenuItem*> m_vecMenuChild;
    size_t mSize;
};

