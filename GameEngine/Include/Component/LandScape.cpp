#include "LandScape.h"
#include "../PathManager.h"
#include "../Resource/Mesh/StaticMesh.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Scene/Navigation3DManager.h"
#include "../Resource/Material/Material.h"
#include "../Resource/Shader/LandScapeConstantBuffer.h"
#include "../Render/RenderManager.h"
#include "../Render/RenderState.h"
#include "../Render/RenderStateManager.h"

#include "../d3dx10math.h"

CLandScape::CLandScape() :
    m_CountX(0),
    m_CountZ(0),
    m_CBuffer(nullptr),
    m_Min(FLT_MAX, FLT_MAX, FLT_MAX),
    m_Max(FLT_MIN, FLT_MIN, FLT_MIN),
    m_YFactor(30.f),
    m_Create(false)
{
    SetTypeID<CLandScape>();
    m_Render = true;

    m_DebugRender = true;
}

CLandScape::CLandScape(const CLandScape& com) :
    CSceneComponent(com)
{
    m_CountX = com.m_CountX;
    m_CountZ = com.m_CountZ;
    m_Mesh = com.m_Mesh;
    m_vecVtx = com.m_vecVtx;
    m_vecPos = com.m_vecPos;
    m_vecIndex = com.m_vecIndex;

    m_CBuffer = com.m_CBuffer->Clone();
}

CLandScape::~CLandScape()
{
    SAFE_DELETE(m_CBuffer);
}

void CLandScape::CreateLandScape(const std::string& Name, int CountX, int CountZ, CTexture* HeightMapTexture, float YFactor)
{
    m_CountX = CountX;
    m_CountZ = CountZ;

    m_PrevYFactor = YFactor;
    m_YFactor = YFactor;

    std::vector<float>   vecY;

    // 이미지 형태의 HeightMap파일 경로가 주어진 경우 높이값을 이미지로부터 얻어온다.
    if (HeightMapTexture)
    {
        // 픽셀 값을 꺼내온다.
        const Image* PixelInfo = HeightMapTexture->GetScratchImage()->GetImages();

        for (size_t i = 0; i < PixelInfo->height; ++i)
        {
            for (size_t j = 0; j < PixelInfo->width; ++j)
            {
                // rgba 1byte씩 저장되어 있으므로, 1pixel씩 접근하려면 index가 4의 배수로 늘어나야 함
                int   PixelIndex = (int)i * (int)PixelInfo->width * 4 + (int)j * 4;

                // Y값이 너무 커지는 것을 막기 위해 나누어준다.
                float   Y = PixelInfo->pixels[PixelIndex] / m_YFactor;

                vecY.push_back(Y);
            }
        }
    }

    // 높이 맵 파일이 없는 경우
    else
        vecY.resize(CountX * CountZ);

    // Vertex 생성
    for (int i = 0; i < m_CountZ; ++i)
    {
        for (int j = 0; j < m_CountX; ++j)
        {
            Vertex3D   Vtx = {};

            Vtx.Pos = Vector3((float)j, vecY[i * (int)m_CountX + j],
                (float)m_CountZ - i - 1);
            Vtx.UV = Vector2(j / (float)(m_CountX - 1),
                i / (float)m_CountZ - 1); \

                m_vecPos.push_back(Vtx.Pos);

            if (m_Min.x > Vtx.Pos.x)
                m_Min.x = Vtx.Pos.x;

            if (m_Min.y > Vtx.Pos.y)
                m_Min.y = Vtx.Pos.y;

            if (m_Min.z > Vtx.Pos.z)
                m_Min.z = Vtx.Pos.z;

            if (m_Max.x < Vtx.Pos.x)
                m_Max.x = Vtx.Pos.x;

            if (m_Max.y < Vtx.Pos.y)
                m_Max.y = Vtx.Pos.y;

            if (m_Max.z < Vtx.Pos.z)
                m_Max.z = Vtx.Pos.z;

            m_vecVtx.push_back(Vtx);
        }
    }

    SetMeshSize(m_Max - m_Min);
    m_SphereInfo.Center = (m_Max + m_Min) / 2.f;

    // 인덱스 정보 만들기
    // 면 법선은 삼각형 수만큼 만들어져야 한다.
    Vector3   TrianglePos[3];
    Vector3   Edge1, Edge2;
    unsigned int   TriIndex = 0;

    // 사각형 수 * 2
    m_vecFaceNormal.resize((m_CountX - 1) * (m_CountZ - 1) * 2);

    for (unsigned int i = 0; i < (unsigned int)m_CountZ - 1; ++i)
    {
        for (unsigned int j = 0; j < (unsigned int)m_CountX - 1; ++j)
        {
            unsigned int   Index = i * m_CountX + j;

            // 사각형의 우 상단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + 1);
            m_vecIndex.push_back(Index + m_CountX + 1);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX + 1].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;


            // 사각형의 좌 하단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + m_CountX + 1);
            m_vecIndex.push_back(Index + m_CountX);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + m_CountX + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;
        }
    }

    ComputeNormal();

    ComputeTangent();

    // 메쉬를 만든다.
    m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, Name,
        &m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
        D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        &m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

    m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(Name);

    m_MeshName = Name;

    m_Scene->GetNavigation3DManager()->SetNavData(this);

    m_Create = true;
}

