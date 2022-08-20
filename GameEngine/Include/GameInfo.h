#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define	DIRECTINPUT_VERSION	0x0800

#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <crtdbg.h>
#include <functional>
#include <string>
#include <math.h>
#include <algorithm>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <d2d1.h>
#include <stack>
#include <queue>
#include <process.h>
#include <utility>
#include <optional>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <thread>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "SharedPtr.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Resource/Texture/DirectXTex.h"
#include "fmod.hpp"

// IMGUIWidget, IMGUIWindow 들이 사용하는 공통 정보
// #include "IMGUIUtil.h"

#ifdef _DEBUG

#pragma comment(lib, "DirectXTex_Debug.lib")

#else

#pragma comment(lib, "DirectXTex.lib")

#endif // _DEBUG

#pragma comment(lib, "fmod64_vc.lib")

#define GRAVITY 9.8f

#define SHADOWMAP_WIDTH		2560.f
#define SHADOWMAP_HEIGHT	2560.f

#define	ROOT_PATH		"Root"
#define	SHADER_PATH		"Shader"
#define	TEXTURE_PATH	"Texture"
#define	FONT_PATH		"Font"
#define	ANIMATION_PATH	"Animation"
#define	SCENE_PATH		"Scene"
#define	SOUND_PATH		"Sound"
#define	MESH_PATH		"Mesh"
#define	PARTICLE_PATH		"ParticleClass"
#define	EXCEL_PATH		"Excel"
#define	OBJECT_PATH		"Object"
#define	COMPONENT_PATH	"Component"
#define	MATERIAL_PATH	 "Material"
#define	MATERIAL_PARTICLE_PATH	 "MaterialParticle"
#define	MATERIAL_ANIMCOMPONENT_PATH	 "MaterialAnimComponent"

// Inclue Path
#define	RESOURCE_ROOT_PATH		"IncludeRoot"
#define	RESOURCE_SHADER_PATH		"ResourceShader"
#define	RESOURCE_TEXTURE_PATH		"ResourceTexture"
#define	RESOURCE_FONT_PATH			"ResourceFont"
#define	RESOURCE_ANIMATION_PATH	"ResourceAnimation"
#define	RESOURCE_SCENE_PATH		"ResourceScene"
#define	RESOURCE_SOUND_PATH		"ResourceSound"
#define	RESOURCE_MESH_PATH			"ResourceMesh"
#define	RESOURCE_PARTICLE_PATH	"ResourceParticleClass"
#define	RESOURCE_EXCEL_PATH			"ResourceExcel"
#define	RESOURCE_OBJECT_PATH		"ResourceObject"
#define	RESOURCE_MATERIAL_PATH		"ResourceMaterial"

// Engine Path 
#define ENGINE_ROOT_PATH				"EngineRoot"
#define	ENGINE_RESOURCE_SHADER_PATH			"EngineShader"
#define	ENGINE_RESOURCE_TEXTURE_PATH			"EngineTexture"
#define	ENGINE_RESOURCE_FONT_PATH				"EngineFont"
#define	ENGINE_RESOURCE_ANIMATION_PATH		"EngineAnimation"
#define	ENGINE_RESOURCE_SCENE_PATH				"EngineScene"
#define	ENGINE_RESOURCE_SOUND_PATH			"EngineSound"
#define	ENGINE_RESOURCE_SEQUENCE_PATH		"EngineSequence"
#define	ENGINE_RESOURCE_OBJECT_PATH			"EngineObject"
#define	ENGINE_RESOURCE_MATERIAL_PATH			"EngineMaterial"

#define	SAFE_DELETE(p)	if(p)	{ delete p; p = nullptr; }
#define	SAFE_DELETE_ARRAY(p)	if(p)	{ delete[] p; p = nullptr; }
#define	SAFE_RELEASE(p)	if(p)	{ p->Release(); p = nullptr; }
#define	SAFE_DELETE_ARRAY_VECLIST(p)	\
for(auto iter = p.begin(); iter != p.end(); ++iter)\
{\
	SAFE_DELETE_ARRAY(*iter);\
}

#define	DECLARE_SINGLE(Type)	\
private:\
	static Type*	m_Inst;\
public:\
	static Type* GetInst()\
	{\
		if(!m_Inst)\
			m_Inst = new Type;\
		return m_Inst;\
	}\
	static void DestroyInst()\
	{\
		SAFE_DELETE(m_Inst);\
	}\
private:\
	Type();\
	~Type();

#define	DEFINITION_SINGLE(Type)	Type* Type::m_Inst = nullptr;
#define FAILED_CHECK(Executable)\
if(FAILED(Executable)){\
	assert(false);\
	return false;\
}\

struct Resolution
{
	unsigned int	Width;
	unsigned int	Height;
};

