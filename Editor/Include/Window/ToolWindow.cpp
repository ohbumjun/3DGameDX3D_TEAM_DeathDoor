#include "ToolWindow.h"
#include "IMGUISetRect.h"
#include "Input.h"
#include "../EditorManager.h"
#include "../Object/3DCameraObject.h"
#include "Render/RenderManager.h"
#include "Engine.h"
#include "IMGUISameLine.h"
#include "IMGUIRadioButton.h"
#include "IMGUIInputFloat2.h"
#include "Scene/SceneManager.h"
#include "../Window/InspectorWindow.h"
#include "../Window/ObjectComponentWindow.h"
#include "../Window/SceneComponentHierarchyWindow.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "IMGUIManager.h"
#include "../EditorInfo.h"
#include "EngineUtil.h"
#include "PathManager.h"
#include "../EditorUtil.h"
#include "GameObject/SkyObject.h"
#include "IMGUITextInput.h"
#include "../DeathDoor/Component/GameStateComponent.h"

CToolWindow::CToolWindow()	:
	m_GizmoBlock(nullptr),
	m_Gizmo(nullptr),
	m_Grid(nullptr),
	m_GizmoOperationMode(nullptr),
	m_GizmoTransformMode(nullptr)
{
}

CToolWindow::~CToolWindow()
{
}