void CLandScape::CreateLandScape(const std::string& Name,
    int CountX, int CountZ, const TCHAR* HeightMap,
    const std::string& PathName, float YFactor)
{
    m_CountX = CountX;
    m_CountZ = CountZ;

    m_PrevYFactor = YFactor;
    m_YFactor = YFactor;

    std::vector<float>   vecY;

    // 이미지 형태의 HeightMap파일 경로가 주어진 경우 높이값을 이미지로부터 얻어온다.
    if (HeightMap)
    {
        TCHAR   FullPath[MAX_PATH] = {};

        const PathInfo* Path = CPathManager::GetInst()->FindPath(PathName);

        if (Path)
            lstrcpy(FullPath, Path->Path);
        lstrcat(FullPath, HeightMap);

        ScratchImage   Img;

        char   Ext[_MAX_EXT] = {};
        char   FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE

        int   ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
        WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, ConvertLength, nullptr, nullptr);

#else

        strcpy_s(FullPathMultibyte, FullPath);

#endif // UNICODE

        _splitpath_s(FullPathMultibyte, nullptr, 0, nullptr, 0, nullptr, 0, Ext, _MAX_EXT);

        _strupr_s(Ext);

        if (strcmp(Ext, ".DDS") == 0)
        {
            if (FAILED(LoadFromDDSFile(FullPath, DDS_FLAGS_NONE, nullptr, Img)))
            {
                return;
            }
        }

        else if (strcmp(Ext, ".TGA") == 0)
        {
            if (FAILED(LoadFromTGAFile(FullPath, nullptr, Img)))
            {
                return;
            }
        }

        else
        {
            if (FAILED(LoadFromWICFile(FullPath, WIC_FLAGS_NONE, nullptr, Img)))
            {
                return;
            }
        }

        // 픽셀 값을 꺼내온다.
        const Image* PixelInfo = Img.GetImages();

        for (size_t i = 0; i < PixelInfo->height; ++i)
        {
            for (size_t j = 0; j < PixelInfo->width; ++j)
            {
                // rgba 1byte씩 저장되어 있으므로, 1pixel씩 접근하려면 index가 4의 배수로 늘어나야 함
                int   PixelIndex = (int)i * (int)PixelInfo->width * 4 + (int)j * 4;

                // Y값이 너무 커지는 것을 막기 위해 나누어준다.
                float   Y = PixelInfo->pixels[PixelIndex] / m_YFactor;

                vecY.push_back(Y);
            }
        }
    }

    // 높이 맵 파일이 없는 경우
    else
        vecY.resize(CountX * CountZ);

    // Vertex 생성
    for (int i = 0; i < m_CountZ; ++i)
    {
        for (int j = 0; j < m_CountX; ++j)
        {
            Vertex3D   Vtx = {};

            Vtx.Pos = Vector3((float)j, vecY[i * (int)m_CountX + j],
                (float)m_CountZ - i - 1);
            Vtx.UV = Vector2(j / (float)(m_CountX - 1),
                i / (float)m_CountZ - 1); \

                m_vecPos.push_back(Vtx.Pos);

            if (m_Min.x > Vtx.Pos.x)
                m_Min.x = Vtx.Pos.x;

            if (m_Min.y > Vtx.Pos.y)
                m_Min.y = Vtx.Pos.y;

            if (m_Min.z > Vtx.Pos.z)
                m_Min.z = Vtx.Pos.z;

            if (m_Max.x < Vtx.Pos.x)
                m_Max.x = Vtx.Pos.x;

            if (m_Max.y < Vtx.Pos.y)
                m_Max.y = Vtx.Pos.y;

            if (m_Max.z < Vtx.Pos.z)
                m_Max.z = Vtx.Pos.z;

            m_vecVtx.push_back(Vtx);
        }
    }

    SetMeshSize(m_Max - m_Min);
    m_SphereInfo.Center = (m_Max + m_Min) / 2.f;

    // 인덱스 정보 만들기
    // 면 법선은 삼각형 수만큼 만들어져야 한다.
    Vector3   TrianglePos[3];
    Vector3   Edge1, Edge2;
    unsigned int   TriIndex = 0;

    // 사각형 수 * 2
    m_vecFaceNormal.resize((m_CountX - 1) * (m_CountZ - 1) * 2);

    for (unsigned int i = 0; i < (unsigned int)m_CountZ - 1; ++i)
    {
        for (unsigned int j = 0; j < (unsigned int)m_CountX - 1; ++j)
        {
            unsigned int   Index = i * m_CountX + j;

            // 사각형의 우 상단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + 1);
            m_vecIndex.push_back(Index + m_CountX + 1);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX + 1].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;


            // 사각형의 좌 하단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + m_CountX + 1);
            m_vecIndex.push_back(Index + m_CountX);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + m_CountX + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;
        }
    }

    ComputeNormal();

    ComputeTangent();

    // 메쉬를 만든다.
    m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, Name,
        &m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
        D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        &m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

    m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(Name);

    m_MeshName = Name;

    m_Scene->GetNavigation3DManager()->SetNavData(this);

    m_Create = true;
}

