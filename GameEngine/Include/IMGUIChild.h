#pragma once

#include "IMGUIWidget.h"

class CIMGUIChild :
    public CIMGUIWidget
{
public:
    CIMGUIChild();
    virtual ~CIMGUIChild();

public:
    virtual bool Init() override;
    virtual void Render() override;

private:
    bool m_SameLine;

public:
    // 경계에 라인 출력 여부
    void EnableBorder(bool bEnable)
    {
        mbBorder = bEnable;
    }

    void SetSameLine(bool SameLine)
    {
        m_SameLine = SameLine;
    }

    bool IsSameLine()   const
    {
        return m_SameLine;
    }

    void ClearWidget();

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
    bool mbBorder;
};

