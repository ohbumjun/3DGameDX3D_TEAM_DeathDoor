#pragma once

#include "IMGUIWindow.h"
#include "../Thread/FBXConvertThread.h"
#include "EngineUtil.h"

// 1. packaged_task 를 통해 promise-future 패턴으로 쓰레드 리턴 값 받아오기
// 2. 완벽한 전달 패턴 
// 3. 가변인자 템플릿 + std::function 객체를 통해, 쓰레드 실행 함수 확장성 제공
// 4. 인터락 기반 동기화 적용 
// Get 함수를 통한 EndFunction 세팅하기
// ThreadPool 에서 make_shared 적용하는 이유
// (완벽한 전달 패턴 + 가변인자 템플릿) 한번 더 공부하기
// + (트라이 자료구조) 를 통한 문자열 탐색 최적화
// + (세마포어 기반 동기화 시도하기)
// + MemoryPool 완성

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
	class CPathStringTrie* m_PathStringTrie;


	char m_SrcDirFullPath[MAX_PATH];
	bool m_SingleFileMode;

	// 특정 파일 이름 형식들만 Convert 시키기
private :
	CIMGUITextInput* m_SpecificFileNameInput;
};
