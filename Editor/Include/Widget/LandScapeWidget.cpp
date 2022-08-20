#include "LandScapeWidget.h"
#include "IMGUISameLine.h"
#include "Component/LandScape.h"
#include "Engine.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CLandScapeWidget::CLandScapeWidget()
{
}

CLandScapeWidget::~CLandScapeWidget()
{
}

bool CLandScapeWidget::Init()
{
    CSceneComponentWidget::Init();

	// AddWidget
	m_ComponentTypeText->SetText("LandScape Component");

	// 최상위 트리
	CIMGUICollapsingHeader* Collapse = AddWidget<CIMGUICollapsingHeader>("LandScape Variables");

 	m_DebugRender = Collapse->AddWidget<CIMGUICheckBox>("DebugRender");
	m_TextureName = Collapse->AddWidget<CIMGUITextInput>("Landscape Texture");
	Collapse->AddWidget<CIMGUISameLine>("line");
	m_LoadTextureButton = Collapse->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);
	m_HeightMapFullPathText = Collapse->AddWidget<CIMGUITextInput>("HeightMap Texture");
	Collapse->AddWidget<CIMGUISameLine>("line");
	m_LoadHeightMapButton = Collapse->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);
	m_HeightMapImage = Collapse->AddWidget<CIMGUIImage>("View");
	m_CountInput = Collapse->AddWidget<CIMGUIInputInt2>("Map Vertex Count");
	m_CreateButton = Collapse->AddWidget<CIMGUIButton>("Create", 0.f, 0.f);
	m_YFactorInput = Collapse->AddWidget<CIMGUIInputFloat>("Y Factor");
	m_ChangeYFactorButton = Collapse->AddWidget<CIMGUIButton>("Change Y Factor", 0.f, 0.f);
	AddWidget<CIMGUISeperator>("Sep");

	// Initial Value
	m_DebugRender->AddCheckInfo("WireFrame");
	m_DebugRender->SetCheck(0, true);
	m_TextureName->ReadOnly(true);
	m_HeightMapFullPathText->ReadOnly(true);
	m_YFactorInput->SetVal(30.f);

	// CallBack
	m_DebugRender->SetCallBackIdx(this, &CLandScapeWidget::OnCheckDebugRender);
	m_LoadTextureButton->SetClickCallback(this, &CLandScapeWidget::OnClickLoadTexture);
	m_LoadHeightMapButton->SetClickCallback(this, &CLandScapeWidget::OnClickLoadHeightMap);
	m_CreateButton->SetClickCallback(this, &CLandScapeWidget::OnClickCreateLandScape);
	m_ChangeYFactorButton->SetClickCallback(this, &CLandScapeWidget::OnClickChangeYFactor);

	return true;
}

void CLandScapeWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);
}

void CLandScapeWidget::OnCheckDebugRender(int Index, bool Check)
{
	static_cast<CLandScape*>(m_Component)->SetDebugRender(Check);
}

void CLandScapeWidget::OnClickLoadTexture()
{
}

void CLandScapeWidget::OnClickLoadHeightMap()
{
	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("모든 파일\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		if (!CSceneManager::GetInst()->GetScene()->GetResource()->LoadTextureFullPath("LandScapeEditorHeightMap",
			FilePath))
		{
			MessageBox(nullptr, TEXT("텍스쳐 로드 실패"), TEXT("실패"), MB_OK);
			return;
		}

		MessageBox(nullptr, TEXT("텍스쳐 로드 성공"), TEXT("성공"), MB_OK);
		m_HeightMapImage->SetTexture(CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture("LandScapeEditorHeightMap"));
	}
}

void CLandScapeWidget::OnClickCreateLandScape()
{
	int CountX, CountY;
	CountX = m_CountInput->GetX();
	CountY = m_CountInput->GetY();

	if (CountX == 0 || CountY == 0)
	{
		MessageBox(nullptr, TEXT("Count 잘못됨"), TEXT("Error"), MB_OK);
		return;
	}

	CTexture* HeightTex = CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture("LandScapeEditorHeightMap");
	static_cast<CLandScape*>(m_Component)->CreateLandScape("LandScape", CountX, CountY, HeightTex, m_YFactorInput->GetVal());
}

void CLandScapeWidget::OnClickChangeYFactor()
{
	CLandScape* LandScape = static_cast<CLandScape*>(m_Component);

	if (!LandScape->IsCreate())
	{
		return;
	}

	if (m_YFactorInput->GetVal() < 0.f)
	{
		return;
	}

	LandScape->SetYFactor(m_YFactorInput->GetVal());
}
