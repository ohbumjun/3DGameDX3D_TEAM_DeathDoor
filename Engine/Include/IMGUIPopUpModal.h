#pragma once

#include "IMGUIWidget.h"

class CIMGUIPopUpModal :
    public CIMGUIWidget
{
public:
	CIMGUIPopUpModal();
	~CIMGUIPopUpModal();

public:
    virtual bool Init() override;
    virtual void Render() override;

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
	PopUpModalState		m_State;

public:
	PopUpModalState	GetPopUpModalState()	const
	{
		return m_State;
	}

	void SetPopUpModalState(PopUpModalState State)
	{
		m_State = State;
	}
};