void CLandScape::SetMaterial(CMaterial* Material, int Index)
{
    m_vecMaterialSlot[Index] = Material->Clone();

    m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CLandScape::SetMaterial(const std::string& Name, int Index)
{
    CMaterial* Material = m_Scene->GetResource()->FindMaterial(Name);

    m_vecMaterialSlot[Index] = Material->Clone();

    m_vecMaterialSlot[Index]->SetScene(m_Scene);
}

void CLandScape::AddMaterial(CMaterial* Material)
{
    m_vecMaterialSlot.push_back(Material->Clone());

    m_vecMaterialSlot[m_vecMaterialSlot.size() - 1]->SetScene(m_Scene);
}

void CLandScape::AddMaterial(const std::string& Name)
{
    CMaterial* Material = m_Scene->GetResource()->FindMaterial(Name);

    m_vecMaterialSlot.push_back(Material->Clone());

    m_vecMaterialSlot[m_vecMaterialSlot.size() - 1]->SetScene(m_Scene);
}

void CLandScape::SetDetailLevel(float Level)
{
    m_CBuffer->SetDetailLevel(Level);
}

void CLandScape::SetSplatCount(int Count)
{
    m_CBuffer->SetSplatCount(Count);
}

void CLandScape::SetYFactor(float YFactor)
{
    m_PrevYFactor = m_YFactor;

    m_YFactor = YFactor;

    RecreateY();
}

float CLandScape::GetHeight(const Vector3& Pos)
{
    Vector3   Convert = (Pos - GetWorldPos()) / GetWorldScale();

    // z 좌표 역으로 계산
    Convert.z = m_CountZ - 1 - Convert.z;

    int   IndexX = (int)Convert.x;
    int   IndexZ = (int)Convert.z;

    if (IndexX < 0 || IndexX >= m_CountX || IndexZ < 0 || IndexZ >= m_CountZ - 1)
        return Pos.y;

    int   Index = IndexZ * m_CountX + IndexX;

    float   RatioX = Convert.x - IndexX;
    float   RatioZ = Convert.z - IndexZ;

    float   Y[4] =
    {
       m_vecPos[Index].y,
       m_vecPos[Index + 1].y,
       m_vecPos[Index + m_CountX].y,
       m_vecPos[Index + m_CountX + 1].y
    };

    // 우상단 삼각형
    if (RatioX > RatioZ)
        return Y[0] + (Y[1] - Y[0]) * RatioX + (Y[3] - Y[1]) * RatioZ;

    return Y[0] + (Y[3] - Y[2]) * RatioX + (Y[2] - Y[0]) * RatioZ;
}

bool CLandScape::CheckInArea(const Vector3& StartPos, const Vector3& EndPos, Vector3& OutPos)
{
    ////////// StartPos //////////
    Vector3   Convert = (StartPos - GetWorldPos()) / GetWorldScale();

    // z 좌표 역으로 계산
    Convert.z = m_CountZ - 1 - Convert.z;

    int   StartIndexX = (int)Convert.x;
    int   StartIndexZ = (int)Convert.z;

    if (StartIndexX < 0 || StartIndexX >= m_CountX || StartIndexZ < 0 || StartIndexZ >= m_CountZ - 1)
        return false;

    int   StartPosIndex = StartIndexZ * m_CountX + StartIndexX;

    float   RatioX = Convert.x - StartIndexX;
    float   RatioZ = Convert.z - StartIndexZ;

    float   Y[4] =
    {
       m_vecPos[StartPosIndex].y,
       m_vecPos[StartPosIndex + 1].y,
       m_vecPos[StartPosIndex + m_CountX].y,
       m_vecPos[StartPosIndex + m_CountX + 1].y
    };

    // 우상단 삼각형
    if (RatioX > RatioZ)
    {
        float LandScapeHeight = Y[0] + (Y[1] - Y[0]) * RatioX + (Y[3] - Y[1]) * RatioZ;

        if (StartPos.y < LandScapeHeight || StartPos.y - LandScapeHeight > 0.3f)
            return false;
    }

    else
    {
        float LandScapeHeight = Y[0] + (Y[3] - Y[2]) * RatioX + (Y[2] - Y[0]) * RatioZ;

        if (StartPos.y < LandScapeHeight || StartPos.y - LandScapeHeight > 0.3f)
            return false;
    }

    ////////// EndPos //////////
    Convert = (EndPos - GetWorldPos()) / GetWorldScale();

    // z 좌표 역으로 계산
    Convert.z = m_CountZ - 1 - Convert.z;

    int   EndIndexX = (int)Convert.x;
    int   EndIndexZ = (int)Convert.z;

    if (EndIndexX < 0 || EndIndexX >= m_CountX || EndIndexZ < 0 || EndIndexZ >= m_CountZ - 1)
        return false;

    int   EndPosIndex = EndIndexZ * m_CountX + EndIndexX;

    RatioX = Convert.x - EndIndexX;
    RatioZ = Convert.z - EndIndexZ;

    float   EndY[4] =
    {
       m_vecPos[EndPosIndex].y,
       m_vecPos[EndPosIndex + 1].y,
       m_vecPos[EndPosIndex + m_CountX].y,
       m_vecPos[EndPosIndex + m_CountX + 1].y
    };

    // 우상단 삼각형
    if (RatioX > RatioZ)
    {
        float LandScapeHeight = EndY[0] + (EndY[1] - EndY[0]) * RatioX + (EndY[3] - EndY[1]) * RatioZ;

        if (EndPos.y > LandScapeHeight || LandScapeHeight - EndPos.y > 0.3f)
            return false;
    }

    else
    {
        float LandScapeHeight = EndY[0] + (EndY[3] - EndY[2]) * RatioX + (EndY[2] - EndY[0]) * RatioZ;

        if (EndPos.y > LandScapeHeight || LandScapeHeight - EndPos.y > 0.3f)
            return false;
    }

    if (StartPos.x < m_Min.x && EndPos.x - StartPos.x <= 0.f)
        return false;

    if (StartPos.z < m_Min.z && EndPos.z - StartPos.z <= 0.f)
        return false;

    if (StartPos.x > m_Max.x && EndPos.x - StartPos.x >= 0.f)
        return false;

    if (StartPos.z > m_Max.z && EndPos.z - StartPos.z >= 0.f)
        return false;

    std::vector<struct Polygon> vecPolygon;

    int Start = StartPosIndex < EndPosIndex ? StartPosIndex : EndPosIndex;
    int End = StartPosIndex < EndPosIndex ? EndPosIndex : StartPosIndex;

    for (int i = 0; i <= End - Start; ++i)
    {
        struct Polygon pol;

        // 우상단 삼각형
        pol.v1 = Vector3(m_vecPos[StartPosIndex + i]);
        pol.v2 = Vector3(m_vecPos[StartPosIndex + i + 1]);
        pol.v3 = Vector3(m_vecPos[StartPosIndex + m_CountX + i + 1]);

        vecPolygon.push_back(pol);

        // 좌하단 삼각형
        pol.v1 = Vector3(m_vecPos[StartPosIndex + i]);
        pol.v2 = Vector3(m_vecPos[StartPosIndex + m_CountX + i + 1]);
        pol.v3 = Vector3(m_vecPos[StartPosIndex + m_CountX + i]);

        vecPolygon.push_back(pol);
    }

    // 모아놓은 polygon 루프 돌면서 StartPos와 EndPos를 잇는 직선이 vecPolygon안에 있는 polygon들 중에 
    // intersect하는게 있다면 바로 그걸 OutPos에 할당하고 리턴
    size_t Count = vecPolygon.size();

    Vector3 RayDirection = EndPos - StartPos;

    for (size_t i = 0; i < Count; ++i)
    {
        Vector3 p1 = vecPolygon[i].v1;
        Vector3 p2 = vecPolygon[i].v2;
        Vector3 p3 = vecPolygon[i].v3;

        Vector3 WorldPos = GetWorldPos();
        Vector3 WorldScale = GetWorldScale();

        p1 = p1 * WorldScale + WorldPos;
        p2 = p2 * WorldScale + WorldPos;
        p3 = p3 * WorldScale + WorldPos;

        Vector3 Edge1 = p2 - p1;
        Vector3 Edge2 = p3 - p1;

        Vector3 Normal = Edge1.Cross(Edge2);
        Normal.Normalize();

        // Normal.X(x - p1.x) + Normal.Y(y - p1.y) + Normal.Z(z - p1.z) = 0
        // 평면의 방정식이 ax + by + cz + d 일때, a는 Normal.x, b는 Normal.y, c는 Normal.z
        // d는 -Normal.X * p1.x - Normal.Y * p1.y - Normal.Z * p1.z
        float Coff_a = Normal.x;
        float Coff_b = Normal.y;
        float Coff_c = Normal.z;
        float Coff_d = -Normal.x * p1.x - Normal.y * p1.y - Normal.z * p1.z;

        XMVECTOR Plane = { Coff_a, Coff_b, Coff_c, Coff_d };

        // polygon 세 점의 월드좌표
        //XMVECTOR XMVec1 = (p1 * WorldScale + WorldPos).Convert();
        //XMVECTOR XMVec2 = (p2 * WorldScale + WorldPos).Convert();
        //XMVECTOR XMVec3 = (p3 * WorldScale + WorldPos).Convert();
        //XMVECTOR XMVec1 = p1.Convert();
        //XMVECTOR XMVec2 = p2.Convert();
        //XMVECTOR XMVec3 = p3.Convert();

        XMVECTOR Result = XMPlaneIntersectLine(Plane, StartPos.Convert(), EndPos.Convert());

        // Intersect하는 polygon을 찾지 못했을 때
        if (isnan(Result.m128_f32[0]) && isnan(Result.m128_f32[1]) && isnan(Result.m128_f32[2]))
        {
            //return true;
            return false;
        }

        OutPos.x = Result.m128_f32[0];
        OutPos.y = Result.m128_f32[1];
        OutPos.z = Result.m128_f32[2];

    }

    return true;
}


void CLandScape::Start()
{
    CSceneComponent::Start();
}

bool CLandScape::Init()
{
    m_CBuffer = new CLandScapeConstantBuffer;

    m_CBuffer->Init();

    m_WireFrameShader = m_Scene->GetResource()->FindShader("Standard3DWireFrameShader");
    m_WireFrameState = CRenderManager::GetInst()->FindRenderState("Wireframe");

    return true;
}

void CLandScape::Update(float DeltaTime)
{
    CSceneComponent::Update(DeltaTime);
}

void CLandScape::PostUpdate(float DeltaTime)
{
    CSceneComponent::PostUpdate(DeltaTime);
}

void CLandScape::PrevRender()
{
    CSceneComponent::PrevRender();
}

void CLandScape::Render()
{
    CSceneComponent::Render();

    if (!m_Mesh)
        return;

    m_CBuffer->UpdateCBuffer();

    size_t   Size = m_vecMaterialSlot.size();

    for (size_t i = 0; i < Size; ++i)
    {
        m_vecMaterialSlot[i]->EnableDecal(m_ReceiveDecal);

        m_vecMaterialSlot[i]->Render();

        m_Mesh->Render((int)i);

        m_vecMaterialSlot[i]->Reset();
    }

    // WireFrame Render
    if (m_DebugRender)
    {
        m_WireFrameShader->SetShader();

        m_WireFrameState->SetState();

        m_Mesh->Render();

        m_WireFrameState->ResetState();
    }
}

void CLandScape::RenderShadowMap()
{
    CSceneComponent::RenderShadowMap();

    if (!m_Mesh)
    {
        return;
    }

    m_CBuffer->UpdateCBuffer();

    size_t   Size = m_vecMaterialSlot.size();

    for (size_t i = 0; i < Size; ++i)
    {
        m_Mesh->Render((int)i);
    }
}

void CLandScape::PostRender()
{
    CSceneComponent::PostRender();
}

CLandScape* CLandScape::Clone()
{
    return new CLandScape(*this);
}

bool CLandScape::Save(FILE* File)
{
    CSceneComponent::Save(File);

    int Length = (int)m_MeshName.length();
    fwrite(&Length, sizeof(int), 1, File);
    fwrite(m_MeshName.c_str(), sizeof(char), Length, File);

    fwrite(&m_YFactor, sizeof(float), 1, File);
    fwrite(&m_PrevYFactor, sizeof(float), 1, File);
    fwrite(&m_DebugRender, sizeof(bool), 1, File);

    fwrite(&m_CountX, sizeof(int), 1, File);
    fwrite(&m_CountZ, sizeof(int), 1, File);

    fwrite(&m_Min, sizeof(Vector3), 1, File);
    fwrite(&m_Max, sizeof(Vector3), 1, File);

    size_t Size = m_vecVtx.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecVtx[i], sizeof(Vertex3D), 1, File);
        fwrite(&m_vecPos[i], sizeof(Vector3), 1, File);
    }

    Size = m_vecIndex.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecIndex[i], sizeof(unsigned int), 1, File);
    }

    Size = m_vecFaceNormal.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecFaceNormal[i], sizeof(Vector3), 1, File);
    }

    Size = m_vecMaterialSlot.size();
    for (size_t i = 0; i < Size; ++i)
    {
        m_vecMaterialSlot[i]->Save(File);
    }

    return true;
}