bool CToolWindow::Init()
{
	CIMGUIWindow::Init();

	// Play Stop
	CIMGUICollapsingHeader* PlayStopBlock = AddWidget<CIMGUICollapsingHeader>("Play", 200.f);
	m_PlayState = PlayStopBlock->AddWidget<CIMGUIText>("Play", 0.f, 0.f);
	m_Play = PlayStopBlock->AddWidget<CIMGUIButton>("Play", 0.f, 0.f);
	PlayStopBlock->AddWidget<CIMGUISameLine>("line");
	m_Pause = PlayStopBlock->AddWidget<CIMGUIButton>("Pause", 0.f, 0.f);
	PlayStopBlock->AddWidget<CIMGUISameLine>("line");
	m_Stop = PlayStopBlock->AddWidget<CIMGUIButton>("Stop", 0.f, 0.f);

	m_GizmoBlock = AddWidget<CIMGUICollapsingHeader>("Gizmo", 200.f);
	//m_Grid = AddWidget<CIMGUIGrid>("Grid", 100.f);
	AddWidget<CIMGUISetRect>("GridRect"); // Gizmo가 그려지기 위한 배경
	m_Gizmo = AddWidget<CIMGUIGizmo>("Gimzo");
	m_GizmoTransformMode = m_GizmoBlock->AddWidget<CIMGUIRadioButton>("Transform");
	m_GizmoOperationMode = m_GizmoBlock->AddWidget<CIMGUIRadioButton>("Operation");

	m_GizmoTransformMode->SetColNum(2);
	m_GizmoOperationMode->SetColNum(3);

	m_GizmoTransformMode->AddCheckInfo("Local");
	m_GizmoTransformMode->AddCheckInfo("World");
	m_GizmoTransformMode->SetCheck(1, true);
	m_GizmoTransformMode->AlwaysCheck(true);

	m_GizmoOperationMode->AddCheckInfo("Translation");
	m_GizmoOperationMode->AddCheckInfo("Rotation");
	m_GizmoOperationMode->AddCheckInfo("Scale");
	m_GizmoOperationMode->SetCheck(0, true);
	m_GizmoOperationMode->AlwaysCheck(true);

	// Camera
	m_EditorCameraBlock = AddWidget<CIMGUICollapsingHeader>("Editor Camera", 200.f);
	m_CameraSpeed = m_EditorCameraBlock->AddWidget<CIMGUISliderFloat>("Speed");
	m_SetCam = m_EditorCameraBlock->AddWidget<CIMGUIButton>("Set FreeCam", 0.f, 0.f);

	// Render
	m_RenderBlock = AddWidget<CIMGUICollapsingHeader>("Render", 200.f);
	m_ClearColor = m_RenderBlock->AddWidget<CIMGUIColor3>("Clear Color", 200.f);
	m_RenderSkyBox = m_RenderBlock->AddWidget<CIMGUICheckBox>("Render SkyBox");
	m_DebugRender = m_RenderBlock->AddWidget<CIMGUICheckBox>("DebugRender");
	m_PostProcessing = m_RenderBlock->AddWidget<CIMGUICheckBox>("PostProcessing(HDR)");
	m_ShadowBias = m_RenderBlock->AddWidget<CIMGUISliderFloat>("Shadow Bias");
	CIMGUITree* Tree = m_RenderBlock->AddWidget<CIMGUITree>("HDR Value", 200.f);
	m_AdaptationTime = Tree->AddWidget<CIMGUISliderFloat>("Adaptation Time");
	m_MiddleGray = Tree->AddWidget<CIMGUISliderFloat>("MiddleGray");
	m_LumWhite = Tree->AddWidget<CIMGUISliderFloat>("LumWhite");
	m_BloomThreshold = Tree->AddWidget<CIMGUISliderFloat>("Bloom Threshold");
	m_BloomScale = Tree->AddWidget<CIMGUISliderFloat>("Bloom Scale");
	m_DOFMin = Tree->AddWidget<CIMGUISliderFloat>("DOF Min");
	m_DOFMax = Tree->AddWidget<CIMGUISliderFloat>("DOF Max");
	Tree = Tree->AddWidget<CIMGUITree>("Fog Value");
	m_FogType = Tree->AddWidget<CIMGUIComboBox>("Fog Type");
	m_FogColor = Tree->AddWidget<CIMGUIColor3>("Fog Color");
	m_FogStart = Tree->AddWidget<CIMGUISliderFloat>("Fog Start");
	m_FogEnd = Tree->AddWidget<CIMGUISliderFloat>("Fog End");
	m_FogDensity = Tree->AddWidget<CIMGUISliderFloat>("Fog Density");

	// Global Light
	m_GLightBlock = AddWidget<CIMGUICollapsingHeader>("Global Light", 200.f);
	m_GLightRotX = m_GLightBlock->AddWidget<CIMGUISliderFloat>("RotX", 200.f);
	m_GLightRotY = m_GLightBlock->AddWidget<CIMGUISliderFloat>("RotY", 200.f);
	m_GLightRotZ = m_GLightBlock->AddWidget<CIMGUISliderFloat>("RotZ", 200.f);
	m_GLightColor = m_GLightBlock->AddWidget<CIMGUIColor3>("Color", 200.f);
	m_GLightAmbIntensity = m_GLightBlock->AddWidget<CIMGUISliderFloat>("Ambient Intensity", 200.f);

	// Global
	m_GlobalBlock = AddWidget<CIMGUICollapsingHeader>("Global", 200.f);
	m_SkyBoxTexPath = m_GlobalBlock->AddWidget<CIMGUITextInput>("SkyBoxTextureFullPath");
	m_GlobalBlock->AddWidget<CIMGUISameLine>("Line");
	m_LoadSkyBoxTex = m_GlobalBlock->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);

	// Initial Value
	bool Play = CSceneManager::GetInst()->GetScene()->IsPlay();

	if (Play)
	{
		m_PlayState->SetText("Current State : Playing");
	}
	else
	{
		m_PlayState->SetText("Current State : Paused");
	}

	m_CameraSpeed->SetMin(0.f);
	m_CameraSpeed->SetMax(10.f);
	m_CameraSpeed->SetValue(CEditorManager::GetInst()->Get3DCameraObject()->GetCameraSpeed());

	m_PostProcessing->AddCheckInfo("Enable PostProcessing(HDR)");
	bool IsPostProcessing = CRenderManager::GetInst()->IsPostProcessingEnable();
	m_PostProcessing->SetCheck(0, IsPostProcessing);

	float ShadowBias = CRenderManager::GetInst()->GetShadowBias();
	m_ShadowBias->SetMin(0.f);
	m_ShadowBias->SetMax(100.f);
	m_ShadowBias->SetValue(ShadowBias);

	float MiddleGray = CRenderManager::GetInst()->GetMiddleGray();
	m_MiddleGray->SetMin(0.1f);
	m_MiddleGray->SetMax(5.f);
	m_MiddleGray->SetValue(MiddleGray);

	float LumWhite = CRenderManager::GetInst()->GetLumWhite();
	m_LumWhite->SetMin(0.1f);
	m_LumWhite->SetMax(5.f);
	m_LumWhite->SetValue(LumWhite);

	float AdaptTime = CRenderManager::GetInst()->GetAdaptationTime();
	m_AdaptationTime->SetMin(1.f);
	m_AdaptationTime->SetMax(10.f);
	m_AdaptationTime->SetValue(AdaptTime);

	float BloomThreshold = CRenderManager::GetInst()->GetBloomThreshold();
	m_BloomThreshold->SetMin(0.1f);
	m_BloomThreshold->SetMax(3.f);
	m_BloomThreshold->SetValue(BloomThreshold);

	float BloomScale = CRenderManager::GetInst()->GetBloomScale();
	m_BloomScale->SetMin(0.5f);
	m_BloomScale->SetMax(10.f);
	m_BloomScale->SetValue(BloomScale);

	float CameraDist = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera()->GetDistance();
	float DOFMin = CRenderManager::GetInst()->GetDOFMin();
	m_DOFMin->SetMin(10.f);
	m_DOFMin->SetMax(CameraDist - 200.f);
	m_DOFMin->SetValue(DOFMin);

	float DOFMax = CRenderManager::GetInst()->GetDOFMax();
	m_DOFMax->SetMin(100.f);
	m_DOFMax->SetMax(CameraDist);
	m_DOFMax->SetValue(DOFMax);

	m_DebugRender->AddCheckInfo("Debug Render");
	bool IsDebugRender = CRenderManager::GetInst()->IsDebugRender();
	m_DebugRender->SetCheck(0, IsDebugRender);

	m_RenderSkyBox->AddCheckInfo("Render SkyBox");
	bool RenderSky = CRenderManager::GetInst()->IsRenderSkyBox();
	m_RenderSkyBox->SetCheck(0, RenderSky);

	std::string TypeName;
	for (int i = 0; i < (int)Fog_Type::Max; ++i)
	{
		TypeName = CEngineUtil::FogTypeToString((Fog_Type)i);
		m_FogType->AddItem(TypeName);
	}
	Fog_Type CurType = CRenderManager::GetInst()->GetFogType();
	m_FogType->SetSelectIndex((int)CurType);

	m_FogColor->SetRGB(CRenderManager::GetInst()->GetFogColor());

	Vector4 ClearColor = CEngine::GetInst()->GetClearColor();
	m_ClearColor->SetRGB(Vector3(ClearColor.x, ClearColor.y, ClearColor.z));

	switch (CurType)
	{
	case Fog_Type::Depth:
		m_FogStart->SetMin(0.f);
		m_FogStart->SetMax(1000.f);
		m_FogEnd->SetMin(0.f);
		m_FogEnd->SetMax(1000.f);
		break;
	case Fog_Type::Y:
		m_FogStart->SetMin(-1000.f);
		m_FogStart->SetMax(1000.f);
		m_FogEnd->SetMin(-1000.f);
		m_FogEnd->SetMax(1000.f);
		break;
	}
	float FogStart = CRenderManager::GetInst()->GetFogStart();
	m_FogStart->SetValue(FogStart);

	float FogEnd = CRenderManager::GetInst()->GetFogEnd();
	m_FogEnd->SetValue(FogEnd);

	float FogDensity = CRenderManager::GetInst()->GetFogDensity();
	m_FogDensity->SetMin(0.1f);
	m_FogDensity->SetMax(1.f);
	m_FogDensity->SetValue(FogDensity);

	CLightComponent* GLight = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLightComponent();
	Vector3 GLightRot = GLight->GetWorldRot();
	m_GLightRotX->SetValue(GLightRot.x);
	m_GLightRotY->SetValue(GLightRot.y);
	m_GLightRotZ->SetValue(GLightRot.z);
	m_GLightColor->SetRGB(GLight->GetLightColor().x, GLight->GetLightColor().y, GLight->GetLightColor().z);
	m_GLightRotX->SetMin(-180.f);
	m_GLightRotY->SetMin(-180.f);
	m_GLightRotZ->SetMin(-180.f);
	m_GLightRotX->SetMax(180.f);
	m_GLightRotY->SetMax(180.f);
	m_GLightRotZ->SetMax(180.f);

	float GLightAmbIntensity = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLightAmbiendIntensity();
	m_GLightAmbIntensity->SetValue(GLightAmbIntensity);
	m_GLightAmbIntensity->SetMin(0.f);
	m_GLightAmbIntensity->SetMax(1.f);

	m_SkyBoxTexPath->ReadOnly(true);

	// CallBack
	m_GizmoTransformMode->SetCallBack(this, &CToolWindow::OnSelectGizmoTransformMode);
	m_GizmoOperationMode->SetCallBack(this, &CToolWindow::OnSelectGizmoOperationMode);
	m_CameraSpeed->SetCallBack(this, &CToolWindow::OnChangeCameraSpeed);
	m_DebugRender->SetCallBackLabel(this, &CToolWindow::OnCheckDebugRender);
	m_ClearColor->SetCallBack(this, &CToolWindow::OnChangeClearColor);
	m_PostProcessing->SetCallBackLabel(this, &CToolWindow::OnCheckPostProcessing);
	m_Play->SetClickCallback(this, &CToolWindow::OnClickPlay);
	m_Pause->SetClickCallback(this, &CToolWindow::OnClickPause);
	m_Stop->SetClickCallback(this, &CToolWindow::OnClickStop);
	m_ShadowBias->SetCallBack(this, &CToolWindow::OnChangeShadowBias);
	m_AdaptationTime->SetCallBack(this, &CToolWindow::OnChangeAdaptationTime);
	m_MiddleGray->SetCallBack(this, &CToolWindow::OnChangeMiddleGray);
	m_LumWhite->SetCallBack(this, &CToolWindow::OnChangeLumWhite);
	m_BloomThreshold->SetCallBack(this, &CToolWindow::OnChangeBloomThreshold);
	m_BloomScale->SetCallBack(this, &CToolWindow::OnChangeBloomScale);
	m_DOFMin->SetCallBack(this, &CToolWindow::OnChangeDOFMin);
	m_DOFMax->SetCallBack(this, &CToolWindow::OnChangeDOFMax);
	m_FogType->SetSelectCallback(this, &CToolWindow::OnSelectFogType);
	m_FogColor->SetCallBack(this, &CToolWindow::OnChageFogColor);
	m_FogStart->SetCallBack(this, &CToolWindow::OnChangeFogStart);
	m_FogEnd->SetCallBack(this, &CToolWindow::OnChangeFogEnd);
	m_FogDensity->SetCallBack(this, &CToolWindow::OnChangeFogDensity);
	m_GLightRotX->SetCallBack(this, &CToolWindow::OnChangeGLightRotX);
	m_GLightRotY->SetCallBack(this, &CToolWindow::OnChangeGLightRotY);
	m_GLightRotZ->SetCallBack(this, &CToolWindow::OnChangeGLightRotZ);
	m_GLightColor->SetCallBack(this, &CToolWindow::OnChangeGLightColor);
	m_GLightAmbIntensity->SetCallBack(this, &CToolWindow::OnChangeGLightAmbIntensity);
	m_LoadSkyBoxTex->SetClickCallback(this, &CToolWindow::OnClickLoadSkyBoxTexture);
	m_RenderSkyBox->SetCallBackLabel(this, &CToolWindow::OnCheckRenderSkyBox);
	m_SetCam->SetClickCallback(this, &CToolWindow::OnClickSetEditorCam);

	// 디버그용 임시 키
	CInput::GetInst()->CreateKey("Z", 'Z');
	CInput::GetInst()->CreateKey("X", 'X');
	CInput::GetInst()->CreateKey("C", 'C');

	// 디버그용 임시 키
	CInput::GetInst()->SetKeyCallback("Z", KeyState_Down, this, &CToolWindow::OnQDown);
	CInput::GetInst()->SetKeyCallback("X", KeyState_Down, this, &CToolWindow::OnWDown);
	CInput::GetInst()->SetKeyCallback("C", KeyState_Down, this, &CToolWindow::OnEDown);

	return true;
}

