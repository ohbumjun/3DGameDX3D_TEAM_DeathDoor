#pragma once

#include "../Shader/GraphicShader.h"
#include "../Texture/Texture.h"
#include "../Shader/MaterialConstantBuffer.h"

struct RenderStateSaveLoadStruct
{
    bool StateEnable;
    int StateNameLength;
    char StateName[MAX_PATH];
};

struct TextureSaveLoadStruct
{
    int TextureNameLength;
    char TextureName[MAX_PATH];
    Sampler_Type SamplerType;
    int Register;
    int ShaderType;
    int SaveFullPathLength;
    char SaveFullPathInfo[MAX_PATH];
    CTexture* Texture;
};

struct MaterialSaveLoadStruct
{
    int ShaderNameLength;
    char ShaderName[MAX_PATH];
    Vector4 BaseColor;
    Vector4 AmbientColor;
    Vector4 SpecularColor;
    Vector4 EmissiveColor;
    float Opacity;
    bool Animation3D;
    bool SpecularTex;
    bool EmissiveTex;
    bool Bump;
    RenderStateSaveLoadStruct RenderStateSaveLoad[(int)RenderState_Type::Max];
    // int TextureCount;
    bool Metallic;
    ShaderParams ShaderParams;
    Vector2 UVScale;
};

struct MaterialTextureInfo
{
    std::string     Name;
    Sampler_Type    SamplerType;
    CSharedPtr<CTexture>    Texture;
    int             Register;
    int             ShaderType;

    // Animation Instance Load 과정 편의를 위해, Texture 들의 FullPath 정보들을 저장
    // - 실제 Load 과정에서는 해당 FullPath 로 부터, File 이름, File Ext 정보를 추출하여 활용할 것이다.
    std::string SavedFullPath;

    MaterialTextureInfo() :
        SamplerType(Sampler_Type::Linear),
        Register(0),
        ShaderType((int)Buffer_Shader_Type::Pixel)
    {
    }
};

struct RenderCallback
{
    std::function<void()>   Func;
    void* Obj;
};


