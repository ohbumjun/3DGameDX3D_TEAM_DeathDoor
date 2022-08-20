#pragma once

#include "IMGUIWidget.h"

class CIMGUIInputInt :
    public CIMGUIWidget
{
public:
    CIMGUIInputInt();
    virtual ~CIMGUIInputInt();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    void SetVal(const int val)
    {
        mVal = val;
    }

    int GetVal() const
    {
        return mVal;
    }

    void SetStep(const int step)
    {
        mStep = step;
    }

    int GetStep() const
    {
        return mStep;
    }

public:
    template <typename T>
    void SetCallBack(T* obj, void(T::* func)(int))
    {
        mCallBack = std::bind(func, obj, std::placeholders::_1);
    }

protected:
    int mVal;
    int mStep; // +, - 시 증,감소되는 간격 
    std::function<void(int)> mCallBack;
};

