#pragma once

#include "IMGUIWindow.h"
#include "../Thread/FBXConvertThread.h"
#include "EngineUtil.h"

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
	void OnClearSrcPaths();
	void OnLoading(const LoadingMessage& msg);
	void OnLoadFail(const std::string& failedPathName);
	void FileConvertEndFunc();
private:
	class CThreadPool* m_ThreadPool;
	// CFBXConvertThread* m_ConvertThread;
	class CIMGUICheckBox* m_SingleFileModeCheckBox;
	class CIMGUITextInput* m_SrcFullDirText;
	class CIMGUIButton* m_SetSrcDirButton;
	class CIMGUIButton* m_ClearSrcButton;
	class CIMGUIChild* m_AddedSrcLog;
private :
	class CIMGUIProgressBar* m_ProgressBar;
	class CIMGUIButton* m_ConvertButton;
	class CIMGUIChild* m_ConvertLog;
private :
	LONG m_CurNumConvertComplete;
	LONG m_MaxNumConvertComplete;
	bool m_ConvertOngoing;

	// char m_SrcFileFullPath[MAX_PATH];
	std::vector<std::string> m_vecSrcFilePaths;
	char m_SrcDirFullPath[MAX_PATH];

	bool m_SingleFileMode;

	// 특정 파일 이름 형식들만 Convert 시키기
private :
	CIMGUITextInput* m_SpecificFileNameInput;
};
