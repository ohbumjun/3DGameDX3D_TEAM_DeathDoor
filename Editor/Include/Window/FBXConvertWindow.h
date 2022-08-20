#pragma once

#include "IMGUIWindow.h"
#include "../Thread/FBXConvertThread.h"
#include "IMGUICheckBox.h"
#include "IMGUIText.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUITextInput.h"
#include "IMGUIButton.h"
#include "IMGUIProgressBar.h"
#include "IMGUIChild.h"

class CFBXConvertWindow :
	public CIMGUIWindow
{
public:
	CFBXConvertWindow();
	virtual ~CFBXConvertWindow();

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	void OnSelectSingleFileMode(int idx, bool Single);
	void OnClickSetSrcDirButton();
	void OnClickConvertButton();
	void OnLoading(const LoadingMessage& msg);
	void OnLoadFail(const std::string& failedPathName);

private:
	CFBXConvertThread* m_ConvertThread;
	CIMGUICheckBox* m_SingleFileModeCheckBox;
	CIMGUITextInput* m_SrcDirText;
	CIMGUIButton* m_SetSrcDirButton;
	CIMGUIButton* m_ConvertButton;
	CIMGUIProgressBar* m_ProgressBar;
	CIMGUIChild* m_ConvertLog;

	char m_SrcFileFullPath[MAX_PATH];
	char m_SrcDirFullPath[MAX_PATH];

	bool m_SingleFileMode;

	// 특정 파일 이름 형식들만 Convert 시키기
private :
	CIMGUITextInput* m_SpecificFileNameInput;
};
