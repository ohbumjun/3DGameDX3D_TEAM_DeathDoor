#pragma once

#include "IMGUIWidget.h"

// BeginMenu¿« «œ¿ß ¿ß¡¨

class CIMGUIMenuItem :
    public CIMGUIWidget
{
public:
    CIMGUIMenuItem();
    virtual ~CIMGUIMenuItem();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    void SetShortCut(const std::string& shortCut)
    {
        mShortCut = mShortCut;
    }

    void SetEnable(bool bEnable)
    {
        mbEnable = bEnable;
    }


public:
    bool IsSelected()
    {
        return mbSelect;
    }

public:
    template <typename T>
    void SetClickCallBack(T* obj, void(T::* func)())
    {
        mCallBack = std::bind(func, obj);
    }

private:
    std::string mShortCut;
    bool mbSelect;
    bool mbEnable;
    std::function<void()> mCallBack;
private :
    bool m_EnableCheck;
};

