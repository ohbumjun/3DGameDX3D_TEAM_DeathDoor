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
	class CIMGUITextInput* m_NewMaterialName; // 동시에 파일 이름으로 저장될 것이다.
	class CIMGUIButton* m_CreateMaterialBtn;

	// 현재 선택된 Material 이름
	class CIMGUITextInput* m_SelectedMaterialName;  

	// Material 이름 수정하기 
	class CIMGUITextInput* m_EditMaterialName; 
	class CIMGUIButton* m_EditMaterialNameBtn;

	// 해당 Material 의 정보를 보여주는 Info Table
	class CIMGUITable* m_MtrlInfoTable;

	// 현재 선택된 Texture 를 보여주는 Image 
	class CIMGUIImage* m_SetTexureImage;

	// Texture 목록을 보여주는 장소
	class CIMGUITableElemList* m_TextureInfoTable;

	// Particle 을 그려내기 위한 기본 세팅을 알아서 해주는 Btn
	class CIMGUIButton* m_SetParticleSettingBtn;

	// Shader 를 Drag, Drop 할 수 있는 장소
	class CIMGUITextInput* m_ShaderSetInput;
	
	// Sampler State 를 확인하는 table
	class CIMGUITable*       m_RenderStateInfoTable;
	class CIMGUITextInput* m_RenderStateSetInput;

	// Drop 해서 Set Texture 하기
	class CIMGUITextInput* m_AddTextureInput; // 동시에 파일 이름으로 저장될 것이다.
	class CIMGUIButton*     m_AddTextureBtn;

	// Drop 해서 Add Texture 하기 
	class CIMGUIInputInt*   m_TextureIndex; // 동시에 파일 이름으로 저장될 것이다.
	class CIMGUIInputInt*	m_TextureRegister;
	class CIMGUITextInput* m_SetTextureInput; // 동시에 파일 이름으로 저장될 것이다.
	class CIMGUIButton*     m_SetTextureBtn;

	// Drop 해서 Image 보여주는 Image Widget
	// 각종 Btn 들
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

	// Material 내용이 IMGUI 를 통해 보여지게 하기 
	void RefreshMaterialDisplayInfo(class CMaterial* Material, class CTexture* Texture = nullptr);
};