struct VertexColor
{
	Vector3	Pos;
	Vector4	Color;

	VertexColor()
	{
	}

	VertexColor(const Vector3& _Pos, const Vector4& _Color)	:
		Pos(_Pos),
		Color(_Color)
	{
	}
};


struct VertexBuffer
{
	ID3D11Buffer* Buffer;
	int		Size;
	int		Count;
	void* Data;

	VertexBuffer() :
		Buffer(nullptr),
		Size(0),
		Count(0),
		Data(nullptr)
	{
	}

	~VertexBuffer()
	{
		SAFE_DELETE_ARRAY(Data);
		SAFE_RELEASE(Buffer);
	}
};

struct IndexBuffer
{
	ID3D11Buffer* Buffer;
	int		Size;
	int		Count;
	DXGI_FORMAT	Fmt;
	void* Data;

	IndexBuffer() :
		Buffer(nullptr),
		Size(0),
		Count(0),
		Fmt(DXGI_FORMAT_UNKNOWN),
		Data(nullptr)
	{
	}

	~IndexBuffer()
	{
		SAFE_DELETE_ARRAY(Data);
		SAFE_RELEASE(Buffer);
	}
};



struct MeshContainer
{
	VertexBuffer	VB;
	std::vector<IndexBuffer>	vecIB;
	D3D11_PRIMITIVE_TOPOLOGY	Primitive;
};

struct MeshSlot
{
	VertexBuffer* VB;
	IndexBuffer* IB;
	D3D11_PRIMITIVE_TOPOLOGY	Primitive;

	MeshSlot() :
		VB(nullptr),
		IB(nullptr)
	{
	}
};


struct TransformCBuffer
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matInvProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
	Matrix	matInvVP;
	Matrix	matInvWVP;
	Vector3	Pivot;
	float	Empty1;
	Vector3	MeshSize;
	float	Empty2;
};

struct VertexUV
{
	Vector3	Pos;
	Vector2 UV;

	VertexUV()
	{
	}

	VertexUV(const Vector3& _Pos, const Vector2& _UV) :
		Pos(_Pos),
		UV(_UV)
	{
	}
};

struct MaterialCBuffer
{
	Vector4	BaseColor;
	Vector4	AmbientColor;
	Vector4	SpecularColor;
	Vector4	EmissiveColor;
	float	Opacity;
	int		PaperBurnEnable;
	int		BumpEnable;
	int		Animation3DEnable;
	int		SpecularTex;
	int		EmissiveTex;
	int		ReceiveDecal;
	int		Metallic;
	Vector2 UVScale;
	Vector2 Empty;
};

struct AnimationFrameData
{
	Vector2	Start;
	Vector2	Size;
};

struct Animation2DCBuffer
{
	Vector2	Animation2DStartUV;
	Vector2	Animation2DEndUV;
	int		Animation2DType;
	Vector3	Animation2DEmpty;
};

struct Standard2DCBuffer
{
	int		AnimationEnable;
	Vector3	Empty;
};

struct FindComponentName
{
	std::string	Name;
	std::string	ParentName;
};

struct CollisionProfile
{
	std::string			Name;
	Collision_Channel	Channel;
	bool				CollisionEnable;

	std::vector<Collision_Interaction>	vecInteraction;
};

struct CollisionResult
{
	class CColliderComponent* Src;
	class CColliderComponent* Dest;
	Vector3	HitPoint;

	CollisionResult()	:
		Src(nullptr),
		Dest(nullptr)
	{
	}
};

struct Box2DInfo
{
	Vector2	Center;
	Vector2	Axis[2];
	Vector2	Length;
	Vector2	Min;
	Vector2	Max;
};

struct Box3DInfo
{
	Vector3 Center;
	Vector3	Axis[3];
	float	AxisLen[3];
	Vector3	Min;
	Vector3	Max;
};

struct HalfLineInfo
{
	Vector3 StartPos;
	Vector3 EndPos;
	//Vector3 Direction;
};

struct CircleInfo
{
	Vector2	Center;
	float	Radius;
	Vector2	Min;
	Vector2	Max;
};

struct PixelInfo
{
	unsigned char* Pixel;
	unsigned int	Width;
	unsigned int	Height;
	PixelCollision_Type	Type;
	unsigned char	Color[4];
	Box2DInfo		Box;
	Vector2	Min;
	Vector2	Max;
	ID3D11ShaderResourceView* SRV;
	int		RefCount;

	PixelInfo()	:
		RefCount(1),
		Pixel(nullptr),
		SRV(nullptr),
		Width(0),
		Height(0),
		Box{},
		Color{},
		Type(PixelCollision_Type::Color_Confirm)
	{
	}
};

