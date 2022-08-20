#define PARTICLE_INIT_ANGLE 1000

cbuffer	ParticleCBuffer : register(b7)
{
	uint	g_ParticleSpawnEnable;	// ��ƼŬ ��������
	float3	g_ParticleStartMin;		// ��ƼŬ�� ������ ������ Min
	float3	g_ParticleStartMax;		// ��ƼŬ�� ������ ������ Max
	uint	g_ParticleSpawnCountMax;	// ������ ��ƼŬ�� �ִ�

	float3	g_ParticleScaleMin;		// ������ ��ƼŬ ũ���� Min
	float	g_ParticleLifeTimeMin;	// ������ ��ƼŬ�� ������� �ּҽð�

	float3	g_ParticleScaleMax;		// ������ ��ƼŬ ũ���� Max
	float	g_ParticleLifeTimeMax;	// ������ ��ƼŬ�� ������� �ִ�ð�

	float4	g_ParticleColorMin;		// ������ ��ƼŬ�� ���� Min
	float4	g_ParticleColorMax;		// ������ ��ƼŬ�� ���� Max

	float	g_ParticleSpeedStart;		// ��ƼŬ�� �ּ� �̵��ӵ�
	float	g_ParticleSpeedEnd;		// ��ƼŬ�� �ִ� �̵��ӵ�
	int		g_ParticleMove;			// �̵��� �ϴ��� ���ϴ���
	int		g_ParticleGravity;		// �߷� ������ �޴��� �ȹ޴���

	float3	g_ParticleMoveDir;		// �̵��� �Ѵٸ� ������ �� �̵� ����
	// int		g_Particle2D;			// 2D�� ��ƼŬ����
	int		g_DestroyAllExistingLivingParticles;			// 2D�� ��ƼŬ����

	float3	g_ParticleMoveAngle;	// �̵��� �Ѵٸ� ������ �� �������κ��� x, y, z �� ����� ������ŭ Ʋ���� ������ ������ ���Ѵ�.
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
	// int g_RotAccordingToDir; // �ڽ��� ���� ���� ��� ȸ����ų ���ΰ�. g_ParticleSpecialMoveDirType �� �����ؾ� �Ѵ�. ȸ�� �߽� ���� ��ƾ� �ϹǷ�
	int g_SeperateLinerRotate; // �ڽ��� ���� ���� ��� ȸ����ų ���ΰ�. g_ParticleSpecialMoveDirType �� �����ؾ� �Ѵ�. ȸ�� �߽� ���� ��ƾ� �ϹǷ�

	int g_ParticleUVMoveEnable;
	int g_ParticleUVRowN;
	int g_ParticleUVColN;
	// int g_ParticleResetSharedInfoSpawnCntSum;
	int g_ParticleApplyLinearEmissiveChange;

	// �� Particle ���� �ٸ��� Rotation Angle�� �ִ� ���
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

	int g_ParticleApplyNoiseTexture; // Pixel Shader ���� �ż��� Noise Texture �� ����, Sampling �� �ؼ� Color, Alpha �� ���� �ٲٴ� ��
	float g_ParticleNoiseTextureApplyRatio; // Noise Texture ������� ȿ�� ���� ����
	int g_ParticleLinearClippingReverseDir;
	int g_ParticleFollowComponentPos;

	// ���� �Ʒ��� Particle Constant Buffer �� ũ�� �������� ���� �ȸ��� ���̴�.
	float3 ParticleEmpty1;
	int ParticleEmpty3;
	// ������ ����ִ� Particle ���� ��� Alive False �� ������ֱ� 
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
	// Particle Component �� Local Space �󿡼��� ��ġ 
	float3	LocalPos;
	float3	Dir;
	float	Speed;
	float	LifeTime;

	float	LifeTimeMax;
	int		Alive;
	float	FallTime;
	float	FallStartY;

	// �� Particle �� ó�� Alpha ���� ��
	float AlphaDistinctStart;

	// 1) Circle, Ring �� Particle Shape
	// 2) Linear Rot �� ���õǴ� �� 
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

// Rotation : ����
void RotateAboutAxisWithDegrees(float3 In, float3 Axis, float Rotation, out float3 Out)
{
	Rotation = radians(Rotation);
	float s = sin(Rotation);
	float c = cos(Rotation);
	float one_minus_c = 1.0 - c;

	Axis = normalize(Axis);

	// DX �� ���, Transpose �� ������� �Ѵ�.
	// ��ǥ�谡 �ٸ��� �����̴�.
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

// Rotation : ����
void GetRotationMatrixAboutAxisWithDegrees(float3 Axis, float Rotation, out float3x3 Out)
{
	Rotation = radians(Rotation);
	float s = sin(Rotation);
	float c = cos(Rotation);
	float one_minus_c = 1.0 - c;

	Axis = normalize(Axis);

	// DX �� ���, Transpose �� ������� �Ѵ�.
	// ��ǥ�谡 �ٸ��� �����̴�.
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