void CToolWindow::SetPlayText(bool Play)
{
	if (Play)
	{
		m_PlayState->SetText("Current State : Playing");
	}
	else
	{
		m_PlayState->SetText("Current State : Paused");
	}
}

void CToolWindow::SetGizmoObject(CGameObject* Object)
{
	m_Gizmo->SetGameObject(Object);
}

void CToolWindow::SetGizmoComponent(CSceneComponent* SceneComp)
{
	m_Gizmo->SetComponent(SceneComp);
}

void CToolWindow::OnSelectGizmoOperationMode(const char* Label, bool Check)
{
	if (strcmp(Label, "Translation") == 0)
	{
		m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::TRANSLATE);
	}
	else if (strcmp(Label, "Rotation") == 0)
	{
		m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::ROTATE);
	}
	else if (strcmp(Label, "Scale") == 0)
	{
		m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::SCALE);
	}
}

void CToolWindow::OnSelectGizmoTransformMode(const char* Label, bool Check)
{
	if (strcmp(Label, "Local") == 0)
	{
		m_Gizmo->SetMode(ImGuizmo::MODE::LOCAL);
	}
	else if (strcmp(Label, "World") == 0)
	{
		m_Gizmo->SetMode(ImGuizmo::MODE::WORLD);
	}
}