bool CLandScape::Load(FILE* File)
{
    CSceneComponent::Load(File);

    char MeshName[128] = {};
    int Length = 0;
    fread(&Length, sizeof(int), 1, File);
    fread(MeshName, sizeof(char), Length, File);
    m_MeshName = MeshName;

    fread(&m_YFactor, sizeof(float), 1, File);
    fread(&m_PrevYFactor, sizeof(float), 1, File);
    fread(&m_DebugRender, sizeof(bool), 1, File);

    fread(&m_CountX, sizeof(int), 1, File);
    fread(&m_CountZ, sizeof(int), 1, File);

    fread(&m_Min, sizeof(Vector3), 1, File);
    fread(&m_Max, sizeof(Vector3), 1, File);

    size_t Size = 0;
    fread(&Size, sizeof(size_t), 1, File);
    m_vecVtx.resize(Size);
    m_vecPos.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecVtx[i], sizeof(Vertex3D), 1, File);
        fread(&m_vecPos[i], sizeof(Vector3), 1, File);
    }

    fread(&Size, sizeof(size_t), 1, File);
    m_vecIndex.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecIndex[i], sizeof(unsigned int), 1, File);
    }

    fread(&Size, sizeof(size_t), 1, File);
    m_vecFaceNormal.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecFaceNormal[i], sizeof(Vector3), 1, File);
    }


    Size = m_vecMaterialSlot.size();
    m_vecMaterialSlot.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        m_vecMaterialSlot[i] = new CMaterial;
        m_vecMaterialSlot[i]->Load(File);
    }

    ComputeNormal();
    ComputeTangent();

    // 메쉬를 만든다.
    m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, m_MeshName,
        &m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
        D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        &m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

    m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(m_MeshName);

    if (!m_Mesh)
    {
        assert(false);
        return false;
    }

    m_Create = true;
    m_Scene->GetNavigation3DManager()->SetNavData(this);

    m_CBuffer = new CLandScapeConstantBuffer;
    m_CBuffer->Init();

    m_WireFrameShader = m_Scene->GetResource()->FindShader("Standard3DWireFrameShader");
    m_WireFrameState = CRenderManager::GetInst()->FindRenderState("Wireframe");

    return true;
}

