#pragma once
#include "IMGUIWindow.h"

class CResourceDisplayWindow :
    public CIMGUIWindow
{
	friend class CEditorManager;
	// friend class CAnimationInstanceConvertThread;
public:
	CResourceDisplayWindow();
	~CResourceDisplayWindow();
private :
	// ���� Project �� Resource Manager �� Load �� ��ϵ��� ���� Window
	// - ���⿡��, Drag �ؼ�, Material Window ���� ���� Drop �ϸ�, �ش� ��ġ�� ���� ������
	// - ���õǴ� ������ �ڵ带 �ۼ��� ���̴�.
	// Texture
	class CIMGUIWidgetList* m_TextureList;
	// Shader
	class CIMGUIWidgetList* m_ShaderList;
	// Sampler
	class CIMGUIWidgetList* m_RenderStateList;
	// Material
	class CIMGUIWidgetList* m_MaterialList;
	// Particle
	class CIMGUIWidgetList* m_ParticleList;
	// Mesh
	class CIMGUIWidgetList* m_MeshList;

private :
	std::vector<class CIMGUIWidgetList*> m_vecResourceTapList;

public :
	virtual bool Init() override;

public  :
	void RefreshLoadedTextureResources();
	void RefreshLoadedMaterialResources();
	void RefreshLoadedRenderStateResources();
	void RefreshLoadedShaderResources();
	void RefreshLoadedParticleResources();
	void RefreshLoadedMeshResources();
	void RefreshAllDisplayInfos();
};

