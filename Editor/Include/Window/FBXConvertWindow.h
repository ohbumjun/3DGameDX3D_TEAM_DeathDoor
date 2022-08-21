#pragma once

#include "IMGUIWindow.h"
#include "../Thread/FBXConvertThread.h"
#include "EngineUtil.h"

// 1. packaged_task �� ���� promise-future �������� ������ ���� �� �޾ƿ���
// 2. �Ϻ��� ���� ���� 
// 3. �������� ���ø� + std::function ��ü�� ����, ������ ���� �Լ� Ȯ�强 ����
// 4. ���Ͷ� ��� ����ȭ ���� 
// Get �Լ��� ���� EndFunction �����ϱ�
// ThreadPool ���� make_shared �����ϴ� ����
// (�Ϻ��� ���� ���� + �������� ���ø�) �ѹ� �� �����ϱ�
// + (Ʈ���� �ڷᱸ��) �� ���� ���ڿ� Ž�� ����ȭ
// + (�������� ��� ����ȭ �õ��ϱ�)
// + MemoryPool �ϼ�

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

	// Ư�� ���� �̸� ���ĵ鸸 Convert ��Ű��
private :
	CIMGUITextInput* m_SpecificFileNameInput;
};