void CToolWindow::OnChangeCameraSpeed(float Speed)
{
	CEditorManager::GetInst()->Get3DCameraObject()->SetCameraSpeed(Speed);
}

void CToolWindow::OnChangeShadowBias(float Bias)
{
	CRenderManager::GetInst()->SetShadowBias(Bias);
}

void CToolWindow::OnChangeLumWhite(float White)
{
	CRenderManager::GetInst()->SetLumWhite(White);
}

void CToolWindow::OnChangeMiddleGray(float Gray)
{
	CRenderManager::GetInst()->SetMiddleGray(Gray);
}

void CToolWindow::OnChangeAdaptationTime(float Time)
{
	CRenderManager::GetInst()->SetAdaptationTime(Time);
}

void CToolWindow::OnChangeBloomThreshold(float Threshold)
{
	CRenderManager::GetInst()->SetBloomThreshold(Threshold);
}

void CToolWindow::OnChangeBloomScale(float Scale)
{
	CRenderManager::GetInst()->SetBloomScale(Scale);
	
}

void CToolWindow::OnChangeDOFMin(float Min)
{
	CRenderManager::GetInst()->SetDOFMin(Min);
}

void CToolWindow::OnChangeDOFMax(float Max)
{
	CRenderManager::GetInst()->SetDOFMax(Max);
}