bool CLandScape::SaveOnly(FILE* File)
{
    CSceneComponent::SaveOnly(File);

    int Length = (int)m_MeshName.length();
    fwrite(&Length, sizeof(int), 1, File);
    fwrite(m_MeshName.c_str(), sizeof(char), Length, File);

    fwrite(&m_YFactor, sizeof(float), 1, File);
    fwrite(&m_PrevYFactor, sizeof(float), 1, File);
    fwrite(&m_DebugRender, sizeof(bool), 1, File);

    fwrite(&m_CountX, sizeof(int), 1, File);
    fwrite(&m_CountZ, sizeof(int), 1, File);

    fwrite(&m_Min, sizeof(Vector3), 1, File);
    fwrite(&m_Max, sizeof(Vector3), 1, File);

    size_t Size = m_vecVtx.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecVtx[i], sizeof(Vertex3D), 1, File);
        fwrite(&m_vecPos[i], sizeof(Vector3), 1, File);
    }

    Size = m_vecIndex.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecIndex[i], sizeof(unsigned int), 1, File);
    }

    Size = m_vecFaceNormal.size();
    fwrite(&Size, sizeof(size_t), 1, File);
    for (size_t i = 0; i < Size; ++i)
    {
        fwrite(&m_vecFaceNormal[i], sizeof(Vector3), 1, File);
    }

    Size = m_vecMaterialSlot.size();
    for (size_t i = 0; i < Size; ++i)
    {
        m_vecMaterialSlot[i]->Save(File);
    }

    return true;
}

