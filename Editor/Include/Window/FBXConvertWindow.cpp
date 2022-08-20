#include "FBXConvertWindow.h"
#include "../EditorUtil.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "PathManager.h"
#include "IMGUICheckBox.h"
#include "IMGUIText.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUITextInput.h"
#include "IMGUIButton.h"
#include "IMGUIProgressBar.h"
#include "IMGUIChild.h"
#include "IMGUISameLine.h"
#include "Resource/ResourceManager.h"
#include "ThreadPool.h"

CFBXConvertWindow::CFBXConvertWindow()	:
	// m_ConvertThread(nullptr),
	m_SrcFullDirText(nullptr),
	m_SetSrcDirButton(nullptr),
	m_ConvertButton(nullptr),
	m_ProgressBar(nullptr),
	m_ConvertLog(nullptr),
	m_SrcDirFullPath{},
	m_ConvertOngoing(false)
{
	m_ThreadPool = new CThreadPool(1); 
	
	m_ThreadPool->SetEndFunction(0, std::bind(&CFBXConvertWindow::FileConvertEndFunc, this));
}

CFBXConvertWindow::~CFBXConvertWindow()
{
	// SAFE_DELETE(m_ConvertThread);

	SAFE_DELETE(m_ThreadPool);
}

bool CFBXConvertWindow::Init()
{
	// 변환 스레드
	// m_ConvertThread = new CFBXConvertThread;
	// m_ConvertThread->Init();
	// m_ConvertThread->Start();

	// Widget 생성
	CIMGUIText* text = AddWidget<CIMGUIText>("Text");
	text->SetText("FBX Converter");

	AddWidget<CIMGUISeperator>("sep");

	m_SingleFileModeCheckBox = AddWidget<CIMGUICheckBox>("Single File", 0.f, 0.f);
	m_SingleFileModeCheckBox->AddCheckInfo("Single File");
	m_SingleFileModeCheckBox->SetCallBackIdx(this, &CFBXConvertWindow::OnSelectSingleFileMode);
	
	m_SpecificFileNameInput = AddWidget<CIMGUITextInput>("SpecifieFileName", 300.f);
	m_SpecificFileNameInput->SetHideName(true);
	m_SpecificFileNameInput->SetHintText("SpecifieFileName");

	m_SrcFullDirText = AddWidget<CIMGUITextInput>("Dir Path", 300.f);
	m_SrcFullDirText->ReadOnly(true);
	m_SrcFullDirText->SetHideName(true);
	m_SrcFullDirText->SetHintText("Set FBX Folder Path Only");

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("line");

	m_SetSrcDirButton = AddWidget<CIMGUIButton>("AddSrc", 0.f, 0.f);
	m_SetSrcDirButton->SetClickCallback(this, &CFBXConvertWindow::OnClickSetSrcDirButton);

	text = AddWidget<CIMGUIText>("Text");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(315);

	m_ClearSrcButton = AddWidget<CIMGUIButton>("Clear Src", 0.f, 0.f);
	m_ClearSrcButton->SetClickCallback(this, &CFBXConvertWindow::OnClearSrcPaths);

	text = AddWidget<CIMGUIText>("Text");
	text->SetText("Target Srces");

	m_AddedSrcLog = AddWidget<CIMGUIChild>("Target Sources Log", 550.f, 150.f);
	m_AddedSrcLog->EnableBorder(true);

	// Convert
	m_ProgressBar = AddWidget<CIMGUIProgressBar>("", 450.f, 0.f);
	m_ProgressBar->SetPercent(0.f);

	AddWidget<CIMGUISameLine>("line");

	m_ConvertButton = AddWidget<CIMGUIButton>("Convert", 0.f, 0.f);
	m_ConvertButton->SetClickCallback(this, &CFBXConvertWindow::OnClickConvertButton);

	text = AddWidget<CIMGUIText>("Text");
	text->SetText("Log");

	m_ConvertLog = AddWidget<CIMGUIChild>("Log", 550.f, 250.f);
	m_ConvertLog->EnableBorder(true);

	// m_ConvertThread->SetLoadingCallBack(this, &CFBXConvertWindow::OnLoading);

	return true;
}

void CFBXConvertWindow::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);
}

