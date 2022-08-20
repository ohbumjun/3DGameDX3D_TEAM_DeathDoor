#include "FBXConvertWindow.h"
#include "../EditorUtil.h"
#include "Engine.h"
#include "PathManager.h"

CFBXConvertWindow::CFBXConvertWindow()	:
	m_ConvertThread(nullptr),
	m_SrcDirText(nullptr),
	m_SetSrcDirButton(nullptr),
	m_ConvertButton(nullptr),
	m_ProgressBar(nullptr),
	m_ConvertLog(nullptr),
	m_SrcDirFullPath{}
{
}

CFBXConvertWindow::~CFBXConvertWindow()
{
	SAFE_DELETE(m_ConvertThread);
}

bool CFBXConvertWindow::Init()
{
	// ��ȯ ������
	m_ConvertThread = new CFBXConvertThread;
	m_ConvertThread->Init();
	m_ConvertThread->Start();

	// Widget ����
	CIMGUIText* text = AddWidget<CIMGUIText>("Text");
	text->SetText("FBX Converter");

	AddWidget<CIMGUISeperator>("sep");

	m_SingleFileModeCheckBox = AddWidget<CIMGUICheckBox>("Single File", 0.f, 0.f);
	m_SingleFileModeCheckBox->AddCheckInfo("Single File");
	
	m_SpecificFileNameInput = AddWidget<CIMGUITextInput>("SpecifieFileName", 300.f);

	m_SrcDirText = AddWidget<CIMGUITextInput>("SrcDirText", 300.f);
	AddWidget<CIMGUISameLine>("line");
	m_SetSrcDirButton = AddWidget<CIMGUIButton>("Browse Source", 0.f, 0.f);

	m_ProgressBar = AddWidget<CIMGUIProgressBar>("", 300.f, 0.f);
	AddWidget<CIMGUISameLine>("line");
	m_ConvertButton = AddWidget<CIMGUIButton>("Convert", 0.f, 0.f);

	text = AddWidget<CIMGUIText>("Text");
	text->SetText("Log");
	m_ConvertLog = AddWidget<CIMGUIChild>("Log", 300.f, 500.f);

	// Widget Initial Value
	m_SpecificFileNameInput->SetHideName(true);
	m_SpecificFileNameInput->SetHintText("SpecifieFileName");

	m_SrcDirText->ReadOnly(true);
	m_SrcDirText->SetHideName(true);
	m_SrcDirText->SetHintText("Set FBX Folder Or File");

	m_ProgressBar->SetPercent(0.f);

	m_ConvertLog->EnableBorder(true);

	// CallBack
	m_SetSrcDirButton->SetClickCallback(this, &CFBXConvertWindow::OnClickSetSrcDirButton);
	m_ConvertButton->SetClickCallback(this, &CFBXConvertWindow::OnClickConvertButton);
	m_SingleFileModeCheckBox->SetCallBackIdx(this, &CFBXConvertWindow::OnSelectSingleFileMode);

	m_ConvertThread->SetLoadingCallBack(this, &CFBXConvertWindow::OnLoading);

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
			int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
			WideCharToMultiByte(CP_ACP, 0, FilePath, -1, m_SrcFileFullPath, Length, 0, 0);

			m_SrcDirText->SetText(m_SrcFileFullPath);
		}
	}
	else
	{
		TCHAR Buf[MAX_PATH] = {};

		BROWSEINFO Bi = {};

		Bi.hwndOwner = CEngine::GetInst()->GetWindowHandle();
		Bi.lpszTitle = TEXT("��ȯ�� FBX���ϵ��� �ִ� ���� ����");
		Bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
		Bi.lpfn = BrowseCallbackProc;
	 //	Bi.lParam = (LPARAM)InitialPath;

		LPITEMIDLIST ItemIDList = ::SHBrowseForFolder(&Bi);

		if (::SHGetPathFromIDList(ItemIDList, Buf))
		{
			int length = WideCharToMultiByte(CP_ACP, 0, Buf, -1, nullptr, 0, nullptr, 0);
			WideCharToMultiByte(CP_ACP, 0, Buf, -1, m_SrcDirFullPath, length, nullptr, 0);
			strcat_s(m_SrcDirFullPath, "\\");

			m_SrcDirText->SetText(m_SrcDirFullPath);
		}
	}
}

void CFBXConvertWindow::OnClickConvertButton()
{
	// ����ó��
	if (m_SingleFileMode)
	{
		if (strlen(m_SrcFileFullPath) == 0)
		{
			return;
		}
	}
	else
	{
		if (strlen(m_SrcDirFullPath) == 0)
		{
			return;
		}
	}

	// �α�â Ŭ����
	m_ConvertLog->ClearWidget();

	CIMGUIText* StartText = m_ConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");

	// ���� �ϳ��� ��ȯ�ϴ� ���
	if (m_SingleFileMode)
	{
		m_ConvertThread->AddWork(m_SrcFileFullPath);
	}
	else
	{
		// ���� ���� ��� FBX ������ ��θ� �޾ƿ�
		std::vector<std::string> VecFullPath;
		CEditorUtil::GetAllFileFullPathInDir(m_SrcDirFullPath, VecFullPath, ".fbx");

		// ���� Ư�� �̸��� ����. �鸸 Convert �ϰ� �ʹٸ�, �ش� ����� �̾ƿ´�.
		if (!m_SpecificFileNameInput->Empty())
		{
			std::vector<std::string> SpecificVecFullPath;
			CEditorUtil::FilterSpecificNameIncludedFilePath(VecFullPath, SpecificVecFullPath, m_SpecificFileNameInput->GetTextUTF8());

			VecFullPath = SpecificVecFullPath;
		}

		size_t Size = VecFullPath.size();

		for (size_t i = 0; i < Size; ++i)
		{
			// �����忡 ���� ��û
			m_ConvertThread->AddWork(VecFullPath[i]);
		}
	}
}

void CFBXConvertWindow::OnLoading(const LoadingMessage& msg)
{
	m_ProgressBar->SetPercent(msg.Percent);
	CIMGUIText* Text = m_ConvertLog->AddWidget<CIMGUIText>("Text");
	std::string LoadingMsg = "Convert Success : " + CEditorUtil::FilterFileName(msg.Message);
	Text->SetText(LoadingMsg.c_str());

	if (msg.Complete)
	{
		Text  = m_ConvertLog->AddWidget<CIMGUIText>("OK");
		Text->SetText("Complete!");
		MessageBox(nullptr, TEXT("��ȯ �Ϸ�"), TEXT("�Ϸ�"), MB_OK);
	}
}

void CFBXConvertWindow::OnLoadFail(const std::string& failedPathName)
{
	m_ProgressBar->SetPercent(0.f);

	std::string ErrorText = failedPathName + " �� �ε��ϴµ� ����";

	char Buf[256] = {};
	strcpy_s(Buf, ErrorText.c_str());
	
	TCHAR BufW[MAX_PATH] = {};
	int Length = MultiByteToWideChar(CP_ACP, 0, Buf, -1, nullptr, 0);
	MultiByteToWideChar(CP_ACP, 0, Buf, -1, BufW, Length);

	MessageBox(nullptr, TEXT("����"), BufW, MB_OK);

	CIMGUIText* Text  = m_ConvertLog->AddWidget<CIMGUIText>("OK");
	std::string LogMsg = "Failed To Load " + failedPathName;
	Text->SetText(LogMsg.c_str());
}
