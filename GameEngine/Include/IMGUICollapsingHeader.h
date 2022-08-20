#pragma once
#include "IMGUIWidget.h"
class CIMGUICollapsingHeader :
    public CIMGUIWidget
{
public:
    CIMGUICollapsingHeader();
    virtual ~CIMGUICollapsingHeader();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    void ClearWidget();
    void DeleteWidget(CIMGUIWidget* widget);

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
        mVecChild.push_back(widget);
        return widget;
    }

protected:
    std::vector<class CIMGUIWidget*> mVecChild;
    size_t mSize;
};

