#include "PathManager.h"

CPathManager* CPathManager::m_Inst = nullptr;

CPathManager::CPathManager()
{
}

CPathManager::~CPathManager()
{
    auto    iter = m_mapPath.begin();
    auto    iterEnd = m_mapPath.end();

    for (; iter != iterEnd; ++iter)
    {
        SAFE_DELETE(iter->second);
    }
}

bool CPathManager::Init()
{
    // RootPath의 경로를 얻어온다.
    // RootPath는 Bin폴더 까지의 전체 경로를 말한다.
    TCHAR   BinPath[MAX_PATH] = {};

    // GetModuleFileName : 현재 실행파일이 있는 경로 + 실행파일이름.exe 로
    // 전체경로를 얻어오게 된다.
    // 여기에서 실행파일 이름까지를 빼서 Bin폴더 까지만 얻어와서
    // Root폴더로 사용한다.
    GetModuleFileName(0, BinPath, MAX_PATH);

    // 얻어온 문자열을 뒤에서부터 차례로 접근하여 \나 / 가 있는 문자를 찾는다.
    int Length = lstrlen(BinPath);

    // D:\Lecture\37th\API\GameFramework\GameFramework\Bin\GameFramework_Debug.exe
    for (int i = Length - 1; i > 0; --i)
    {
        if (BinPath[i] == '\\')
        {
            memset(&BinPath[i + 1], 0, sizeof(TCHAR) * (Length - i - 1));
            break;
        }
    }

    PathInfo* Info = new PathInfo;

    lstrcpy(Info->Path, BinPath);

#ifdef UNICODE
    // 유니코드 문자열을 멀티바이트 문자열로 변환한다.
    int ConvertLength = WideCharToMultiByte(CP_ACP, 0, BinPath, -1, nullptr, 0, 0, 0);

    WideCharToMultiByte(CP_ACP, 0, BinPath, -1,
        Info->PathMultibyte, ConvertLength, 0, 0);
#else
    strcpy_s(Info->PathMultibyte, BinPath);
#endif // UNICODE

    m_mapPath.insert(std::make_pair(ROOT_PATH, Info));

    AddPath(SHADER_PATH, TEXT("Shader\\"));
    AddPath(TEXTURE_PATH, TEXT("Texture\\"));
    AddPath(FONT_PATH, TEXT("Font\\"));
    AddPath(ANIMATION_PATH, TEXT("Animation\\"));
    AddPath(SCENE_PATH, TEXT("Scene\\"));
    AddPath(SOUND_PATH, TEXT("Sound\\"));
    AddPath(MESH_PATH, TEXT("Mesh\\"));
    AddPath(EXCEL_PATH, TEXT("Excel\\"));
    AddPath(PARTICLE_PATH, TEXT("ParticleClass\\"));
    AddPath(OBJECT_PATH, TEXT("Object\\"));
    AddPath(COMPONENT_PATH, TEXT("Component\\"));
    AddPath(MATERIAL_PATH, TEXT("Material\\"));
    AddPath(MATERIAL_ANIMCOMPONENT_PATH, TEXT("Material\\AnimComponentMaterial\\"));
    AddPath(MATERIAL_PARTICLE_PATH, TEXT("Material\\ParticleMaterial\\"));

    // Resource Path
    TCHAR   IncludePath[MAX_PATH] = {};

    GetModuleFileName(0, IncludePath, MAX_PATH);

    int IncludePathCount = 0;

    for (int i = Length - 1; i > 0; --i)
    {
        if (IncludePath[i] == '\\' )
        {
            if (IncludePathCount == 1)
            {
                memset(&IncludePath[i + 1], 0, sizeof(TCHAR) * (Length - i - 1));
                break;
            }
            IncludePathCount += 1;
        }
    }

    PathInfo* IncludeInfo = new PathInfo;

    lstrcpy(IncludeInfo->Path, IncludePath);

#ifdef UNICODE
    // 유니코드 문자열을 멀티바이트 문자열로 변환한다.
    int IncludeConvertLength = WideCharToMultiByte(CP_ACP, 0, IncludePath, -1, nullptr, 0, 0, 0);

    WideCharToMultiByte(CP_ACP, 0, IncludePath, -1,
        IncludeInfo->PathMultibyte, IncludeConvertLength, 0, 0);
#else
    strcpy_s(IncludeInfo->PathMultibyte, IncludePath);
#endif // UNICODE

    m_mapPath.insert(std::make_pair(RESOURCE_ROOT_PATH, IncludeInfo));

    // Bin Folder 가 아니라, Inclue 폴더로 접근하는 코드
    AddPath(RESOURCE_SHADER_PATH, TEXT("Include\\Resource\\Shader\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_TEXTURE_PATH, TEXT("Include\\Resource\\Texture\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_FONT_PATH, TEXT("Include\\Resource\\Font\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_ANIMATION_PATH, TEXT("Include\\Resource\\Animation\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_SCENE_PATH, TEXT("Include\\Resource\\Scene\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_SOUND_PATH, TEXT("Include\\Resource\\Sound\\"), RESOURCE_ROOT_PATH);
    // AddPath(RESOURCE_MESH_PATH, TEXT("Include\\Resource\\Mesh\\"));
    AddPath(RESOURCE_EXCEL_PATH, TEXT("Include\\Resource\\Excel\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_OBJECT_PATH, TEXT("Include\\Resource\\Object\\"), RESOURCE_ROOT_PATH);
    AddPath(RESOURCE_PARTICLE_PATH, TEXT("Include\\Resource\\Texture\\ParticleClass\\"), RESOURCE_ROOT_PATH);

    // Engine Path 만들기 ---
    TCHAR EnginePath[MAX_PATH] = {};
    GetModuleFileName(nullptr, EnginePath, MAX_PATH);

    int EnginePathLength = lstrlen(EnginePath);

    int Count = 0;

    for (int i = EnginePathLength - 1; i > 0; --i)
    {
        if (EnginePath[i] == '\\')
        {
            Count += 1;

            if (Count == 3)
            {
                memset(&EnginePath[i + 1], 0, sizeof(TCHAR) * (EnginePathLength - i - 1));
                break;
            }
        }
    }

    PathInfo* GameEngineInfo = new PathInfo;

    lstrcpy(GameEngineInfo->Path, EnginePath);

#ifdef UNICODE
    // 유니코드 문자열을 멀티바이트 문자열로 변환한다.
    int ConvertEngineLength = WideCharToMultiByte(CP_ACP, 0, EnginePath, -1, nullptr, 0, nullptr, nullptr);

    WideCharToMultiByte(CP_ACP, 0, EnginePath, -1,
        GameEngineInfo->PathMultibyte, ConvertEngineLength, nullptr, nullptr);
#else
    strcpy_s(Info->PathMultibyte, Path);
#endif // UNICODE

    m_mapPath.insert(std::make_pair(ENGINE_ROOT_PATH, GameEngineInfo));
    
    AddPath(ENGINE_RESOURCE_SHADER_PATH, TEXT("GameEngine\\Include\\Resource\\Shader\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_TEXTURE_PATH, TEXT("GameEngine\\Include\\Resource\\Texture\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_FONT_PATH, TEXT("GameEngine\\Include\\Resource\\Font\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_ANIMATION_PATH, TEXT("GameEngine\\Include\\Resource\\Animation\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_SCENE_PATH, TEXT("GameEngine\\Include\\Resource\\Scene\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_SOUND_PATH, TEXT("GameEngine\\Include\\Resource\\Sound\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_SEQUENCE_PATH, TEXT("GameEngine\\Include\\Resource\\Sequence\\"), ENGINE_ROOT_PATH);
    AddPath(ENGINE_RESOURCE_OBJECT_PATH, TEXT("GameEngine\\Include\\Resource\\Object\\"), ENGINE_ROOT_PATH);

    return true;
}

bool CPathManager::AddPath(const std::string& Name, const TCHAR* Path, 
    const std::string& BaseName)
{
    if (FindPath(Name))
        return false;

    const PathInfo* BaseInfo = FindPath(BaseName);

    if (!BaseInfo)
        return false;

    PathInfo* Info = new PathInfo;

    lstrcpy(Info->Path, BaseInfo->Path);
    lstrcat(Info->Path, Path);

#ifdef UNICODE
    // 유니코드 문자열을 멀티바이트 문자열로 변환한다.
    int ConvertLength = WideCharToMultiByte(CP_ACP, 0, Info->Path, -1, nullptr, 0, 0, 0);

    WideCharToMultiByte(CP_ACP, 0, Info->Path, -1,
        Info->PathMultibyte, ConvertLength, 0, 0);
#else
    strcpy_s(Info->PathMultibyte, Info->Path);
#endif // UNICODE

    m_mapPath.insert(std::make_pair(Name, Info));

    return true;
}

const PathInfo* CPathManager::FindPath(const std::string& Name)
{
    std::unordered_map<std::string, PathInfo*>::iterator iter = m_mapPath.find(Name);

    if (iter == m_mapPath.end())
        return nullptr;

    return iter->second;
}
