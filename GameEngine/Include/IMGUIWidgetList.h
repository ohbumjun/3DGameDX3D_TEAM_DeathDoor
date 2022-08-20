#pragma once

#include "IMGUIWidget.h"

// 위젯들을 child로 가지고 있는 위젯
class CIMGUIWidgetList :
    public CIMGUIWidget
{
public:
    CIMGUIWidgetList();
    virtual ~CIMGUIWidgetList();

private:
    bool m_ApplyHideEffect;
public :
    void SetApplyHideEffect(bool Enable)
    {
        m_ApplyHideEffect = Enable;
    }
public:
    virtual bool Init() override;
    virtual void Render() override;
private :
    void RenderHide();
    void RenderNoHide();
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

