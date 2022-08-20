#pragma once

#include "SceneComponentWidget.h"
#include "IMGUIText.h"
#include "IMGUIInputInt2.h"
#include "IMGUIInputFloat.h"
#include "IMGUIButton.h"
#include "IMGUITextInput.h"
#include "IMGUICheckBox.h"
#include "IMGUICollapsingHeader.h"
#include "IMGUISeperator.h"
#include "IMGUIImage.h"

class CLandScapeWidget:
    public CSceneComponentWidget
{
public:
    CLandScapeWidget();
    virtual ~CLandScapeWidget();

public:
    virtual bool Init() override;

public:
    // 반드시 Init 이후 이 함수 호출해야 함
    // SceneComponent 상속 위젯 만들때마다 오버라이드 해야 함
    virtual void SetSceneComponent(class CSceneComponent* Com);

public:
    void OnCheckDebugRender(int Index, bool Check);
    void OnClickLoadTexture();
    void OnClickLoadHeightMap();
    void OnClickCreateLandScape();
    void OnClickChangeYFactor();

private:
    CIMGUICheckBox* m_DebugRender;
    CIMGUITextInput* m_TextureName;
    CIMGUIButton* m_LoadTextureButton;
    CIMGUITextInput* m_HeightMapFullPathText;
    CIMGUIImage* m_HeightMapImage;
    CIMGUIButton* m_LoadHeightMapButton;
    CIMGUIInputInt2* m_CountInput;
    CIMGUIInputFloat* m_YFactorInput;
    CIMGUIButton* m_CreateButton;
    CIMGUIButton* m_ChangeYFactorButton;

    char m_HeightMapFullPath[MAX_PATH];
};