struct ColliderCBuffer
{
	Vector4	Color;
	Matrix	matWVP;
};

struct WidgetCBuffer
{
	Vector4	Tint;
	Matrix	matWP;
	int		UseTexture;
	int		WidgetAnimType;
	Vector2	WidgetAnimStartUV;
	Vector2	WidgetAnimEndUV;
	int		WidgetAnimEnable;
	float	Opacity;
};

struct ProgressBarCBuffer
{
	float	Percent;
	int		Dir;
	Vector2	Empty;
};

struct GlobalCBuffer
{
	float	DeltaTime;
	float	AccTime;
	Vector2	Resolution;
	Vector2	NoiseResolution;
	Vector2	Empty;
};

struct PaperBurnCBuffer
{
	Vector4	InLineColor;
	Vector4	OutLineColor;
	Vector4	CenterLineColor;
	float	Filter;
	int		Inverse;
	float	InFilter;
	float	OutFilter;
	float	CenterFilter;
	Vector3	Empty;
};

struct TileMapCBuffer
{
	Vector2 ImageSize;
	Vector2 Size;
	Vector2 Start;
	Vector2 End;
	Matrix  matWVP;
};

struct TileInfo
{
	Matrix matWVP;
	Vector2 TileStart;
	Vector2 TileEnd;
	Vector4 TileColor;
	float Opacity;
	Vector3 Empty;
};

// Paper Burn을 이용하는 물체들은 Instancing에서 제외시켜준다.
// 별도로 출력을 해주는 것이다.
struct InstancingInfo
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;
	Matrix	matWV;
	Matrix	matWVP;
	Matrix	matVP;
	Vector3	Pivot;
	float	Opacity;
	Vector3	MeshSize;
	int		PaperBurnEnable;
	Vector4	BaseColor;
	Vector2	Animation2DStartUV;
	Vector2	Animation2DEndUV;
	int		AnimEnable;
	Vector3	Empty;
};

struct NavWorkData
{
	std::function<void(const std::list<Vector3>&)>	Callback;
	class CSceneComponent*	OwnerComponent;
	//Vector3		Start;
	Vector3		End;
};

struct NavResultData
{
	std::function<void(const std::list<Vector3>&)>	Callback;
	std::list<Vector3>	vecPath;
};

struct Vertex3D
{
	Vector3	Pos;
	Vector3	Normal;
	Vector2	UV;
	Vector3	Tangent;
	Vector3	Binormal;
	Vector4	BlendWeight;
	Vector4	BlendIndex;
};


struct AnimationCBuffer
{
	int	BoneCount;
	int	CurrentFrame;
	int	NextFrame;
	float	Ratio;
	int	FrameCount;
	int	RowIndex;
	int	ChangeAnimation;
	float	ChangeRatio;
	int		ChangeFrameCount;
	Vector3	Empty;
};


struct LightCBuffer
{
	Vector4	Color;
	int	LightType;
	Vector3	Pos;
	Vector3	Dir;	
	float	Distance;
	float	AngleIn;
	float	AngleOut;
	float	Att1;
	float	Att2;
	float	Att3;
	Vector3	Empty;
};

struct LandScapeCBuffer
{
	float DetailLevel;
	int SplatCount;
	Vector2 Empty;
};

struct Ray
{
	Vector3 Pos;
	Vector3 Dir;
};

struct SphereInfo
{
	Vector3 Center;
	float Radius;
	Vector3 Min;
	Vector3 Max;
};

struct Instancing3DInfo
{
	Matrix matWV;
	Matrix matWVP;
	Vector4 MtrlBaseColor;
	Vector4 MtrlAmbientColor;
	Vector4 MtrlSpecularColor;
	Vector4 MtrlEmissiveColor;
	float MtrlOpacity;
	int MtrlPaperBurnEnable;
	int MtrlBumpEnable;
	int MtrlAnimation3DEnable;
	int MtrlSpecularTex;
	int MtrlEmissiveTex;
	int MtrlReceiveDecal;
	float PaperBurnFilter;
	int PaperBurnInverse;
	float PaperBurnInFilter;
	float PaperBurnOutFilter;
	float PaperBurnCenterFilter;
	Vector4 PaperBurnInLineColor;
	Vector4 PaperBurnOutLineColor;
	Vector4 PaperBurnCenterLineColor;
	int MtrlMetallic;
	Vector2 UVScale;
	float	Empty;
};

struct LightForwardCBuffer
{
	int LightCount;
	Vector3 Empty;
};

struct InstancingCBuffer
{
	int InstancingBoneCount;
	int MaterialIndex;
	int ObjectCount;
	float Empty;
};

 //struct OutlineCBuffer
 //{
 //	float DepthMultiplier;
 //	float DepthBias;
 //	float NormalMultiplier;
 //	float NormalBias;
 //};

