#define PARTICLE_INIT_ANGLE 1000

cbuffer	ParticleCBuffer : register(b7)
{
	uint	g_ParticleSpawnEnable;	// 파티클 생성여부
	float3	g_ParticleStartMin;		// 파티클이 생성될 영역의 Min
	float3	g_ParticleStartMax;		// 파티클이 생성될 영역의 Max
	uint	g_ParticleSpawnCountMax;	// 생성될 파티클의 최대

	float3	g_ParticleScaleMin;		// 생성될 파티클 크기의 Min
	float	g_ParticleLifeTimeMin;	// 생성될 파티클이 살아있을 최소시간

	float3	g_ParticleScaleMax;		// 새성될 파티클 크기의 Max
	float	g_ParticleLifeTimeMax;	// 생성될 파티클이 살아있을 최대시간

	float4	g_ParticleColorMin;		// 생성될 파티클의 색상 Min
	float4	g_ParticleColorMax;		// 생성될 파티클의 색상 Max

	float	g_ParticleSpeedStart;		// 파티클의 최소 이동속도
	float	g_ParticleSpeedEnd;		// 파티클의 최대 이동속도
	int		g_ParticleMove;			// 이동을 하는지 안하는지
	int		g_ParticleGravity;		// 중력 적용을 받는지 안받는지

	float3	g_ParticleMoveDir;		// 이동을 한다면 기준이 될 이동 방향
	// int		g_Particle2D;			// 2D용 파티클인지
	int		g_DestroyAllExistingLivingParticles;			// 2D용 파티클인지

	float3	g_ParticleMoveAngle;	// 이동을 한다면 기준이 될 방향으로부터 x, y, z 에 저장된 각도만큼 틀어진 랜덤한 방향을 구한다.
	int      g_ParticleBounce;

	float3 g_ParticleRotationAngle;
	float   g_ParticleBounceResist;

	int g_ParticleGenerateShapeType;
	float g_ParcticleGenerateRadius;
	int g_LoopGenerateRing;
	int g_ParticleSpecialMoveDirType;

	float g_ParticleAlphaEnd;
	float g_ParticleAlphaStart;
	int g_ParticleLifeTimeLinear;
	// int g_RotAccordingToDir; // 자신의 진행 방향 대로 회전시킬 것인가. g_ParticleSpecialMoveDirType 을 지정해야 한다. 회전 중심 축을 잡아야 하므로
	int g_SeperateLinerRotate; // 자신의 진행 방향 대로 회전시킬 것인가. g_ParticleSpecialMoveDirType 을 지정해야 한다. 회전 중심 축을 잡아야 하므로

	int g_ParticleUVMoveEnable;
	int g_ParticleUVRowN;
	int g_ParticleUVColN;
	// int g_ParticleResetSharedInfoSpawnCntSum;
	int g_ParticleApplyLinearEmissiveChange;

	// 각 Particle 별로 다르게 Rotation Angle을 주는 경우
	float3 g_ParticleSeperateRotAngleMin;
	int g_ParticleDisableNewAlive;

	float3 g_ParticleSeperateRotAngleMax;
	int g_ParticleAlphaLinearFromCenter;

	// Particle Component Relative Scale
	// float3 g_ParticleCommonWorldScale;
	float3 g_ParticleStartEmissiveColor;
	int g_ParticleLinearUVClipping;

	// Particle Component WorldPos
	// float3 g_ParticleComponentWorldPos;
	float3 g_ParticleEndEmissiveColor;
	int g_ParticleSpeedChangeMethod;

	int g_ParticleApplyNoiseTexture; // Pixel Shader 에서 매순간 Noise Texture 로 부터, Sampling 을 해서 Color, Alpha 값 등을 바꾸는 것
	float g_ParticleNoiseTextureApplyRatio; // Noise Texture 사라지는 효과 시작 비율
	int g_ParticleLinearClippingReverseDir;
	int g_ParticleFollowComponentPos;

	// 여기 아래는 Particle Constant Buffer 의 크기 제한으로 인해 안먹힐 것이다.
	float3 ParticleEmpty1;
	int ParticleEmpty3;
	// 기존에 살아있는 Particle 들을 모두 Alive False 로 만들어주기 
	float3 ParticleEmpty2;
	float ParticleEmpty4;
};

