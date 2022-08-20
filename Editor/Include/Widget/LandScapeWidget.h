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
    // �ݵ�� Init ���� �� �Լ� ȣ���ؾ� ��
    // SceneComponent ��� ���� ���鶧���� �������̵� �ؾ� ��
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

