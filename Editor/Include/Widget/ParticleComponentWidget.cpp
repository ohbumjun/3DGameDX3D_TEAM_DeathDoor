// IMGUI Widget
#include "IMGUITextInput.h"
#include "IMGUIInputFloat.h"
#include "IMGUIInputInt.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIImage.h"
#include "IMGUISliderFloat.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "IMGUIManager.h"
#include "IMGUIColor3.h"
#include "IMGUIColor4.h"
#include "IMGUIInputFloat.h"
#include "IMGUIComboBox.h"
#include "IMGUIDummy.h"
#include "IMGUITree.h"
// Components
#include "Component/StaticMeshComponent.h"
#include "Resource/Particle/Particle.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Render/RenderManager.h"
#include "Component/ParticleComponent.h"
#include "Component/SceneComponent.h"
#include "Resource/Particle/Particle.h"
// Editor Windows
#include "ParticleComponentWidget.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "../Object/3DCameraObject.h"
#include "../EditorManager.h"
#include "../EditorManager.h"
#include "../Window/ResourceDisplayWindow.h"


CParticleComponentWidget::CParticleComponentWidget()
{
}

CParticleComponentWidget::~CParticleComponentWidget()
{
}

bool CParticleComponentWidget::Init()
{
    CSceneComponentWidget::Init();

    // AddWidget
    m_ComponentTypeText->SetText("Particle Component");

    // �ֻ��� Ʈ��
    CIMGUITree* m_RootTree = AddWidget<CIMGUITree>("Particle Variables");

    m_LoadedParticleName = m_RootTree->AddWidget<CIMGUITextInput>("Loaded Particle(Drop)", 150.f, 30.f);
    m_LoadedParticleName->SetHintText("Particle Name");
    m_LoadedParticleName->SetDropCallBack<CParticleComponentWidget>(this, &CParticleComponentWidget::OnDropParticleToParticleWidget);
    
    m_LoadedParticleFileName = m_RootTree->AddWidget<CIMGUITextInput>("Particle File Name", 150.f, 30.f);
    m_LoadedParticleFileName->SetHintText("FileName");

    m_LoadParticleButton = m_RootTree->AddWidget<CIMGUIButton>("Load Particle", 150.f, 20.f);
    m_LoadParticleButton->SetClickCallback<CParticleComponentWidget>(this, &CParticleComponentWidget::OnLoadParticleClass);

    m_SaveParticleButton = m_RootTree->AddWidget<CIMGUIButton>("Save Particle", 150.f, 20.f);
    m_SaveParticleButton->SetClickCallback<CParticleComponentWidget>(this, &CParticleComponentWidget::OnSaveParticleClass);

    // OnResetParticleInfo
    m_ResetParticleButton = m_RootTree->AddWidget<CIMGUIButton>("Reset Particle", 150.f, 20.f);
    m_ResetParticleButton->SetClickCallback<CParticleComponentWidget>(this, &CParticleComponentWidget::OnResetParticleInfo);
   

    // BillBoard Effect
    m_BillBoardEnableEdit = m_RootTree->AddWidget<CIMGUICheckBox>("BillBoard", 80.f);
    m_BillBoardEnableEdit->AddCheckInfo("BillBoard");
    m_BillBoardEnableEdit->SetCallBackLabel<CParticleComponentWidget>(this, &CParticleComponentWidget::OnSetBillBoardEffectEdit);
    
    // Color Setting
    m_BaseColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("BaseColor", 200.f);
    m_BaseColorEdit->SetCallBack(this, &CParticleComponentWidget::OnEditBaseColor);
    
    m_ApplyLinearEmissiveChangeEdit = m_RootTree->AddWidget<CIMGUICheckBox>("Emissive Linear", 100.f);
    m_ApplyLinearEmissiveChangeEdit->AddCheckInfo("Emissive Linear");
    m_ApplyLinearEmissiveChangeEdit->SetCallBackLabel<CParticleComponentWidget>(this, &CParticleComponentWidget::OnEditEmissiveLinearCheck);
    
    m_StartEmissiveColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("StartEmissive", 200.f);
    m_StartEmissiveColorEdit->SetCallBack(this, &CParticleComponentWidget::OnEditStartEmissiveColor);
    
    m_EndEmissiveColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("EndEmissive", 200.f);
    m_EndEmissiveColorEdit->SetCallBack(this, &CParticleComponentWidget::OnEditEndEmissiveColor);

	return true;
}

void CParticleComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CParticleComponent* MeshCom = (CParticleComponent*)m_Component;

    // ó������ Particle �� ���õǾ� ���� �����Ƿ�, Diable ó���Ͽ� Update X
    // m_Component->Enable(false);
    // m_Component->GetGameObject()->Enable(false);

    // Material , Particle ����
    // OnSetParticleMaterialSetting(m_Component);

    CParticle* ParticleClass = dynamic_cast<CParticleComponent*>(MeshCom)->GetParticle();

    if (ParticleClass)
        ParticleLoadSuccessCallback(ParticleClass);
}

void CParticleComponentWidget::OnEditBaseColor(const Vector3& Color)
{
    if (!m_ParticleClass || !m_ParticleClass->GetMaterial())
        return;

    m_ParticleClass->GetMaterial()->SetBaseColor(Color.x, Color.y, Color.z, 1.f);
    dynamic_cast<CParticleComponent*>(m_Component)->GetMaterial()->SetBaseColor(Color.x, Color.y, Color.z, 1.f);
}

void CParticleComponentWidget::OnEditStartEmissiveColor(const Vector3& Color)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetStartEmissiveColor(Color);
    dynamic_cast<CParticleComponent*>(m_Component)->GetCBuffer()->SetStartEmissiveColor(Color);
}
void CParticleComponentWidget::OnEditEndEmissiveColor(const Vector3& Color)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetEndEmissiveColor(Color);
    dynamic_cast<CParticleComponent*>(m_Component)->GetCBuffer()->SetEndEmissiveColor(Color);
}
void CParticleComponentWidget::OnEditEmissiveLinearCheck(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLinearEmissiveChangeEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_Component)->GetCBuffer()->SetLinearEmissiveChangeEnable(Enable);
}