class CMaterial :
    public CRef
{
    friend class CMaterialManager;
    friend class CMesh;
    friend class CStaticMeshComponent;
    friend class CAnimationMeshComponent;
    friend class CLandScape;

public:
    CMaterial();
    CMaterial(const CMaterial& Material);
    virtual ~CMaterial();

private:
    class CScene* m_Scene;

public:
    void SetScene(class CScene* Scene)
    {
        m_Scene = Scene;
    }

protected:
    CSharedPtr<CGraphicShader>  m_Shader;
    CSharedPtr<CGraphicShader>  m_PrevShader;           // Transparency 적용될 경우, 이전에 가지고 있던 Shader
    std::vector<MaterialTextureInfo>    m_TextureInfo;
    Vector4     m_BaseColor;
    Vector4		m_AmbientColor;
    Vector4		m_SpecularColor;
    Vector4     m_EmissiveColor;
    float       m_Opacity;
    bool        m_Animation3D;
    bool        m_SpecularTex;
    bool        m_EmissiveTex;
    bool        m_Bump;
    bool        m_RecieveDecal;
    bool        m_Metallic;
    Vector2     m_UVScale;
    ShaderParams m_ShaderParams;
    CMaterialConstantBuffer* m_CBuffer;
    CSharedPtr<class CRenderState>  m_RenderStateArray[(int)RenderState_Type::Max];
    std::list<RenderCallback*>    m_RenderCallback;

protected :
    std::vector<std::string> m_vecTextureFullPathInfos;
    
public:
    CGraphicShader* GetShader() const
    {
        return m_Shader;
    }

    CGraphicShader* GetPrevShader() const
    {
        return m_PrevShader;
    }

    CMaterialConstantBuffer* GetCBuffer()   const
    {
        return m_CBuffer;
    }

    const std::vector<MaterialTextureInfo>& GetTextureInfo() const
    {
        return m_TextureInfo;
    }

    const CSharedPtr<class CRenderState>* GetRenderStateArray() const
    {
        return m_RenderStateArray;
    }

private:
    void SetConstantBuffer(class CMaterialConstantBuffer* Buffer)
    {
        m_CBuffer = Buffer->Clone();
    }

    void CreateConstantBuffer();

public:
    CTexture* GetTexture(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture;
    }

    unsigned int GetTextureWidth(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture->GetWidth();
    }

    unsigned int GetTextureHeight(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture->GetHeight();
    }

    const Vector4& GetBaseColor() const
    {
        return m_BaseColor;
    }

    const Vector4& GetAmbientColor() const
    {
        return m_AmbientColor;
    }

    const Vector4& GetSpecularColor() const
    {
        return m_SpecularColor;
    }

    const Vector4& GetEmissiveColor() const
    {
        return m_EmissiveColor;
    }

    float GetSpecularPower() const
    {
        return m_SpecularColor.w;
    }

    bool IsTransparent() const
    {
        return m_RenderStateArray[(int)RenderState_Type::Blend];
    }

    float GetOpacity() const
    {
        return m_Opacity;
    }

    bool IsMetallic() const
    {
        return m_Metallic;
    }

    const Vector2& GetUVScale() const
    {
        return m_UVScale;
    }

    bool EmptyTexture() const
    {
        return m_TextureInfo.empty();
    }

    const ShaderParams& GetShaderParams() const
    {
        return m_ShaderParams;
    }

public :
    void SetShaderParams(const ShaderParams& Params);

public :
    void EnableBump();
    void EnableAnimation3D();
    void EnableSpecularTex();
    void EnableEmissiveTex();
    void EnableDecal(bool Decal);

public:
    void SetRenderState(class CRenderState* State);
    void SetRenderState(const std::string& Name);
    void SetTransparency(bool Enable);
    void SetOpacity(float Opacity);
    void AddOpacity(float Opacity);

public:
    void SetBaseColor(const Vector4& Color);
    void SetBaseColor(float r, float g, float b, float a);
    void SetAmbientColor(const Vector4& Color);
    void SetAmbientColor(float r, float g, float b, float a);
    void SetSpecularColor(const Vector4& Color);
    void SetSpecularColor(float r, float g, float b, float a);
    void SetEmissiveColor(const Vector4& Color);
    void SetEmissiveColor(float r, float g, float b, float a);
    void SetSpecularPower(float Power);
    void SetMetallic(bool Metallic);
    void SetUVScale(const Vector2& UVScale);

public:
    void AddTexture(int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void AddTexture(int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void AddTextureFullPath(int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void AddTexture(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);
    void AddTextureArrayFullPath(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
    void AddTextureArray(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);

    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void SetTextureFullPath(int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);
    void SetTextureArrayFullPath(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
    void SetTextureArray(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);

    void ClearTexture();

public :
    // Texture  정보만 세팅하는 코드
    void SetTextureInfoResource(int Index, class CTexture* Texture);
    // Register 정보만 세팅하는 코드
    void SetTextureInfoRegister(int Index, int Register);
public:
    void SetPaperBurn(bool Enable);
    bool CheckMaterial(CMaterial* Material);

public:
    void SetShader(const std::string& Name);
    void SetShader(CGraphicShader* Shader);
    void RevertShader();                    // PrevShader로 되돌림
    void Render();
    void RenderTexture();
    void UpdateCBuffer();
    void Reset();
    CMaterial* Clone()  const;
    bool Save(FILE* File);
    bool Load(FILE* File);
    bool SaveMaterial(FILE* File);
    bool LoadMaterial(FILE* File);

public:
    bool SaveFullPath(const char* FullPath);
    bool LoadFullPath(const char* FullPath);

private:
    void UpdateShaderParams();

public:
    template <typename T>
    void AddRenderCallback(T* Obj, void(T::* Func)())
    {
        RenderCallback* Callback = new RenderCallback;
        Callback->Obj = Obj;
        Callback->Func = std::bind(Func, Obj);
        m_RenderCallback.push_back(Callback);
    }

    void DeleteRenderCallback(void* Obj)
    {
        auto    iter = m_RenderCallback.begin();
        auto    iterEnd = m_RenderCallback.end();

        for (; iter != iterEnd; ++iter)
        {
            if ((*iter)->Obj == Obj)
            {
                SAFE_DELETE((*iter));
                m_RenderCallback.erase(iter);
                break;
            }
        }
    }
};