bool CLandScape::LoadOnly(FILE* File)
{
    CSceneComponent::LoadOnly(File);

	char MeshName[128] = {};
    int Length = 0;
    fread(&Length, sizeof(int), 1, File);
    fread(MeshName, sizeof(char), Length, File);
    m_MeshName = MeshName;

    fread(&m_YFactor, sizeof(float), 1, File);
    fread(&m_PrevYFactor, sizeof(float), 1, File);
    fread(&m_DebugRender, sizeof(bool), 1, File);

    fread(&m_CountX, sizeof(int), 1, File);
    fread(&m_CountZ, sizeof(int), 1, File);

    fread(&m_Min, sizeof(Vector3), 1, File);
    fread(&m_Max, sizeof(Vector3), 1, File);

    size_t Size = 0;
    fread(&Size, sizeof(size_t), 1, File);
    m_vecVtx.resize(Size);
    m_vecPos.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecVtx[i], sizeof(Vertex3D), 1, File);
        fread(&m_vecPos[i], sizeof(Vector3), 1, File);
    }

    fread(&Size, sizeof(size_t), 1, File);
    m_vecIndex.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecIndex[i], sizeof(unsigned int), 1, File);
    }

    fread(&Size, sizeof(size_t), 1, File);
    m_vecFaceNormal.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        fread(&m_vecFaceNormal[i], sizeof(Vector3), 1, File);
    }


    Size = m_vecMaterialSlot.size();
    m_vecMaterialSlot.resize(Size);
    for (size_t i = 0; i < Size; ++i)
    {
        m_vecMaterialSlot[i] = new CMaterial;
        m_vecMaterialSlot[i]->Load(File);
    }

    ComputeNormal();
    ComputeTangent();

    // 메쉬를 만든다.
    m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, m_MeshName,
        &m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
        D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        &m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

    m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(m_MeshName);

    if (!m_Mesh)
    {
        assert(false);
        return false;
    }

    m_Create = true;
    m_Scene->GetNavigation3DManager()->SetNavData(this);

    m_CBuffer = new CLandScapeConstantBuffer;
    m_CBuffer->Init();

    m_WireFrameShader = m_Scene->GetResource()->FindShader("Standard3DWireFrameShader");
    m_WireFrameState = CRenderManager::GetInst()->FindRenderState("Wireframe");

    return true;
}

