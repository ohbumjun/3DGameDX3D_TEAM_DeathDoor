struct PSOutput_Single
{
	float4	Color : SV_TARGET;
};

struct PSOutput_GBuffer
{
    float4 Diffuse : SV_Target;
    float4 GBuffer1 : SV_Target1;
    float4 GBuffer2 : SV_Target2;
    float4 GBuffer3 : SV_Target3;
    float4 GBuffer4 : SV_Target4;
    float4 GBuffer5 : SV_Target5;
};

cbuffer Transform : register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
    matrix  g_matInvProj;
	matrix	g_matWV;
	matrix	g_matWVP;
	matrix	g_matVP;
	matrix	g_matInvVP;
    matrix g_matInvWVP;
	float3	g_Pivot;
	float	g_TransformEmpty1;
	float3	g_MeshSize;
    float	g_TransformEmpty2;
};

cbuffer Material : register(b1)
{
    float4  g_MtrlBaseColor;
    float4  g_MtrlAmbientColor;
    float4  g_MtrlSpecularColor;
    float4  g_MtrlEmissiveColor;
	float	g_MtrlOpacity;
    int		g_MtrlPaperBurnEnable;
    int		g_MtrlBumpEnable;
    int		g_MtrlAnimation3DEnable;
    int		g_MtrlSpecularTex;
    int		g_MtrlEmissiveTex;
	int		g_MtrlRecevieDecal;
	int		g_MtrlMetallic;
	float2	g_MtrlUVScale;
};

cbuffer Standard2D : register(b2)
{
	int		g_Animation2DEnable;
	float3	g_Standard2DEmpty;
};

cbuffer	GlobalCBuffer : register(b3)
{
	float	g_DeltaTime;
	float	g_AccTime;
	float2	g_Resolution;
	float2	g_NoiseResolution;
	float2	g_GlobalEmpty;
};

cbuffer PaperBurnCBuffer : register(b4)
{
	float4	g_PaperBurnInLineColor;
	float4	g_PaperBurnOutLineColor;
	float4	g_PaperBurnCenterLineColor;
	float	g_PaperBurnFilter;
	int		g_PaperBurnInverse;
	float	g_PaperBurnInFilter;
	float	g_PaperBurnOutFilter;
	float	g_PaperBurnCenterFilter;
	float3	g_PaperBurnEmpty;
};

cbuffer LightCBuffer : register(b5)
{
    float4 g_LightColor;
    int g_LightType;
    float3 g_LightPos;
    float3 g_LightDir;
    float g_LightDistance;
    float g_LightAngleIn;
    float g_LightAngleOut;
    float g_LightAtt1;
    float g_LightAtt2;
    float g_LightAtt3;
    float3 g_LightEmpty;
};

cbuffer InstancingCBuffer : register(b6)
{
	int g_InstancingBoneCount;
	int g_InstancingMaterialIndex;
	int g_InstancingObjecCount;
	float g_InstancingEmpty;
};

cbuffer DownScaleCBuffer : register(b10)
{
	uint2 g_DownScaleRS;
	uint g_DownScaleDomain;
	uint g_DownScaleGroupSize;
	float g_DownScaleAdpatation;
	float g_DownScaleBloomThreshold;
	float2 g_DownScaleEmpty;
}

cbuffer GLightCBuffer : register(b11)
{
	float g_GLightAmbIntensity;
	float3 g_GLightEmpty;
}

cbuffer FadeCBuffer : register(b8)
{
	int g_FadeStart;
	float3 g_FadeStartColor;
	float g_FadeRatio;
	float3 g_FadeEndColor;
}

struct LightResult
{
    float3 Dif;
    float3 Amb;
    float3 Spc;
    float3 Emv;
};

#define	LightTypeDir	0
#define	LightTypePoint	1
#define	LightTypeSpot	2

#define OutlineThickMax 20.f

// Light Blend전에 렌더되어있는 픽셀이 어떤 쉐이더를 거쳐서 온 것인가 판단하는 정수
#define Pixel_Type_Default 1
#define Pixel_Type_Toon 2

#define ToonType_Default	0
#define ToonType_Easy		1
#define ToonType_Light		2
#define ToonType_Warm		3

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