void CFBXConvertWindow::OnSelectSingleFileMode(int idx, bool Single)
{
	m_SingleFileMode = Single;
}

void CFBXConvertWindow::OnClickSetSrcDirButton()
{
	if (m_ConvertOngoing)
	{
		MessageBox(nullptr, TEXT("진행중"), TEXT("진행중"), MB_OK);
		return;
	}

	if (m_SingleFileMode)
	{
		TCHAR   FilePath[MAX_PATH] = {};

		OPENFILENAME    OpenFile = {};

		OpenFile.lStructSize = sizeof(OPENFILENAME);
		OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
		OpenFile.lpstrFilter = TEXT("FBX File(.fbx)\0*.fbx\0");
		OpenFile.lpstrFile = FilePath;
		OpenFile.nMaxFile = MAX_PATH;
		OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MESH_PATH)->Path;

		if (GetOpenFileName(&OpenFile) != 0)
		{
			char SrcFileFullPath[MAX_PATH];

			int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
			WideCharToMultiByte(CP_ACP, 0, FilePath, -1, SrcFileFullPath, Length, 0, 0);

			// m_SrcDirText->SetText(m_SrcFileFullPath);
			m_vecSrcFilePaths.push_back(SrcFileFullPath);

			// Log 에 출력한다.
			CIMGUIText* Text = m_AddedSrcLog->AddWidget<CIMGUIText>(SrcFileFullPath);
			Text->SetText(SrcFileFullPath);
		}
	}
	else
	{
		TCHAR Buf[MAX_PATH] = {};

		BROWSEINFO Bi = {};

		Bi.hwndOwner = CEngine::GetInst()->GetWindowHandle();
		Bi.lpszTitle = TEXT("변환할 FBX파일들이 있는 폴더 선택");
		Bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
		Bi.lpfn = BrowseCallbackProc;
	 //	Bi.lParam = (LPARAM)InitialPath;

		LPITEMIDLIST ItemIDList = ::SHBrowseForFolder(&Bi);

		if (::SHGetPathFromIDList(ItemIDList, Buf))
		{
			int length = WideCharToMultiByte(CP_ACP, 0, Buf, -1, nullptr, 0, nullptr, 0);
			WideCharToMultiByte(CP_ACP, 0, Buf, -1, m_SrcDirFullPath, length, nullptr, 0);
			strcat_s(m_SrcDirFullPath, "\\");

			m_SrcFullDirText->SetText(m_SrcDirFullPath);
		}
	}

}

void CFBXConvertWindow::OnClickConvertButton()
{
	if (m_ConvertOngoing)
	{
		MessageBox(nullptr, TEXT("진행중"), TEXT("진행중"), MB_OK);
		return;
	}

	// 예외처리
	// 파일 하나하나 처리
	if (m_SingleFileMode)
	{
		// if (strlen(m_SrcFileFullPath) == 0)
		if (m_vecSrcFilePaths.empty())
		{
			return;
		}
	}
	// 파일 여러개 처리 
	else
	{
		if (strlen(m_SrcDirFullPath) == 0)
		{
			return;
		}
	}

	// 로그창 클리어
	m_ConvertLog->ClearWidget();

	CIMGUIText* StartText = m_ConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");

	// 파일 하나만 변환하는 경우
	if (m_SingleFileMode)
	{
		// m_ConvertThread->AddWork(m_SrcFullDirText);
		size_t SrcSize = m_vecSrcFilePaths.size();

		for (size_t i = 0; i < SrcSize; ++i)
		{
			auto Function = std::bind(&CResourceManager::ConvertFBXLocalFormatFullPathMultiByte, CResourceManager::GetInst(), m_vecSrcFilePaths[i].c_str());
			
			m_ThreadPool->EnqueueJob(Function, NULL);
		}

		// 한번 Convert 한 Path 정보는 모두 지워줄 것이다
		// m_vecSrcFilePaths.clear();

		// Widget 정보도 모두 지워준다.
		m_AddedSrcLog->ClearWidget();
	}
	else
	{
		if (m_vecSrcFilePaths.empty() == false)
			assert(false);

		// 폴더 내의 모든 FBX 파일의 경로를 받아옴
		std::vector<std::string> VecFullPath;
		CEditorUtil::GetAllFileFullPathInDir(m_SrcDirFullPath, VecFullPath, ".fbx");

		// 만약 특정 이름의 파일. 들만 Convert 하고 싶다면, 해당 목록을 뽑아온다.
		if (!m_SpecificFileNameInput->Empty())
		{
			std::vector<std::string> SpecificVecFullPath;
			
			CEditorUtil::FilterSpecificNameIncludedFilePath(VecFullPath, m_vecSrcFilePaths, m_SpecificFileNameInput->GetTextUTF8());
			// CEditorUtil::FilterSpecificNameIncludedFilePath(SpecificVecFullPath, m_vecSrcFilePaths, m_SpecificFileNameInput->GetTextUTF8());
		}

		size_t Size = m_vecSrcFilePaths.size();

		for (size_t i = 0; i < Size; ++i)
		{
			auto Function = std::bind(&CResourceManager::ConvertFBXLocalFormatFullPathMultiByte, CResourceManager::GetInst(), m_vecSrcFilePaths[i].c_str());

			m_ThreadPool->EnqueueJob(Function, NULL);
		}
	}

	m_CurNumConvertComplete = 0;
	m_MaxNumConvertComplete = (LONG)m_vecSrcFilePaths.size();
}

