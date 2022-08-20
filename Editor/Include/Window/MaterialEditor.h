#pragma once
#include "IMGUIWindow.h"

class CMaterialEditor :
    public CIMGUIWindow
{
	friend class CEditorManager;
	// friend class CAnimationInstanceConvertThread;
public:
	CMaterialEditor();
	~CMaterialEditor();

private :
	class CMaterial* m_SelectedMaterial;

private :
	class CIMGUITextInput* m_NewMaterialName; // ���ÿ� ���� �̸����� ����� ���̴�.
	class CIMGUIButton* m_CreateMaterialBtn;

	// ���� ���õ� Material �̸�
	class CIMGUITextInput* m_SelectedMaterialName;  

	// Material �̸� �����ϱ� 
	class CIMGUITextInput* m_EditMaterialName; 
	class CIMGUIButton* m_EditMaterialNameBtn;

	// �ش� Material �� ������ �����ִ� Info Table
	class CIMGUITable* m_MtrlInfoTable;

	// ���� ���õ� Texture �� �����ִ� Image 
	class CIMGUIImage* m_SetTexureImage;

	// Texture ����� �����ִ� ���
	class CIMGUITableElemList* m_TextureInfoTable;

	// Particle �� �׷����� ���� �⺻ ������ �˾Ƽ� ���ִ� Btn
	class CIMGUIButton* m_SetParticleSettingBtn;

	// Shader �� Drag, Drop �� �� �ִ� ���
	class CIMGUITextInput* m_ShaderSetInput;
	
	// Sampler State �� Ȯ���ϴ� table
	class CIMGUITable*       m_RenderStateInfoTable;
	class CIMGUITextInput* m_RenderStateSetInput;

	// Drop �ؼ� Set Texture �ϱ�
	class CIMGUITextInput* m_AddTextureInput; // ���ÿ� ���� �̸����� ����� ���̴�.
	class CIMGUIButton*     m_AddTextureBtn;

	// Drop �ؼ� Add Texture �ϱ� 
	class CIMGUIInputInt*   m_TextureIndex; // ���ÿ� ���� �̸����� ����� ���̴�.
	class CIMGUIInputInt*	m_TextureRegister;
	class CIMGUITextInput* m_SetTextureInput; // ���ÿ� ���� �̸����� ����� ���̴�.
	class CIMGUIButton*     m_SetTextureBtn;

	// Drop �ؼ� Image �����ִ� Image Widget
	// ���� Btn ��
	class CIMGUIButton* m_SaveMaterialBtn;
	class CIMGUIButton* m_LoadMaterialBtn;

public:
	virtual bool Init();

public :
	void SetSelectMaterial(class CMaterial* Material)
	{
		m_SelectedMaterial = Material;
	}

private :
	void OnCreateMaterialCallback();

	// Render State
	void OnDropAndCreateMaterialCallback(const std::string&);

	// Shader
	void OnDropAndSetShaderToMaterial(const std::string&);

	// Render State
	void OnDropAndSetRenderStateToMaterial(const std::string&);

	// Particle Material Setting
	void OnEditMaterialNameCallback();

	void OnSetParticleMaterialSettingCallback();

	void OnIsOutLineEdit(const char*, bool Enable);
	void OnSetOutLineColor(const Vector3& Color);
	void OnSetOutLineThickNess(float ThickNess);

	void OnSetTextureBtn();
	void OnSetTextureBtnWithString(const std::string&);
	void OnAddTextureBtn();
	void OnAddTextureBtnWithString(const std::string&);
	void TextureSetSuccessCallback(class CTexture* Texture);

	void OnSaveMaterial();
	void OnLoadMaterial();

	// Material ������ IMGUI �� ���� �������� �ϱ� 
	void RefreshMaterialDisplayInfo(class CMaterial* Material, class CTexture* Texture = nullptr);
};