SamplerState	g_PointSmp : register(s0);
SamplerState	g_LinearSmp : register(s1);
SamplerState	g_AnisotropicSmp : register(s2);

SamplerState	g_BaseSmp : register(s3);

Texture2D g_BaseTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_SpecularTexture : register(t2);
Texture2D g_EmissiveTexture : register(t3);

Texture2D<float4>		g_NoiseTexture	: register(t100);
Texture2D				g_PaperBurnTexture	: register(t101);

StructuredBuffer<float>	g_RandBuffer	: register(t90);

TextureCube g_SkyTex : register(t20);

static const float2 g_NullPos[4] =
{
    float2(-1.f, 1.f),
    float2(1.f, 1.f),
    float2(-1.f, -1.f),
    float2(1.f, -1.f)
};

static const float2 g_NullUV[4] =
{
    float2(0.f, 0.f),
    float2(1.f, 0.f),
    float2(0.f, 1.f),
    float2(1.f, 1.f)
};

static float Gaussian5x5[25] =
{
	0.003765f, 0.015019f, 0.023792f, 0.015019f, 0.003765f,
	0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
	0.023792f, 0.094907f, 0.150342f, 0.094907f, 0.023792f,
	0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
	0.003765f, 0.015019f, 0.023792f, 0.015019f, 0.003765f
};

float GaussianSample(int2 UV, Texture2D Tex)
{
	float	Output = 0.f;

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			int2	ConvertUV = UV + int2(j - 2, i - 2);

			if (ConvertUV.x < 0)
				ConvertUV.x = 0;

			else if (ConvertUV.x > g_NoiseResolution.x)
				ConvertUV.x = g_NoiseResolution.x - 1;

			if (ConvertUV.y < 0)
				ConvertUV.y = 0;

			else if (ConvertUV.y > g_NoiseResolution.y)
				ConvertUV.y = g_NoiseResolution.y - 1;

			Output += Tex[ConvertUV].r * Gaussian5x5[i * 5 + j];
		}
	}

	return Output;
}

float Rand(float Key)
{
	float2	UV = float2(cos(Key + g_AccTime), sin(g_AccTime));
	
	UV = frac(UV);

	return GaussianSample(UV * g_NoiseResolution, g_NoiseTexture);
}

float DegreeToRadian(float Angle)
{
	return Angle / 180.f * 3.14159f;
}

float RadianToDegree(float Radian)
{
	return (Radian / 3.14159f) * 180.f;
}

// Radian (X), Degree 로 리턴한다.
float AngleBetweenTwoVector(float3 V1, float3 V2)
{
	float cosResult = dot(V1, V2);

	// acos 는 -1에서 1 사이의 값을 반환한다.
	float RadianRotAngle = acos(cosResult);

	// 주의할점 : cos210 -> cos 150과 동일
	// 실제 각도는 210도 인데도 불구하고 150 도를 리턴한다.
	float RotAngle = RadianToDegree(RadianRotAngle);

	return RotAngle;
}

float4 PaperBurn2DInstancing(float4 Color, float2 UV, int Inverse, float Filter, float InFilter, float CenterFilter,
		float OutFilter, float4 InColor, float4 CenterColor, float4 OutColor)
{
    if (Color.a == 0.f)
		return Color;

    float4 BurnColor = g_PaperBurnTexture.Sample(g_LinearSmp, UV);

	float4	result = Color;

	if (Inverse == 0)
	{
		if (Filter >= BurnColor.r)
		{
			clip(-1);
		}

		else
		{
			if (Filter - OutFilter <= BurnColor.r &&
				BurnColor.r <= Filter + OutFilter)
				result = OutColor;

			if (Filter - CenterFilter <= BurnColor.r &&
				BurnColor.r <= Filter + CenterFilter)
				result = CenterColor;

			if (Filter - InFilter <= BurnColor.r &&
				BurnColor.r <= Filter + InFilter)
				result = InColor;
		}
	}

	else
	{
		if (Filter < BurnColor.r)
		{
			clip(-1);
		}

		else
		{
			if (Filter - OutFilter <= BurnColor.r &&
				BurnColor.r <= Filter + OutFilter)
				result = OutColor;

			if (Filter - CenterFilter <= BurnColor.r &&
				BurnColor.r <= Filter + CenterFilter)
				result = CenterColor;

			if (Filter - InFilter <= BurnColor.r &&
				BurnColor.r <= Filter + InFilter)
				result = InColor;
		}
	}

	result.a *= Color.a;

	return result;
}