void CLandScape::ComputeNormal()
{
    size_t   Size = m_vecFaceNormal.size();

    for (size_t i = 0; i < Size; ++i)
    {
        unsigned int   Idx0 = m_vecIndex[i * 3];
        unsigned int   Idx1 = m_vecIndex[i * 3 + 1];
        unsigned int   Idx2 = m_vecIndex[i * 3 + 2];

        m_vecVtx[Idx0].Normal += m_vecFaceNormal[i];
        m_vecVtx[Idx1].Normal += m_vecFaceNormal[i];
        m_vecVtx[Idx2].Normal += m_vecFaceNormal[i];
    }

    Size = m_vecVtx.size();

    for (size_t i = 0; i < Size; ++i)
    {
        m_vecVtx[i].Normal.Normalize();
    }
}

void CLandScape::ComputeTangent()
{
    // 탄젠트 벡터 구함.
    for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
    {
        int   idx0 = m_vecIndex[i * 3];
        int   idx1 = m_vecIndex[i * 3 + 1];
        int   idx2 = m_vecIndex[i * 3 + 2];

        float   fVtx1[3], fVtx2[3];
        fVtx1[0] = m_vecVtx[idx1].Pos.x - m_vecVtx[idx0].Pos.x;
        fVtx1[1] = m_vecVtx[idx1].Pos.y - m_vecVtx[idx0].Pos.y;
        fVtx1[2] = m_vecVtx[idx1].Pos.z - m_vecVtx[idx0].Pos.z;

        fVtx2[0] = m_vecVtx[idx2].Pos.x - m_vecVtx[idx0].Pos.x;
        fVtx2[1] = m_vecVtx[idx2].Pos.y - m_vecVtx[idx0].Pos.y;
        fVtx2[2] = m_vecVtx[idx2].Pos.z - m_vecVtx[idx0].Pos.z;

        float   ftu[2], ftv[2];
        ftu[0] = m_vecVtx[idx1].UV.x - m_vecVtx[idx0].UV.x;
        ftv[0] = m_vecVtx[idx1].UV.y - m_vecVtx[idx0].UV.y;

        ftu[1] = m_vecVtx[idx2].UV.x - m_vecVtx[idx0].UV.x;
        ftv[1] = m_vecVtx[idx2].UV.y - m_vecVtx[idx0].UV.y;

        float   fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

        Vector3   Tangent;
        Tangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
        Tangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
        Tangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

        Tangent.Normalize();

        m_vecVtx[idx0].Tangent = Tangent;
        m_vecVtx[idx1].Tangent = Tangent;
        m_vecVtx[idx2].Tangent = Tangent;

        m_vecVtx[idx0].Binormal = m_vecVtx[idx0].Normal.Cross(Tangent);
        m_vecVtx[idx1].Binormal = m_vecVtx[idx1].Normal.Cross(Tangent);
        m_vecVtx[idx2].Binormal = m_vecVtx[idx2].Normal.Cross(Tangent);

        m_vecVtx[idx0].Binormal.Normalize();
        m_vecVtx[idx1].Binormal.Normalize();
        m_vecVtx[idx2].Binormal.Normalize();
    }
}

