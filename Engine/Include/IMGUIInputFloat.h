#pragma once

#include "IMGUIWidget.h"

class CIMGUIInputFloat :
    public CIMGUIWidget
{
public:
    CIMGUIInputFloat();
    virtual ~CIMGUIInputFloat();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    void SetVal(const float val)
    {
        mVal = val;
    }

    float GetVal() const
    {
        return mVal;
    }

    void SetStep(const float step)
    {
        mStep = step;
    }

    float GetStep() const
    {
        return mStep;
    }

public:
    template <typename T>
    void SetCallBack(T* obj, void(T::* func)(float))
    {
        mCallBack = std::bind(func, obj, std::placeholders::_1);
    }

protected:
    float mVal;
    float mStep;
    std::function<void(float)> mCallBack;
};