void CFBXConvertWindow::OnClearSrcPaths()
{
	if (m_ConvertOngoing)
	{
		MessageBox(nullptr, TEXT("진행중"), TEXT("진행중"), MB_OK);
		return;
	}

	m_vecSrcFilePaths.clear();
	m_vecPopupWidget.clear();
}

void CFBXConvertWindow::OnLoading(const LoadingMessage& msg)
{
	m_ProgressBar->SetPercent(msg.Percent);
	CIMGUIText* Text = m_ConvertLog->AddWidget<CIMGUIText>("Text");
	std::string LoadingMsg = "Convert Success : " + CEditorUtil::FilterFileName(msg.Message);
	Text->SetText(LoadingMsg.c_str());

	if (msg.Complete)
	{
		m_ConvertOngoing = false;
		Text  = m_ConvertLog->AddWidget<CIMGUIText>("OK");
		Text->SetText("Complete!");
		MessageBox(nullptr, TEXT("변환 완료"), TEXT("완료"), MB_OK);
	}
}

void CFBXConvertWindow::OnLoadFail(const std::string& failedPathName)
{
	m_ProgressBar->SetPercent(0.f);

	std::string ErrorText = failedPathName + " 을 로드하는데 실패";

	char Buf[256] = {};
	strcpy_s(Buf, ErrorText.c_str());
	
	TCHAR BufW[MAX_PATH] = {};
	int Length = MultiByteToWideChar(CP_ACP, 0, Buf, -1, nullptr, 0);
	MultiByteToWideChar(CP_ACP, 0, Buf, -1, BufW, Length);

	MessageBox(nullptr, TEXT("오류"), BufW, MB_OK);

	CIMGUIText* Text  = m_ConvertLog->AddWidget<CIMGUIText>("OK");
	std::string LogMsg = "Failed To Load " + failedPathName;
	Text->SetText(LogMsg.c_str());
}

void CFBXConvertWindow::FileConvertEndFunc()
{
	InterlockedIncrement(&m_CurNumConvertComplete);

	float CurPercent = (float)m_CurNumConvertComplete / (float)m_MaxNumConvertComplete;
	m_ProgressBar->SetPercent(CurPercent);

	CIMGUIText* Text = m_ConvertLog->AddWidget<CIMGUIText>("Text");

	std::string LoadingMsg = "Convert Success : " + CEditorUtil::FilterFileName(m_vecSrcFilePaths[m_CurNumConvertComplete - 1]);

	Text->SetText(LoadingMsg.c_str());

	if (m_CurNumConvertComplete > m_MaxNumConvertComplete)
		assert(false);

	if (m_CurNumConvertComplete == m_MaxNumConvertComplete)
	{
		m_ConvertOngoing = false;

		m_vecSrcFilePaths.clear();

		Text = m_ConvertLog->AddWidget<CIMGUIText>("OK");
		Text->SetText("Complete!");
		MessageBox(nullptr, TEXT("변환 완료"), TEXT("완료"), MB_OK);
	}
}