void CLandScape::RecreateY()
{
    // Vertex Y 값 변경
    for (int i = 0; i < m_CountZ; ++i)
    {
        for (int j = 0; j < m_CountX; ++j)
        {
            int Index = i * (int)m_CountX + j;

            float PrevY = m_vecVtx[Index].Pos.y;
            float AfterY = (PrevY * m_PrevYFactor) / m_YFactor;

            m_vecVtx[Index].Pos.y = AfterY;

            if (m_Min.y > AfterY)
                m_Min.y = AfterY;

            if (m_Max.y < AfterY)
                m_Max.y = AfterY;
        }
    }

    SetMeshSize(m_Max - m_Min);
    m_SphereInfo.Center = (m_Max + m_Min) / 2.f;

    // Normal, Tangent, Binormal 재생성
    Vector3   TrianglePos[3];
    Vector3   Edge1, Edge2;
    unsigned int   TriIndex = 0;

    // 사각형 수 * 2
    m_vecFaceNormal.clear();

    m_vecFaceNormal.resize((m_CountX - 1) * (m_CountZ - 1) * 2);

    for (unsigned int i = 0; i < m_CountZ - 1; ++i)
    {
        for (unsigned int j = 0; j < m_CountX - 1; ++j)
        {
            unsigned int   Index = i * m_CountX + j;

            // 사각형의 우 상단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + 1);
            m_vecIndex.push_back(Index + m_CountX + 1);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX + 1].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;

            // 사각형의 좌 하단 삼각형
            m_vecIndex.push_back(Index);
            m_vecIndex.push_back(Index + m_CountX + 1);
            m_vecIndex.push_back(Index + m_CountX);

            TrianglePos[0] = m_vecVtx[Index].Pos;
            TrianglePos[1] = m_vecVtx[Index + m_CountX + 1].Pos;
            TrianglePos[2] = m_vecVtx[Index + m_CountX].Pos;

            Edge1 = TrianglePos[1] - TrianglePos[0];
            Edge2 = TrianglePos[2] - TrianglePos[0];

            Edge1.Normalize();
            Edge2.Normalize();

            m_vecFaceNormal[TriIndex] = Edge1.Cross(Edge2);
            m_vecFaceNormal[TriIndex].Normalize();

            ++TriIndex;
        }
    }

    ComputeNormal();

    ComputeTangent();

    // 기존 메쉬 Release
    m_Mesh = nullptr;
    m_Scene->GetResource()->ReleaseMesh(m_MeshName);
    CResourceManager::GetInst()->ReleaseMesh(m_MeshName);

    // 새로운 메쉬 생성
    m_Scene->GetResource()->CreateMesh(Mesh_Type::Static, m_MeshName,
        &m_vecVtx[0], sizeof(Vertex3D), (int)m_vecVtx.size(),
        D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        &m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
        D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);

    m_Mesh = (CStaticMesh*)m_Scene->GetResource()->FindMesh(m_MeshName);
}

bool CLandScape::SortHeight(struct Polygon Src, struct Polygon Dest)
{
    float SrcHeight = (Src.v1.y + Src.v2.y + Src.v3.y) / 3.f;
    float DestHeight = (Dest.v1.y + Dest.v2.y + Dest.v3.y) / 3.f;

    return SrcHeight > DestHeight;
}
