#pragma once

#include "IMGUIWidget.h"
#include "IMGUIInfo.h"

class CIMGUICheckBox :
    public CIMGUIWidget
{
public:
    CIMGUICheckBox();
    virtual ~CIMGUICheckBox();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    void AddCheckInfo(const char* label)
    {
        CheckInfo* info = new CheckInfo;
        
        memset(info->Label, 0, sizeof(char) * 128);
        memset(info->wLabel, 0, sizeof(wchar_t) * 128);
        memset(info->LabelUTF8, 0, sizeof(char) * 128);

        strcpy_s(info->Label, label);
        
        int length = MultiByteToWideChar(CP_ACP, 0, label, -1, nullptr, 0);
        MultiByteToWideChar(CP_ACP, 0, label, -1, info->wLabel, length);

        length = WideCharToMultiByte(CP_UTF8, 0, info->wLabel, -1, nullptr, 0, nullptr, nullptr);
        WideCharToMultiByte(CP_UTF8, 0, info->wLabel, -1, info->LabelUTF8, length, nullptr, nullptr);

        info->bCheck = false;
        mVecCheckInfo.push_back(info);
    }
public:
    void SetColNum(const int val)
    {
        mbMultiColumn = true;
        mColNum = val;
    }

    void SetSpacingX(const float spacing)
    {
        mSpacingX = spacing;
    }

    void SetCheck(const int idx, bool bCheck)
    {
        mVecCheckInfo[idx]->bCheck = bCheck;
    }

public:
    bool GetCheck(const int idx) const
    {
        return mVecCheckInfo[idx]->bCheck;
    }

    CheckInfo* GetInfo(const int idx) const
    {
        return mVecCheckInfo[idx];
    }

    size_t GetCount() const
    {
        return mVecCheckInfo.size();
    }

    int GetSelectIdx() const
    {
        size_t Count = mVecCheckInfo.size();
        
        for (size_t i = 0; i < Count; ++i)
        {
            if (mVecCheckInfo[i]->bCheck)
                return (int)i;
        }

        return -1;
    }

public:
    template <typename T>
    void SetCallBackLabel(T* obj, void(T::* func)(const char*, bool))
    {
        mCallBackLabel = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename T>
    void SetCallBackIdx(T* obj, void(T::* func)(int, bool))
    {
        mCallBackIdx = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);
    }

protected:
    std::vector<CheckInfo*> mVecCheckInfo; 
    bool mbMultiColumn;
    int mColNum;
    float mSpacingX;
    std::function<void(const char*, bool)> mCallBackLabel;
    std::function<void(int, bool)> mCallBackIdx;
    int SelectIdx;
};