void CParticleComponentWidget::OnLoadParticleClass()
{
    // Bin//ParticleClass �����κ��� Load �ǰ� �����ϱ�
    const PathInfo* ParticleClassPathInfo = CPathManager::GetInst()->FindPath(PARTICLE_PATH);
    CEngineUtil::CheckAndMakeDirectory(ParticleClassPathInfo);

    TCHAR FileFullPath[MAX_PATH] = {};

    OPENFILENAME OpenFile = {};
    OpenFile.lStructSize = sizeof(OPENFILENAME);
    OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
    OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Particle File\0*.prtc");
    OpenFile.lpstrFile = FileFullPath;
    OpenFile.nMaxFile = MAX_PATH;
    OpenFile.lpstrInitialDir = ParticleClassPathInfo->Path;

    if (GetOpenFileName(&OpenFile) != 0)
    {
        // ��� ����
        std::string FileExt;
        std::string FileName;

        //const char* FilePathMultibyte = CEditorUtil::ChangeTCHARTextToMultibyte(FileFullPath);
        char FilePathMultibyteCopy[MAX_PATH] = {};

        int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
        WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyteCopy, ConvertLength, 0, 0);

        //strcpy_s(FilePathMultibyteCopy, FilePathMultibyte);

        // ���� Load�ϴ� Directory��  Bin//ParticleClass ���� Ȯ���ϱ� => �ƴ϶��, Load
        std::string PathInfoBeforeFileName;
        CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyteCopy, PathInfoBeforeFileName);

        //if (strcmp(ParticleClassPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
        //{
        //    MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle �� ��� Bin/ParticleClass �κ��� Load �ؾ� �Ѵ�."), NULL, MB_OK);
        //    return;
        //}

        CEditorUtil::ExtractFileNameAndExtFromPath(FilePathMultibyteCopy, FileName, FileExt);

        // Ȯ���� �˻�
        std::transform(FileExt.begin(), FileExt.end(), FileExt.begin(), [](char c) {return std::toupper(c); });

        if (FileExt != ".PRTC")
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Ȯ���ڰ� .prtc �̾�� �մϴ�."), NULL, MB_OK);
            return;
        }

        // SAFE_DELETE(m_ParticleClass); --> Particle Manager ���� �˾Ƽ� �������� ���̴�.
        CParticle* LoadedParticle = CResourceManager::GetInst()->CreateParticleEmpty<CParticle>();

        bool LoadResult = LoadedParticle->LoadFile(FilePathMultibyteCopy);

        if (!LoadResult)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Class Load Failure While Loading"), NULL, MB_OK);
            return;
        }

        ParticleLoadSuccessCallback(LoadedParticle);
    }

}
void CParticleComponentWidget::OnSaveParticleClass()
{
    if (!m_ParticleClass)
        return;

    // Bin//ParticleClass �����κ��� Load �ǰ� �����ϱ�
    const PathInfo* ParticleClassPathInfo = CPathManager::GetInst()->FindPath(PARTICLE_PATH);
    CEngineUtil::CheckAndMakeDirectory(ParticleClassPathInfo);

    TCHAR FileFullPath[MAX_PATH] = {};

    OPENFILENAME OpenFile = {};
    OpenFile.lStructSize = sizeof(OPENFILENAME);
    OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
    OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Particle File\0*.prtc");
    OpenFile.lpstrFile = FileFullPath;
    OpenFile.nMaxFile = MAX_PATH;
    OpenFile.lpstrInitialDir = ParticleClassPathInfo->Path;

    if (GetSaveFileName(&OpenFile) != 0)
    {
        // ��� ����
        const char* FilePathMultibyte = CEditorUtil::ChangeTCHARTextToMultibyte(FileFullPath);
        char FilePathMultibyteCopy[MAX_PATH] = {};
        strcpy_s(FilePathMultibyteCopy, FilePathMultibyte);

        // .prtc Ȯ���� Ȯ���ϱ� 
        char FileFullPathMultibyte[MAX_PATH];
        char FileExt[MAX_PATH];

        _splitpath_s(FilePathMultibyteCopy, nullptr, 0, nullptr, 0, nullptr, 0, FileExt, _MAX_EXT);

        _strupr_s(FileExt);

        // Ȯ���� .anim �� �ƴ϶�� return;
        if (strcmp(FileExt, ".PRTC") != 0)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .prtc"), NULL, MB_OK);
            return;
        }

        // �ش� Particle File �����ϱ� 
        m_ParticleClass->SaveFile(FilePathMultibyteCopy);

        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("���� ����"), TEXT("���� ����"), NULL);
    }
}
void CParticleComponentWidget::OnDropParticleToParticleWidget(const std::string& InputName)
{
    // Key �� ���·� Resource Manager ���� Particle�� ���� ã�´�.
    CParticle* FoundParticle = CResourceManager::GetInst()->FindParticle(InputName);

    if (FoundParticle)
    {
        ParticleLoadSuccessCallback(FoundParticle);
        return;
    }

    // �׷��� ���ٸ�, �ϵ��ũ���� ã�´�.
    std::string ParticleKeyName;

    // Bin//ParticleClass �� ����
    std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(PARTICLE_PATH,
        InputName, ParticleKeyName);

    // ã�� ���ߴٸ� 
    if (!FoundResult.has_value())
    {
        // New Texture Load Failure Message Box
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Class Load Failure From Bin/ParticleClass"), NULL, MB_OK);

        // m_LoadedParticleName Text Input ������ �ǵ��� ���´�
        m_LoadedParticleName->ResetText();

        return;
    }

    // ������ Ȯ���ڰ� .prtc ���� Ȯ���Ѵ�.
    const std::string& FullPathMultibyte = FoundResult.value();
    std::string ExtractedFilName;
    std::string ExtractedExt;
    CEditorUtil::ExtractFileNameAndExtFromPath(FullPathMultibyte, ExtractedFilName, ExtractedExt);

    // �빮�ڷ� ��ȯ ����, Ext Ȯ��
    std::transform(ExtractedExt.begin(), ExtractedExt.end(), ExtractedExt.begin(), ::toupper);
    if (ExtractedExt != ".PRTC")
    {
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Ȯ���ڰ� .prtc �̾�� �մϴ�."), NULL, MB_OK);

        // m_LoadedParticleName Text Input ������ �ǵ��� ���´�
        m_LoadedParticleName->ResetText();

        return;
    }

    // SAFE_DELETE(m_ParticleClass); --> Particle Manager ���� �˾Ƽ� �������� ���̴�.
    FoundParticle = CResourceManager::GetInst()->CreateParticleEmpty<CParticle>();
    bool LoadResult = FoundParticle->LoadFile(FoundResult.value().c_str());

    if (!LoadResult)
    {
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Class Load Failure From Bin/ParticleClass"), NULL, MB_OK);

        // m_LoadedParticleName Text Input ������ �ǵ��� ���´�
        m_LoadedParticleName->ResetText();

        return;
    }

    ParticleLoadSuccessCallback(FoundParticle);
}
void CParticleComponentWidget::OnResetParticleInfo()
{
    if (!m_Component)
        return;

    // ���� Component �� ���� Object 
    // �ش� Object �� ������ �ִ� ParticleComponent ��ϵ��� �̾ƿ���
    // �� �༮�鿡 ���ؼ� ResetParticleStructuredBufferInfo �Լ��� ȣ�����ش�
    const std::list<CSceneComponent*>& ObjectSceneComponents = m_Component->GetGameObject()->GetSceneComponents();

    auto iter = ObjectSceneComponents.begin();
    auto iterEnd = ObjectSceneComponents.end();
    
    for (; iter != iterEnd; ++iter)
    {
        // ParticleComponent �鿡 ���ؼ��� �Ʒ� �Լ��� ȣ���Ѵ�.
        CParticleComponent* CurParticleComponent = dynamic_cast<CParticleComponent*>((*iter));
    
        if (!CurParticleComponent)
            continue;
    
        // CurParticleComponent->RecreateOnlyOnceCreatedParticle();
        CurParticleComponent->RecreateOnlyOnceCreatedParticleWithOutLifeTimeSetting();
    }

   // dynamic_cast<CParticleComponent*>(m_Component)->ResetParticleStructuredBufferInfo();
}
void CParticleComponentWidget::OnSetBillBoardEffectEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    CParticleComponent* MeshCom = (CParticleComponent*)m_Component;

    MeshCom->SetBillBoardEffect(Enable);
}
void CParticleComponentWidget::ParticleLoadSuccessCallback(CParticle* LoadedParticle)
{
    if (!LoadedParticle)
        return;

    m_ParticleClass = LoadedParticle;

    SetParticleClassToParticleComponent(LoadedParticle);

    // Particle Manager �� Map �� �߰��ϱ�
    CResourceManager::GetInst()->GetParticleManager()->AddParticle(m_ParticleClass);

    // IMGUI �� �ݿ��ϱ� 
    ReflectParticleToIMGUI();

    // Resource Display Window �����ϱ�
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedParticleResources();
}
void CParticleComponentWidget::SetParticleClassToParticleComponent(CParticle* Particle)
{
    if (!Particle)
        return;

    dynamic_cast<CParticleComponent*>(m_Component)->SetParticle(Particle);

    // Name ����
    m_LoadedParticleName->ClearText();
    m_LoadedParticleName->SetText(Particle->GetName().c_str());

    // FileName �����ϱ� 
    std::string LoadedParticleName = dynamic_cast<CParticleComponent*>(m_Component)->GetParticleName();
    
    // .prtc �� ���̱�
    if (LoadedParticleName.find(".prtc") == std::string::npos)
        LoadedParticleName.append(".prtc");

    m_LoadedParticleFileName->ClearText();
    m_LoadedParticleFileName->SetText(LoadedParticleName.c_str());

}
void CParticleComponentWidget::ReflectParticleToIMGUI()
{
    if (!m_Component || !m_ParticleClass || !m_ParticleClass->GetMaterial())
        return;

   CMaterial* ParticleMaterial = m_ParticleClass->GetMaterial();
   
   const Vector4& BaseColor = ParticleMaterial->GetBaseColor();
   m_BaseColorEdit->SetRGB(BaseColor.x, BaseColor.y, BaseColor.z);
   
   const Vector3& StEmissiveColor = m_ParticleClass->GetStartEmissiveColor();
   m_StartEmissiveColorEdit->SetRGB(StEmissiveColor.x, StEmissiveColor.y, StEmissiveColor.z);
   
   const Vector3& EdEmissiveColor = m_ParticleClass->GetEndEmissiveColor();
   m_EndEmissiveColorEdit->SetRGB(EdEmissiveColor.x, EdEmissiveColor.y, EdEmissiveColor.z);
   
   bool IsLinearEmissiveEnable = m_ParticleClass->IsLinearEmissiveChangeEnable();
   m_ApplyLinearEmissiveChangeEdit->SetCheck(0, IsLinearEmissiveEnable);

    CParticleComponent* MeshCom = (CParticleComponent*)m_Component;
    m_BillBoardEnableEdit->SetCheck(0, MeshCom->IsBillBoardEffectEnable());
}