cbuffer	ParticleTempElemCBuffer : register(b9)
{
	// float3 g_CompRelativeAxis;
	// TV : Temporary Value
	float3 g_TVParticleCommonWorldScale;
	int g_TVResetParticleSharedInfoSumSpawnCnt;

	float3 g_TVParticleComponentWorldPos;
	int ParticleTempValEmpty1;

	float3 g_TVParticleRotatedAxisX;
	int ParticleTempEmpty2;

	float3 g_TVParticleRotatedAxisY;
	int ParticleTempEmpty3;

	float3 g_TVParticleRotatedAxisZ;
	int ParticleTempEmpty4;
}

struct ParticleInfo
{
	// Particle Component 의 Local Space 상에서의 위치 
	float3	LocalPos;
	float3	Dir;
	float	Speed;
	float	LifeTime;

	float	LifeTimeMax;
	int		Alive;
	float	FallTime;
	float	FallStartY;

	// 각 Particle 의 처음 Alpha 시작 값
	float AlphaDistinctStart;

	// 1) Circle, Ring 등 Particle Shape
	// 2) Linear Rot 때 세팅되는 값 
	float CurrentParticleAngle;

	float3 SeperateRotAngleOffset;
	float3 FinalSeperateRotAngle;

	float  InitLocalPosY;
	float3 InitParticleComponentWorldPos;

	float DestroyedDuringAlive;
	float SingleInfoEmpty1;
	float SingleInfoEmpty2;
	float3 SingleEmptyInfo2;
};

struct ParticleInfoShared
{
	uint	SpawnEnable;
	uint	SpawnCountMax;

	uint CurrentSpawnCountSum;
	int DisableNewAlive;

	float3	ScaleMin;
	float3	ScaleMax;

	float4	ColorStart;
	float4	ColorEnd;

	float3 CommonWorldScale;

	int		GravityEnable;

	float3 RotationAngle;
	float3  SeperateMinRotAngle;
	float3  SeperateMaxRotAngle;

	float3  ParticleComponentWorldPos;

	float MaxDistFromCenter;

	float PrevRingAngle;

	int UVMoveEnable;
	int UVRowN;
	int UVColN;

	float3 ShareEmptyInfo1;
	float3 ShareEmptyInfo2;
};

float3x3 ComputeRotationMatrix(float3 Angle)
{
	float3	ConvertAngle;
	ConvertAngle.x = Angle.x / 180.f * 3.14159f;
	ConvertAngle.y = Angle.y / 180.f * 3.14159f;
	ConvertAngle.z = Angle.z / 180.f * 3.14159f;

	float3x3	matRotX, matRotY, matRotZ, matRot;

	// 11, 12, 13
	// 21, 22, 23
	// 31, 32, 33
	matRotX._11 = 1.f;
	matRotX._21 = 0.f;
	matRotX._31 = 0.f;

	matRotX._12 = 0.f;
	matRotX._22 = cos(ConvertAngle.x);
	matRotX._32 = -sin(ConvertAngle.x);

	matRotX._13 = 0.f;
	matRotX._23 = sin(ConvertAngle.x);
	matRotX._33 = cos(ConvertAngle.x);


	matRotY._11 = cos(ConvertAngle.y);
	matRotY._21 = 0.f;
	matRotY._31 = -sin(ConvertAngle.y);

	matRotY._12 = 0.f;
	matRotY._22 = 1.f;
	matRotY._32 = 0.f;

	matRotY._13 = sin(ConvertAngle.y);
	matRotY._23 = 0.f;
	matRotY._33 = cos(ConvertAngle.y);


	matRotZ._11 = cos(ConvertAngle.z);
	matRotZ._21 = -sin(ConvertAngle.z);
	matRotZ._31 = 0.f;

	matRotZ._12 = sin(ConvertAngle.z);
	matRotZ._22 = cos(ConvertAngle.z);
	matRotZ._32 = 0.f;

	matRotZ._13 = 0.f;
	matRotZ._23 = 0.f;
	matRotZ._33 = 1.f;

	matRot = mul(matRotX, matRotY);
	matRot = mul(matRot, matRotZ);

	return matRot;
}

