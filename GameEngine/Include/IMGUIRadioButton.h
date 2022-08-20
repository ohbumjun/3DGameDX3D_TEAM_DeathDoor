#pragma once

#include "IMGUIWidget.h"
#include "IMGUIInfo.h"

class CIMGUIRadioButton :
    public CIMGUIWidget
{
public:
    CIMGUIRadioButton();
    virtual ~CIMGUIRadioButton();

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
    void SetCheck(int idx, bool check)
    {
        if (idx >= mVecCheckInfo.size())
        {
            return;
        }

        mVecCheckInfo[idx]->bCheck = check;
        mCheckItemIdx = idx;
    }

    // ���� �� ��ư�� �� �ٿ� ��ġ�� ��� �� ���� �����ϴ� ���
    void SetColNum(const int val)
    {
        mbMultiColumn = true;
        mColNum = val;
    }

    // �ʺ� ����
    void SetSpacingX(const float spacing)
    {
        mSpacingX = spacing;
    }

    // ���� üũ Ȱ��ȭ ����
    void SetMultiCheck(const bool bMultiCheck)
    {
        mbMultiCheck = bMultiCheck;
    }

    // �׻� �ϳ� �̻��� üũ�Ǿ� �־�� �ϴ��� ����
    void AlwaysCheck(bool Check)
    {
        mAlwaysCheck = Check;
    }

public:
    bool GetCheck(const int idx) const
    {
        return mVecCheckInfo[idx]->bCheck;
    }

    int GetCheckItemIdx()   const
    {
        return mCheckItemIdx;
    }

public:
    template <typename T>
    void SetCallBack(T* obj, void(T::* func)(const char*, bool))
    {
        mCallBack = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);
    }

protected:
    std::vector<CheckInfo*> mVecCheckInfo; 
    bool mbMultiColumn;
    int mColNum;
    float mSpacingX;
    std::function<void(const char*, bool)> mCallBack;
    
    bool mbMultiCheck;
    bool mAlwaysCheck; // �׻� �ϳ� �̻��� üũ�Ǿ� �־�� �Ѵٸ�
    int mCheckItemIdx;
};