//struct NotifyParameter
//{
//	CGameObject* SrcObject;	// 애니메이션 동작하는 본인
//	CGameObject* TargetObject; // Notify에서 대상이 되는 Object(ex. StateManager::GiveDamage에서 데미지를 줘야 할 대상)
//	float Range;
//	int Amount;
//};

struct ObjectData
{
	int HP;
	float DetectRange;
	float MoveSpeed;
	int Attack;
	float MeleeAttackRange;
	float JumpAttackRange;
	float RotateSpeedPerSec;
	float FlySpeed;
};

struct DownScaleCBuffer
{
	Resolution		RS;			// 다운스케일 해상도 (1 / 16)
	unsigned int	Domain;		// 다운스케일 이미지의 총 픽셀 수
	unsigned int	GroupSize;	// 첫 패스에 적용된 그룹 수 계산
	float			Adaptation; // 적응
	float			BloomThreshold;	// 블룸 임계값
	Vector2			Empty;
};

struct HDRRenderCBuffer
{
	float MiddleGray;
	float LumWhite;
	float BloomScale;
	float DOFMin;				// DOF 적용 최소 거리
	float DOFMax;				// 완전 초점 상실 거리
	Vector3 Empty;
};

struct FogCBuffer
{
	Vector3 Color;
	int Type;
	float Start; 
	float End;
	float Density;				
	float Empty;
};

struct PlayerData
{
	Player_Ability Abilty_Type;
	float AttackSpeed;
	float MagicAttack;
};
struct ShaderParams
{
	size_t				Type;
	ToonShaderType		ToonType;
};

struct ToonCBuffer
{
	int Type;
	Vector3 Empty;
};

struct GlobalLightCBuffer
{
	float AmbientIntensity;
	Vector3 Empty;
};

struct HDRSceneSaveData
{
	float AdaptationTime;
	float MiddleGray;
	float LumWhite;
	float BloomTreshold;
	float BloomScale;
	float DOFMax;
	float DOFMin;
	Fog_Type FogType;
	Vector3 FogColor;
	float FogStart;
	float FogEnd;
	float FogDensity;

	HDRSceneSaveData() :
		AdaptationTime(3.f),
		MiddleGray(1.f),
		LumWhite(1.1f),
		BloomTreshold(1.f),
		BloomScale(1.f),
		DOFMax(600.f),
		DOFMin(300.f),
		FogType(Fog_Type::Depth),
		FogColor(0.f, 0.f, 0.f),
		FogStart(500.f),
		FogEnd(950.f),
		FogDensity(1.f)
	{
	}
};

struct GlobalLightSceneSaveData
{
	Vector3 Rot;
	Vector3 Color;
	float AmbientIntensity;

	GlobalLightSceneSaveData()	:
		Rot(45.f, 90.f, 0.f),
		Color(1.f, 1.f, 1.f),
		AmbientIntensity(1.f)
	{
	}
};

struct BackGroundSceneSaveData
{
	bool RenderSkyBox;
	std::string SkyBoxFileName;
	Vector4 ClearColor;
	std::string PrevMusicKeyName;
	std::string MusicKeyName;
};

struct SceneSaveGlobalData
{
	HDRSceneSaveData			HDRData;
	GlobalLightSceneSaveData	GLightData;
	BackGroundSceneSaveData		BackGroundData;
};

struct NavMeshPolygon
{
	std::vector<Vector3>		m_vecVertexPos;
	std::vector<Vector3>		m_vecVertexOriginPos;
	std::vector<int>		m_vecAdjIndex;
	int m_Index;
	Vector3 m_Min;
	Vector3 m_Max;
};

struct WaterCBuffer
{
	float Speed;
	float FoamDepthThreshold;
	float Attn1;
	float Attn2;
};

struct NavigationCell
{
	NAVIMESH_CELL_LIST_TYPE	Type;
	NavMeshPolygon Polygon;
	Vector3		Center;
	int			ParentIdx;
	float		G;
	float		H;
	float		Total;
	bool		Enable;

	void Clear()
	{
		Type = NCLT_NONE;
		ParentIdx = -1;
		G = -1.f;
		H = -1.f;
		Total = -1.f;
	}

	NavigationCell()
	{
		Type = NCLT_NONE;
		ParentIdx = -1;
		G = 0.f;
		H = 0.f;
		Total = 0.f;
		Enable = true;
	}

	NavigationCell operator + (const NavigationCell& cell)
	{
		NavigationCell	_cell;

		return _cell;
	}
};

struct FadeCBuffer
{
	int FadeStart;
	Vector3 StartColor;
	float Ratio;
	Vector3 EndColor;
};