float GetRandValForParticle(float2 co) {
	return(frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 1;
}

// Rotation : 각도
void RotateAboutAxisWithDegrees(float3 In, float3 Axis, float Rotation, out float3 Out)
{
	Rotation = radians(Rotation);
	float s = sin(Rotation);
	float c = cos(Rotation);
	float one_minus_c = 1.0 - c;

	Axis = normalize(Axis);

	// DX 의 경우, Transpose 를 시켜줘야 한다.
	// 좌표계가 다르기 때문이다.
	// float3x3 rot_mat =
	// { one_minus_c * Axis.x * Axis.x + c, one_minus_c * Axis.x * Axis.y - Axis.z * s, one_minus_c * Axis.z * Axis.x + Axis.y * s,
	// 	one_minus_c * Axis.x * Axis.y + Axis.z * s, one_minus_c * Axis.y * Axis.y + c, one_minus_c * Axis.y * Axis.z - Axis.x * s,
	// 	one_minus_c * Axis.z * Axis.x - Axis.y * s, one_minus_c * Axis.y * Axis.z + Axis.x * s, one_minus_c * Axis.z * Axis.z + c
	// };

	float3x3 rot_mat =
	{ one_minus_c * Axis.x * Axis.x + c,				one_minus_c * Axis.x * Axis.y + Axis.z * s,			one_minus_c * Axis.z * Axis.x - Axis.y * s,
		one_minus_c* Axis.x* Axis.y - Axis.z * s,		one_minus_c * Axis.y * Axis.y + c,					one_minus_c* Axis.y* Axis.z + Axis.x * s,
		one_minus_c* Axis.z* Axis.x + Axis.y * s,	one_minus_c* Axis.y* Axis.z - Axis.x * s,		one_minus_c * Axis.z * Axis.z + c
	};

	Out = mul(rot_mat, In);
}

// Rotation : 각도
void GetRotationMatrixAboutAxisWithDegrees(float3 Axis, float Rotation, out float3x3 Out)
{
	Rotation = radians(Rotation);
	float s = sin(Rotation);
	float c = cos(Rotation);
	float one_minus_c = 1.0 - c;

	Axis = normalize(Axis);

	// DX 의 경우, Transpose 를 시켜줘야 한다.
	// 좌표계가 다르기 때문이다.
	// float3x3 rot_mat =
	// { one_minus_c * Axis.x * Axis.x + c, one_minus_c * Axis.x * Axis.y - Axis.z * s, one_minus_c * Axis.z * Axis.x + Axis.y * s,
	// 	one_minus_c * Axis.x * Axis.y + Axis.z * s, one_minus_c * Axis.y * Axis.y + c, one_minus_c * Axis.y * Axis.z - Axis.x * s,
	// 	one_minus_c * Axis.z * Axis.x - Axis.y * s, one_minus_c * Axis.y * Axis.z + Axis.x * s, one_minus_c * Axis.z * Axis.z + c
	// };

	float3x3 rotMat = { one_minus_c * Axis.x * Axis.x + c,				one_minus_c * Axis.x * Axis.y + Axis.z * s,			one_minus_c * Axis.z * Axis.x - Axis.y * s,
		one_minus_c * Axis.x * Axis.y - Axis.z * s,		one_minus_c * Axis.y * Axis.y + c,					one_minus_c * Axis.y * Axis.z + Axis.x * s,
		one_minus_c * Axis.z * Axis.x + Axis.y * s,	one_minus_c * Axis.y * Axis.z - Axis.x * s,		one_minus_c * Axis.z * Axis.z + c
	};

	Out = rotMat;

	// Out = mul(rot_mat, In);
}