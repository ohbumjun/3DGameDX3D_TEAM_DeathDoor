// IMGUI
#include "IMGUIWidgetList.h"
#include "IMGUIText.h"
// Window
#include "ResourceDisplayWindow.h"
// Engine
#include "Resource/Texture/TextureManager.h"
#include "Resource/ResourceManager.h"
#include "Render/RenderManager.h"
#include "Render/RenderStateManager.h"
#include "Resource/Shader/ShaderManager.h"
#include "Resource/Particle/ParticleManager.h"
#include "Resource/Mesh/MeshManager.h"

CResourceDisplayWindow::CResourceDisplayWindow()
{
}

CResourceDisplayWindow::~CResourceDisplayWindow()
{
}

bool CResourceDisplayWindow::Init()
{
	// Texture List
	m_TextureList = AddWidget<CIMGUIWidgetList>("Texture", 100.f, 20.f);
	m_TextureList->SetApplyHideEffect(true);

	// -- Sample
	CIMGUIText* AddedText = m_TextureList->AddWidget<CIMGUIText>("Sample Text");
	AddedText->SetText("Hello");

	m_vecResourceTapList.push_back(m_TextureList);

	// Mesh List
	m_MeshList = AddWidget<CIMGUIWidgetList>("Mesh", 100.f, 20.f);
	m_MeshList->SetApplyHideEffect(true);
	m_vecResourceTapList.push_back(m_MeshList);

	// Material List
	m_MaterialList = AddWidget<CIMGUIWidgetList>("Material", 100.f, 20.f);
	m_MaterialList->SetApplyHideEffect(true);
	m_vecResourceTapList.push_back(m_MaterialList);

	// Shader List
	m_ShaderList = AddWidget<CIMGUIWidgetList>("Shader", 100.f, 20.f);
	m_ShaderList->SetApplyHideEffect(true);
	m_vecResourceTapList.push_back(m_ShaderList);

	// Sampler List
	m_RenderStateList = AddWidget<CIMGUIWidgetList>("RenderState", 100.f, 20.f);
	m_RenderStateList->SetApplyHideEffect(true);
	m_vecResourceTapList.push_back(m_RenderStateList);

	// Sampler List
	m_ParticleList = AddWidget<CIMGUIWidgetList>("Particle", 100.f, 20.f);
	m_ParticleList->SetApplyHideEffect(true);
	m_vecResourceTapList.push_back(m_ParticleList);

	RefreshLoadedTextureResources();
	RefreshLoadedMaterialResources();
	RefreshLoadedRenderStateResources();
	RefreshLoadedShaderResources();
	RefreshLoadedMeshResources();
	RefreshLoadedParticleResources();

	CResourceManager::GetInst()->AddTextureResourceChangeCallBack(this, &CResourceDisplayWindow::RefreshLoadedTextureResources);
	CResourceManager::GetInst()->AddMaterialResourceChangeCallBack(this, &CResourceDisplayWindow::RefreshLoadedMaterialResources);
	CResourceManager::GetInst()->AddShaderResourceChangeCallBack(this, &CResourceDisplayWindow::RefreshLoadedShaderResources);
	CResourceManager::GetInst()->AddParticleResourceChangeCallBack(this, &CResourceDisplayWindow::RefreshLoadedParticleResources);
	CResourceManager::GetInst()->AddMeshResourceChangeCallBack(this, &CResourceDisplayWindow::RefreshLoadedMeshResources);

	return true;
}

void CResourceDisplayWindow::RefreshLoadedTextureResources()
{
	m_TextureList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CTexture>>& MapTexture = CResourceManager::GetInst()->GetTextureManager()->GetMapTexture();

	auto iter = MapTexture.begin();
	auto iterEnd = MapTexture.end();

	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_TextureList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}

void CResourceDisplayWindow::RefreshLoadedMaterialResources()
{
	m_MaterialList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CMaterial>>& MapTexture = CResourceManager::GetInst()->GetMaterialManager()->GetMapMaterial();
	
	auto iter = MapTexture.begin();
	auto iterEnd = MapTexture.end();
	
	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_MaterialList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}



void CResourceDisplayWindow::RefreshLoadedShaderResources()
{
	m_ShaderList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CShader>>& MapShader = CResourceManager::GetInst()->GetShaderManager()->GetMapShader();

	auto iter = MapShader.begin();
	auto iterEnd = MapShader.end();

	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_ShaderList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}

void CResourceDisplayWindow::RefreshLoadedParticleResources()
{
	m_ParticleList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CParticle>>& MapParticle = CResourceManager::GetInst()->GetParticleManager()->GetMapParticle();
	
	auto iter = MapParticle.begin();
	auto iterEnd = MapParticle.end();
	
	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_ParticleList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}

void CResourceDisplayWindow::RefreshLoadedMeshResources()
{
	m_MeshList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CMesh>>& MapMesh = CResourceManager::GetInst()->GetMeshManager()->GetMapMesh();
	
	auto iter = MapMesh.begin();
	auto iterEnd = MapMesh.end();
	
	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_MeshList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}

void CResourceDisplayWindow::RefreshAllDisplayInfos()
{
}

void CResourceDisplayWindow::RefreshLoadedRenderStateResources()
{
	m_RenderStateList->ClearWidget();

	const std::unordered_map<std::string, CSharedPtr<class CRenderState>>& MapRenderState = CRenderManager::GetInst()->GetRenderStateManager()->GetMapRenderState();
	
	auto iter = MapRenderState.begin();
	auto iterEnd = MapRenderState.end();
	
	for (; iter != iterEnd; ++iter)
	{
		CIMGUIText* AddedText = m_RenderStateList->AddWidget<CIMGUIText>(iter->first);
		AddedText->SetText(iter->first.c_str());
	}
}