void CToolWindow::OnChangeClearColor(const Vector3& Color)
{
	CEngine::GetInst()->SetClearColor(Vector4(Color.x, Color.y, Color.z, 1.f));
}

void CToolWindow::OnSelectFogType(int Index, const char* Label)
{
	Fog_Type Type = (Fog_Type)Index;
	CRenderManager::GetInst()->SetFogType(Type);

	switch (Type)
	{
	case Fog_Type::Depth:
		m_FogStart->SetMin(0.f);
		m_FogStart->SetMax(1000.f);
		m_FogEnd->SetMin(0.f);
		m_FogEnd->SetMax(1000.f);
		break;
	case Fog_Type::Y:
		m_FogStart->SetMin(-1000.f);
		m_FogStart->SetMax(1000.f);
		m_FogEnd->SetMin(-1000.f);
		m_FogEnd->SetMax(1000.f);
		break;
	}
}

void CToolWindow::OnChageFogColor(const Vector3& Color)
{
	CRenderManager::GetInst()->SetFogColor(Color);
}

void CToolWindow::OnChangeFogStart(float Val)
{
	CRenderManager::GetInst()->SetFogStart(Val);
}

void CToolWindow::OnChangeFogEnd(float Val)
{
	CRenderManager::GetInst()->SetFogEnd(Val);
}

void CToolWindow::OnChangeFogDensity(float Val)
{
	CRenderManager::GetInst()->SetFogDensity(Val);
}

void CToolWindow::OnCheckDebugRender(const char* Label, bool Check)
{
	CRenderManager::GetInst()->SetDebugRender(Check);
}

void CToolWindow::OnCheckPostProcessing(const char* Label, bool Check)
{
	CRenderManager::GetInst()->EnablePostProcessing(Check);
}

void CToolWindow::OnCheckRenderSkyBox(const char* Label, bool Check)
{
	CRenderManager::GetInst()->SetRenderSkyBox(Check);
}

