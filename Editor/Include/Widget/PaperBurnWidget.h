#pragma once

#include "ObjectComponentWidget.h"

class CPaperBurnWidget :
    public CObjectComponentWidget
{
public:
    CPaperBurnWidget();
    virtual ~CPaperBurnWidget();

public:
    virtual bool Init() override;

public:
    virtual void SetObjectComponent(class CObjectComponent* Com) override;

public:
    void OnClickLoad();
    void OnSelectEndEventType(int Idx, const char* Label);
    void OnClickFindComponent();
    void OnCheckInverse(int Idx, bool Bool);
    void OnChangeFinishTime(float Val);
    void OnChangeInFilter(float Val);
    void OnChangeCenterFilter(float Val);
    void OnChangeOutFilter(float Val);
    void OnChangeInColor(const Vector4& Color);
    void OnChangeCenterColor(const Vector4& Color);
    void OnChangeOutColor(const Vector4& Color);
    void OnClickStart();
    void OnDropTexture(const std::string& Name);

private:
    class CIMGUIComboBox* m_EndEventType;
    class CIMGUITextInput* m_PaperBurnComponent;
    class CIMGUIButton* m_FindCompnent;
    class CIMGUITextInput* m_TexName;
    class CIMGUIButton* m_LoadTex;
    class CIMGUIImage* m_Tex;
    class CIMGUICheckBox* m_Inverse;
    class CIMGUIInputFloat* m_FinishTime;
    class CIMGUIColor4* m_OutlineColor;
    class CIMGUIInputFloat* m_OutFilter;
    class CIMGUIColor4* m_CenterlineColor;
    class CIMGUIInputFloat* m_CenterFilter;
    class CIMGUIColor4* m_InlineColor;
    class CIMGUIInputFloat* m_InFilter;
    class CIMGUIButton* m_StartButton;
};