float4 PaperBurn2D(float4 Color, float2 UV)
{
	if (g_MtrlPaperBurnEnable == 0)
		return Color;
	
    else if (Color.a == 0.f)
        return Color;

    float4 BurnColor = g_PaperBurnTexture.Sample(g_LinearSmp, UV);

	float4	result = Color;

	if (g_PaperBurnInverse == 0)
	{
		if (g_PaperBurnFilter >= BurnColor.r)
		{
			clip(-1);
		}

		else
		{
			if (g_PaperBurnFilter - g_PaperBurnOutFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnOutFilter)
				result = g_PaperBurnOutLineColor;

			if (g_PaperBurnFilter - g_PaperBurnCenterFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnCenterFilter)
				result = g_PaperBurnCenterLineColor;

			if (g_PaperBurnFilter - g_PaperBurnInFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnInFilter)
				result = g_PaperBurnInLineColor;
		}
	}

	else
	{
		if (g_PaperBurnFilter < BurnColor.r)
		{
			clip(-1);
		}

		else
		{
			if (g_PaperBurnFilter - g_PaperBurnOutFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnOutFilter)
				result = g_PaperBurnOutLineColor;

			if (g_PaperBurnFilter - g_PaperBurnCenterFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnCenterFilter)
				result = g_PaperBurnCenterLineColor;

			if (g_PaperBurnFilter - g_PaperBurnInFilter <= BurnColor.r &&
				BurnColor.r <= g_PaperBurnFilter + g_PaperBurnInFilter)
				result = g_PaperBurnInLineColor;
		}
	}

	result.a *= Color.a;

	return result;
}


float3 ComputeBumpNormal(float3 Normal, float3 Tangent, float3 Binormal,
	float2 UV)
{
    float3 result = Normal;
	
    if (g_MtrlBumpEnable == 1)
    {
        float4 NormalColor = g_NormalTexture.Sample(g_BaseSmp, UV);
		
		// 색상은 0 ~ 1 사이이므로 -1 ~ 1 사이의 법선벡터로 만들어준다.
        float3 ConvertNormal = NormalColor.xyz * 2.f - 1.f;

		// z는 무조건 + 방향으로 만들어준다.
        ConvertNormal.z = 1.f;
        ConvertNormal = normalize(ConvertNormal);
		
		// Tangent, Binormal, Normal을 이용해서 위에서 만들어준 법선
		// 벡터를 뷰공간으로 변환해준다. 뷰공간이 되는 이유는 Tangent,
		// Binormal, Normal 3개 모두 뷰공간으로 변환된 벡터가 들어오기
		// 때문이다.
        float3x3 mat =
        {
			Tangent,
			Binormal,
			Normal
        };

        result = normalize(mul(ConvertNormal, mat));
    }
	
    return result;
}

float ConvertColor(float4 Color)
{
    uint4 Convert = (uint4) 0;
	
    Convert.r = uint(Color.r * 255);
    Convert.g = uint(Color.g * 255);
    Convert.b = uint(Color.b * 255);
    Convert.a = uint(Color.a * 255);

    uint Result = 0;
	
    Result = Convert.a;
    Result = (Result << 8) | Convert.r;
    Result = (Result << 8) | Convert.g;
    Result = (Result << 8) | Convert.b;
	
    return asfloat(Result);
}

float4 ConvertColor(float Color)
{
    uint Convert = asuint(Color);

    float4 Result;
    Result.b = (Convert & 0x000000ff) / 255.f;
    Result.g = ((Convert >> 8) & 0x000000ff) / 255.f;
    Result.r = ((Convert >> 16) & 0x000000ff) / 255.f;
    Result.a = ((Convert >> 24) & 0x000000ff) / 255.f;

    return Result;

}

float GetRandomNumber(float key)
{
	float3	RandomPos = float3(Rand(key), Rand(2.142f), Rand(key * 3.f));
	float	Rand = (RandomPos.x + RandomPos.y + RandomPos.z) / 3.f;
	return Rand;
}
