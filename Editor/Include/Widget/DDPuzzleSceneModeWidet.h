#pragma once

#include "DDSceneModeWidget.h"

class CDDPuzzleSceneModeWidet :
    public CDDSceneModeWidget
{
public:
    CDDPuzzleSceneModeWidet();
    virtual ~CDDPuzzleSceneModeWidet();

public:
    virtual bool Init() override;

public:
    virtual void RefreshWidgets() override;

private:
    void OnCheckClearCamMove(int Idx, bool Enable);
    void OnClickFindBlokcerObj();
    void OnClickFindLadderObj();

private:
    class CIMGUICheckBox* m_ClearCamMove;

    class CIMGUITextInput* m_BlockerObjName;
    class CIMGUITextInput* m_BlockerObjNameInput;
    class CIMGUIButton* m_FindBlockerObj;

    class CIMGUITextInput* m_LadderObjName;
    class CIMGUITextInput* m_LadderObjNameInput;
    class CIMGUIButton* m_FindLadderObj;
};