void CToolWindow::OnClickPlay()
{
	CScene* CurScene = CSceneManager::GetInst()->GetScene();

	CurScene->Play();

	m_PlayState->SetText("Current State : Playing");

	std::vector<CGameObject*> vecObject;
	CSceneManager::GetInst()->GetScene()->GetAllObjectsPointer(vecObject);

	size_t Count = vecObject.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CGameStateComponent* Comp = vecObject[i]->FindObjectComponentFromType<CGameStateComponent>();

		if (Comp)
		{
			Comp->SetTreeUpdate(true);
		}
	}

	CGameObject* Player = CurScene->GetPlayerObject();

	if (Player)
	{
		CCameraComponent* Cam = Player->FindComponentFromType<CCameraComponent>();

		if (Cam)
		{
			CurScene->GetCameraManager()->SetCurrentCamera(Cam);
		}
	}
}

void CToolWindow::OnClickPause()
{
	CSceneManager::GetInst()->GetScene()->Pause();

	m_PlayState->SetText("Current State : Paused");

	std::vector<CGameObject*> vecObject;
	CSceneManager::GetInst()->GetScene()->GetAllObjectsPointer(vecObject);

	size_t Count = vecObject.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CGameStateComponent* Comp = vecObject[i]->FindObjectComponentFromType<CGameStateComponent>();

		if (Comp)
		{
			Comp->SetTreeUpdate(false);
		}
	}
}

void CToolWindow::OnClickStop()
{
	bool Success = CSceneManager::GetInst()->ReloadScene();

	if (Success)
	{
		ClearSceneRelatedWindows();

		// Hierachy 갱신
		std::vector<CGameObject*> vecObj;
		CSceneManager::GetInst()->GetNextScene()->GetAllIncludeSaveObjectsPointer(vecObj);
		RefreshSceneRelatedWindow(vecObj);

		m_PlayState->SetText("Current State : Paused");

		// Scene Global Data 로드때 세팅으로 변경
		RefreshGlobalSceneDataWidget();

		CGameObject* EditorCamObj = CEditorManager::GetInst()->Get3DCameraObject();

		CCameraComponent* Cam = EditorCamObj->FindComponentFromType<CCameraComponent>();

		if (Cam)
		{
			CSceneManager::GetInst()->GetNextScene()->GetCameraManager()->SetCurrentCamera(Cam);
		}
	}
}

void CToolWindow::OnChangeGLightRotX(float Val)
{
	CGameObject* GLight = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLight();
	GLight->SetWorldRotationX(Val);
}

void CToolWindow::OnChangeGLightRotY(float Val)
{
	CGameObject* GLight = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLight();
	GLight->SetWorldRotationY(Val);
}

void CToolWindow::OnChangeGLightRotZ(float Val)
{
	CGameObject* GLight = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLight();
	GLight->SetWorldRotationZ(Val);
}

void CToolWindow::OnChangeGLightAmbIntensity(float Val)
{
	CSceneManager::GetInst()->GetScene()->GetLightManager()->SetGlogbalLightAmbientIntensity(Val);
}

void CToolWindow::OnChangeGLightColor(const Vector3& Color)
{
	CLightComponent* GLight = CSceneManager::GetInst()->GetScene()->GetLightManager()->GetGlobalLightComponent();

	Vector4 vCol = Vector4( Color.x, Color.y, Color.z, 0.f );
	GLight->SetColor(vCol);
}

void CToolWindow::OnClickLoadSkyBoxTexture()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("모든 파일\0*.*\0");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);
	
		CSkyObject* SkyObj = (CSkyObject*)CSceneManager::GetInst()->GetScene()->GetSkyObject();
		
		bool Success = SkyObj->SetSkyTextureFullPath(FilePathMultibyte);

		if (!Success)
		{
			MessageBox(nullptr, TEXT("SkyBox 교체 실패"), TEXT("Error"), MB_OK);
			return;
		}

		m_SkyBoxTexPath->SetText(FilePathMultibyte);

		MessageBox(nullptr, TEXT("SkyBox 교체 성공"), TEXT("Success"), MB_OK);
	}
}

void CToolWindow::ClearSceneRelatedWindows()
{
	CObjectHierarchyWindow* ObjHierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	CSceneComponentHierarchyWindow* SceneCompHierachy = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

	if (ObjHierachy)
	{
		ObjHierachy->Clear();
	}
	if (SceneCompHierachy)
	{
		SceneCompHierachy->ClearExistingHierarchy();
	}
	if (ObjCompWindow)
	{
		ObjCompWindow->Clear();
	}
	if (Inspector)
	{
		Inspector->OnDeleteGameObject();
	}

	SetGizmoObject(nullptr);
}

void CToolWindow::RefreshSceneRelatedWindow(CGameObject* Object)
{
	// Window 갱신
	CObjectHierarchyWindow* ObjHierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	CSceneComponentHierarchyWindow* SceneCompHierachy = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

	if (ObjHierachy)
	{
		ObjHierachy->OnCreateObject(Object);
	}
	if (SceneCompHierachy)
	{
		SceneCompHierachy->OnLoadGameObject(Object);
	}
	if (ObjCompWindow)
	{
		ObjCompWindow->OnRefreshObjectComponentList(Object);
	}
	if (Inspector)
	{
		Inspector->OnSelectGameObject(Object);
	}

	SetGizmoObject(Object);
}

void CToolWindow::RefreshSceneRelatedWindow(const std::vector<CGameObject*>& vecObj)
{
	size_t Size = vecObj.size();

	for (size_t i = 0; i < Size; ++i)
	{
		RefreshSceneRelatedWindow(vecObj[i]);
	}
}

void CToolWindow::OnClickSetEditorCam()
{
	CCameraManager* CamMng = CSceneManager::GetInst()->GetScene()->GetCameraManager();

	C3DCameraObject* CamObj = CEditorManager::GetInst()->Get3DCameraObject();
	CCameraComponent* CamCom = CamObj->FindComponentFromType<CCameraComponent>();

	if (CamMng->GetCurrentCamera() == CamCom)
	{
		return;
	}

	CamMng->SetCurrentCamera(CamCom);
}

void CToolWindow::RefreshGlobalSceneDataWidget()
{
	// 현재 씬 Change 되지 않은 프레임이기 때문에 NextScene에서 데이터를 받아온다.
	CScene* LoadedScene = CSceneManager::GetInst()->GetNextScene();

	SceneSaveGlobalData GlobalData = LoadedScene->GetSceneSaveGlobalData();
	m_AdaptationTime->SetValue(GlobalData.HDRData.AdaptationTime);
	m_LumWhite->SetValue(GlobalData.HDRData.LumWhite);
	m_MiddleGray->SetValue(GlobalData.HDRData.MiddleGray);
	m_BloomThreshold->SetValue(GlobalData.HDRData.BloomTreshold);
	m_BloomScale->SetValue(GlobalData.HDRData.BloomScale);
	m_DOFMin->SetValue(GlobalData.HDRData.DOFMin);
	m_DOFMax->SetValue(GlobalData.HDRData.DOFMax);
	m_FogColor->SetRGB(GlobalData.HDRData.FogColor);
	m_FogType->SetSelectIndex((int)GlobalData.HDRData.FogType);
	m_FogStart->SetValue(GlobalData.HDRData.FogStart);
	m_FogEnd->SetValue(GlobalData.HDRData.FogEnd);
	m_FogDensity->SetValue(GlobalData.HDRData.FogDensity);
	m_GLightRotX->SetValue(GlobalData.GLightData.Rot.x);
	m_GLightRotY->SetValue(GlobalData.GLightData.Rot.y);
	m_GLightRotZ->SetValue(GlobalData.GLightData.Rot.z);
	m_GLightColor->SetRGB(GlobalData.GLightData.Color);
	m_GLightAmbIntensity->SetValue(GlobalData.GLightData.AmbientIntensity);
	m_RenderSkyBox->SetCheck(0, GlobalData.BackGroundData.RenderSkyBox);
	m_ClearColor->SetRGB(Vector3(GlobalData.BackGroundData.ClearColor.x, GlobalData.BackGroundData.ClearColor.y, GlobalData.BackGroundData.ClearColor.z));
}

void CToolWindow::OnQDown(float DetlaTime)
{
	m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::TRANSLATE);
}

void CToolWindow::OnWDown(float DetlaTime)
{
	m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::ROTATE);
}

void CToolWindow::OnEDown(float DetlaTime)
{
	m_Gizmo->SetOperationMode(ImGuizmo::OPERATION::SCALE);
}


