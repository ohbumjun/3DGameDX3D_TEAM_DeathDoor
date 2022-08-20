#include "ShaderInfo.fx"

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
	int		g_Particle2D;			// 2D�� ��ƼŬ����

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
	int g_SeperateLinerRotate; // �ڽ��� ���� ���� ��� ȸ����ų ���ΰ�. g_ParticleSpecialMoveDirType �� �����ؾ� �Ѵ�. ȸ�� �߽� ���� ��ƾ� �ϹǷ�

	int g_ParticleUVMoveEnable;
	int g_ParticleUVRowN;
	int g_ParticleUVColN;
	int g_ParticleResetSharedInfoSpawnCntSum;

	// �� Particle ���� �ٸ��� Rotation Angle�� �ִ� ���
	float3 g_ParticleSeperateRotAngleMin;
	int g_ParticleDisableNewAlive;

	float3 g_ParticleSeperateRotAngleMax;
	int g_ParticleAlphaLinearFromCenter;

	// Particle Component Relative Scale
	float3 g_ParticleCommonWorldScale;
	int g_ParticleUVClippingReflectingMoveDir;

	// Particle Component WorldPos
	float3 g_ParticleComponentWorldPos;
	int g_ParticleSpeedChangeMethod;

	int g_ParticleApplyNoiseTexture; // Pixel Shader ���� �ż��� Noise Texture �� ����, Sampling �� �ؼ� Color, Alpha �� ���� �ٲٴ� ��
	float g_ParticleNoiseTextureApplyRatio; // Noise Texture ������� ȿ�� ���� ����
	float g_ParticleExponentialAndLogFunctionXIntercept;
	int g_ParticleFollowComponentPos;

	float3 g_ParticleEmptyInfo1;
	float3 g_ParticleEmptyInfo2;
};

#define	GRAVITY	9.8f

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

	float3 SingleEmptyInfo1;
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

RWStructuredBuffer<ParticleInfo>		g_ParticleArray	: register(u0);
RWStructuredBuffer<ParticleInfoShared> g_ParticleShare : register(u1);

StructuredBuffer<float>		g_ParticleNormalDistValArray	: register(t20);

Texture2DMS<float4> g_GBufferDepth : register(t10);


float3x3 ComputeRotationMatrix(float3 Angle)
{
	float3	ConvertAngle;
	ConvertAngle.x = DegreeToRadian(Angle.x);
	ConvertAngle.y = DegreeToRadian(Angle.y);
	ConvertAngle.z = DegreeToRadian(Angle.z);

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


void ApplyInitSpecialParticleGenerateShape(float RandomAngle, int ThreadID, float FinalAppliedRadius,
	float Rand)
{
	switch (g_ParticleGenerateShapeType)
	{
		// Ring ��� ���� => Generate Circle �� �ϰ� �Ǹ�, Start Min, Max �� ���ǹ������� �Ǵ� ���̴�.
		// 0.f, 0.f, 0.f �߽����� ���� �����ϱ�
	case 0:
	{
		float CurrentRingAngle = RandomAngle;

		float3 RingPos = float3(0.f, 0.f, 0.f) + float3(
			cos(CurrentRingAngle) * FinalAppliedRadius,
			0.f,
			sin(CurrentRingAngle) * FinalAppliedRadius);

		g_ParticleArray[ThreadID].LocalPos = RingPos;

		// Loop �� �����ϰ� �Ǹ�, ���ʴ�� ��������� �Ѵ�.
		if (g_LoopGenerateRing == 1)
		{
			CurrentRingAngle = g_ParticleShare[0].PrevRingAngle + (360.f / g_ParticleSpawnCountMax);

			if (CurrentRingAngle > 360.f)
				CurrentRingAngle -= 360.f;

			RingPos = float3(0.f, 0.f, 0.f) + float3(cos(CurrentRingAngle) * FinalAppliedRadius,
				0.f,
				sin(CurrentRingAngle) * FinalAppliedRadius);

			g_ParticleArray[ThreadID].LocalPos = RingPos;

			g_ParticleShare[0].PrevRingAngle = CurrentRingAngle;

			// ���� Frame ���� �ش� Particle �� ���� ���� (�߽� ����)
			g_ParticleArray[ThreadID].CurrentParticleAngle = CurrentRingAngle;
		}
	}
	break;
	// Circle
	// ���� ���� �� �ȿ��� �����ǰ� �ϱ� (Ring �� ���� ���� ���ݿ��� ���� X. �� �ȿ� ���� ��ġ�� ����)
	case 1:
	{
		// ���� ������ Random
		// ���� ������ ũ�⵵ Random
		// float RandomRadius = FinalAppliedRadius * Rand;

		float3 CirclePos = float3(0.f, 0.f, 0.f) + float3(
			cos(RandomAngle) * FinalAppliedRadius * Rand,
			0.f,
			sin(RandomAngle) * FinalAppliedRadius * Rand);

		g_ParticleArray[ThreadID].LocalPos = CirclePos;
	}
	break;
	// ����� ������ �ٶ󺸴� ���·� Ring �����ϰ� �ϱ� 
	case 2:
	{
		float CurrentRingAngle = RandomAngle;

		float3 RingPos = float3(0.f, 0.f, 0.f) + float3(
			cos(RandomAngle) * FinalAppliedRadius,
			sin(RandomAngle) * FinalAppliedRadius,
			0.f);

		g_ParticleArray[ThreadID.x].LocalPos = RingPos;

		// Loop �� �����ϰ� �Ǹ�, ���ʴ�� ��������� �Ѵ�.
		if (g_LoopGenerateRing == 1)
		{
			CurrentRingAngle = g_ParticleShare[0].PrevRingAngle + (360.f / g_ParticleSpawnCountMax);

			if (CurrentRingAngle > 360.f)
				CurrentRingAngle -= 360.f;

			RingPos = float3(0.f, 0.f, 0.f) + float3(
				cos(CurrentRingAngle) * FinalAppliedRadius,
				sin(CurrentRingAngle) * FinalAppliedRadius,
				0.f);

			g_ParticleArray[ThreadID].LocalPos = RingPos;

			g_ParticleShare[0].PrevRingAngle = CurrentRingAngle;

			// ���� Frame ���� �ش� Particle �� ���� ���� (�߽� ����)
			g_ParticleArray[ThreadID].CurrentParticleAngle = CurrentRingAngle;
		}
	}
	break;
	// 3���� �� �����ϱ�
	case 3:
	{
		// http://www.songho.ca/opengl/gl_sphere.html
		float Rand2 = GetRandomNumber((Rand * 1000.f) % ThreadID);

		float PI = 3.14159f;
		int SectorCnt = 360;
		int StackCnt = 180;
		float X, Y, Z, XZ;

		float SectorStep = 2 * 180 / SectorCnt;
		float StackStep = 180 / StackCnt;
		
		float StackAngle = 180 - StackStep * (StackCnt * Rand);
		XZ = FinalAppliedRadius * cos(StackAngle);
		Y = FinalAppliedRadius * sin(StackAngle);

		float SectorAngle = SectorStep * (SectorCnt * Rand2);
		X = XZ * cos(SectorAngle);
		Z = XZ * sin(SectorAngle);

		float3 RingPos = float3(X,Y,Z);

		g_ParticleArray[ThreadID.x].LocalPos = RingPos;
	}
	break;
	}
}

void ApplyInitSpecialMoveDirType(int ThreadID, float RandomAngle, float3 OriginDir, float Rand)
{
	float MoveTowardAngle = RandomAngle;

	// ���� Generate Loop �̶��, ���������� ��������� ��
	// �׷��ٸ�, �߽� ���� -> �ڽ��� ������ ��ġ
	// �ش� ���� �������� ȸ���ؾ� �Ѵ�.
	if (g_LoopGenerateRing == 1)
	{
		MoveTowardAngle = g_ParticleArray[ThreadID].CurrentParticleAngle;
	}

	switch (g_ParticleSpecialMoveDirType)
	{
	case 0:
	{
		// y �� 0 -> XZ �������, ������� ������� �ϱ� 
		float3 RandDir = float3(0.f, 0.f, 0.f) + float3(
			// cos(RandomAngle) * Rand, ������ normalize �� �Ʒ����� �������ִµ�, ���� Rand �� ������� �ϴ°�
			cos(MoveTowardAngle),
			OriginDir.y,
			sin(MoveTowardAngle));
		normalize(RandDir);

		g_ParticleArray[ThreadID].Dir = RandDir;
	}
	break;

	case 1:
	{
		// z �� 0, xy ��� �������� ������� �ϱ� 
		float3 RandDir = float3(0.f, 0.f, 0.f) + float3(
			cos(MoveTowardAngle),
			sin(MoveTowardAngle),
			OriginDir.z);

		normalize(RandDir);

		g_ParticleArray[ThreadID].Dir = RandDir;
	}
	break;
	}
}

// LifeTime �� Setting �ǰ� ����, �ش� �Լ��� �����ؾ� �Ѵ�.
void ApplyInitParticleMove(float3 RandomPos, int ThreadID, float RandomAngle, float Rand)
{
	// Rand�� 0 ���� 1 ������ ��
	// Move �ϴ� Particle �̶��, Speed �� Dir �� �����Ѵ�.
	if (g_ParticleMove == 1)
	{
		float ParticleComponentScaleRatio = (g_ParticleCommonWorldScale.x / 3.f + g_ParticleCommonWorldScale.y / 3.f + g_ParticleCommonWorldScale.z / 3.f);

		// RandomPos.xyz �� ���� 0 ���� 1 ������ �� --> -1 ���� 1 ������ ������ �ٲ۴�.
		// ex) 360�� => -180 ~ 180 �� ������ ������ ������ ȸ���� �Ѵٰ� �����ϸ� �ȴ�.
		float3	ConvertAngle = (RandomPos.xyz * 2.f - 1.f) * g_ParticleMoveAngle;

		// Particle Component �� World Rot Angle ������ �����ش�.
		// float3x3 matRot = ComputeRotationMatrix(ConvertAngle);
		float3x3 matRot = ComputeRotationMatrix(ConvertAngle + g_ParticleRotationAngle);

		float3 OriginDir = mul(g_ParticleMoveDir, matRot);

		float3	Dir = normalize(mul(g_ParticleMoveDir, matRot));

		float ScaledSpeedStart = g_ParticleSpeedStart * ParticleComponentScaleRatio;
		float ScaledSpeedEnd = g_ParticleSpeedEnd * ParticleComponentScaleRatio;

		g_ParticleArray[ThreadID.x].Speed = Rand * (ScaledSpeedEnd - ScaledSpeedStart) + ScaledSpeedStart;

		// Start ���� End �� ���� �ٲ��� �Ѵ�.
		// ���� ���� Speed �� g_ParticleSpeedStart �� �����Ѵ�.
		if (g_ParticleSpeedChangeMethod >= 1)
		{
			g_ParticleArray[ThreadID.x].Speed = ScaledSpeedStart;
		}

		g_ParticleArray[ThreadID.x].Dir = Dir;

		// �ܼ� Dir �̵��� �ƴ϶�, Ư�� �������� Special �ϰ� �̵��ϴ� ������ �ߴٸ�
		ApplyInitSpecialMoveDirType(ThreadID, RandomAngle, OriginDir, Rand);
	}
}

float3 ApplyAliveParticleMove(int ThreadID)
{
	// Speed �� Particle Component�� Scale ��ȭ�� �ݿ�
	float ParticleComponentScaleRatio = (g_ParticleCommonWorldScale.x / 3.f + g_ParticleCommonWorldScale.y / 3.f + g_ParticleCommonWorldScale.z / 3.f);

	float ScaledSpeedStart = g_ParticleSpeedStart * ParticleComponentScaleRatio;
	float ScaledSpeedEnd = g_ParticleSpeedEnd * ParticleComponentScaleRatio;

	float3	MovePos = (float3)0.f;

	float LifeTimeRatio = g_ParticleArray[ThreadID].LifeTime / g_ParticleArray[ThreadID].LifeTimeMax;

	float Inclination = (ScaledSpeedEnd - ScaledSpeedStart / g_ParticleArray[ThreadID].LifeTimeMax);

	// X ���� Offset
	// �ش� ���ڸ�ŭ, ���� �����Լ� �׷��������� X ������ �������� ����.
	// �׷���, ���� �׷��������� ���� �� ���������� ũ�� ���� �� �����ϰ� �ȴ�.
	float XInterceptOffet = 0.0f;

	// Start ���� End �� ���� �ٲ��� �Ѵ�.
// ���� ���� Speed �� g_ParticleSpeedStart �� �����Ѵ�.
	if (g_ParticleSpeedChangeMethod >= 1)
	{
		switch (g_ParticleSpeedChangeMethod)
		{
			// ���������� 
		case 1:
		{
			g_ParticleArray[ThreadID].Speed = ScaledSpeedStart + LifeTimeRatio * (ScaledSpeedEnd - ScaledSpeedStart);
			// LifeTimeRatio
		}
		break;

		// ���� ����
		case 2:
		{
			// y = a^(x - b) + c
			// c �� Speed Start
			// b �� LifeTime �߰�
			// x,y �׷�������
			// x �� 0 ~ LifeTimeMax ���� �����ϰ�
			// y �� SpeedStart ~ SpeedEnd ���� �����Ѵ�.

			// g_ParticleArray[ThreadID.x].LifeTime �� x ������
			// y �� Speed ������ 

			// 1. ���� �����ϴ� ���
			if (Inclination > 0)
			{
				// 	y = a ^ (x - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f) + (ScaledSpeedStart)
				// ScaledSpeedEnd = a ^ (g_ParticleArray[ThreadID.x].LifeTime - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f) + (ScaledSpeedStart)
				// a ^ (g_ParticleArray[ThreadID.x].LifeTime - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f)  = (ScaledSpeedEnd - ScaledSpeedStart)
				// pow(x, y) : x^y
				// a = pow(ScaledSpeedEnd - ScaledSpeedStart), 1 / (g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f));

				// a�� �� (��)
				// - a �� �׻� 1 ���� ū ������� �Ѵ�.
				// - LifeTimeMax �� ���, Speed �� ���� ParticleComponentScaleRatio �� �ݿ��� ���¿��� �Ѵ�.
				float ExponentialBottom = pow(ScaledSpeedEnd - ScaledSpeedStart, 1 / (g_ParticleArray[ThreadID].LifeTimeMax * 0.5f));

				// a �� ���� 1���� Ŀ�� �Ѵ�.
				// �̸� Ȯ���ϱ� ���ؼ��� a ^ (x-b) ���� (x-b) �κ��� 1�� ������ָ� �ȴ�.
				// �׸��� �̶��� y ����, ScaledSpeedStart + 1 ���� ū ���̾�� �Ѵ�.
				// ex) y = a ^ (x-5) + 3
				// x �� 5�϶�, 4
				// x �� 6�϶�, a + 3 
				// a �� 1 ���� ū ���̶��, a + 3 ��, (3 + 1) �� 4 ��� ������ Ŀ�� �Ѵ�.
				// �׷��� ������, x�� ������, y�� �����ϴ� �����Լ��� ����, 1 ���� ũ�ٶ��,
				// �����Լ��� ���ǰ� �������� �ʴ´�.
				// ����, �� ��쿡��, ���������� �����ϰ� �����Ѵ�.
				if (ExponentialBottom + ScaledSpeedStart > ScaledSpeedStart + 1)
				{
					g_ParticleArray[ThreadID].Speed = pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;
					// g_ParticleArray[ThreadID].Speed = pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

					// �Ʒ� �ڵ带 �ְ� �Ǹ�, ���ڿ�������.
					// ���� �Լ��� Y ���� ���Ѵ�� �����Ѵ�.
					// �� ���, Speed End ���� ������Ų��.
					// if (g_ParticleArray[ThreadID].Speed > ScaledSpeedEnd)
					// {
					// 	g_ParticleArray[ThreadID].Speed = ScaledSpeedEnd;
					// }
				}
				else
				{
					g_ParticleArray[ThreadID].Speed = ScaledSpeedStart + LifeTimeRatio * (ScaledSpeedEnd - ScaledSpeedStart);
				}
			}
			else if (Inclination < 0)
			{
				// 	y = -1 * a ^ (x - g_ParticleArray[ThreadID.x].LifeTime / 2.f) + (ScaledSpeedStart)
				// ScaledSpeedEnd = -1 * a ^ (g_ParticleArray[ThreadID.x].LifeTime - g_ParticleArray[ThreadID.x].LifeTime / 2.f) + (ScaledSpeedStart)
				// a ^ (g_ParticleArray[ThreadID.x].LifeTime / 2.f)  = - 1 *  (ScaledSpeedEnd - ScaledSpeedStart)
				// a = pow(-1 * (ScaledSpeedEnd - ScaledSpeedStart)), 1 / (g_ParticleArray[ThreadID.x].LifeTime / 2.f));

				// a�� �� (��)
				// - a �� �׻� 1 ���� ū ������� �Ѵ�.
				// - LifeTimeMax �� ���, Speed �� ���� ParticleComponentScaleRatio �� �ݿ��� ���¿��� �Ѵ�.
				float ExponentialBottom = pow(-1 * (ScaledSpeedEnd - ScaledSpeedStart), 1 / (g_ParticleArray[ThreadID].LifeTimeMax * 0.5f));

				// a �� ���� 1���� Ŀ�� �Ѵ�.
				// �̸� Ȯ���ϱ� ���ؼ��� a ^ (x-b) ���� (x-b) �κ��� 1�� ������ָ� �ȴ�.
				// �׸��� �̶��� y ����, ScaledSpeedStart - 1 ���� ���� ���̾�� �Ѵ�.
				// ex) y = -1 * (a ^ (x-5)) + 3
				// x �� 5�϶�, 2
				// x �� 6�϶�, -1 * a + 3 
				// a �� 1 ���� ū ���̶��, -1 * a + 3 ��, (3 - 1) �� 2 ��� ������ �۾ƾ� �Ѵ�.
				// ����, �� ��쿡��, ���������� �����ϰ� �����Ѵ�.
				if (-1 * ExponentialBottom + ScaledSpeedStart < ScaledSpeedStart - 1)
				{
					g_ParticleArray[ThreadID].Speed = -1 * pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

					// ���� �Լ��� y ���� ���Ѵ�� �����ϹǷ�, 0 �Ʒ��� ���������� �ʰ� �Ѵ�.
					// ������ ScaledSpeedEnd �� ��� �ϴµ�, ���ڿ� ������ ������ �������, �켱 0���� ����
					if (g_ParticleArray[ThreadID].Speed < 0)
					{
						g_ParticleArray[ThreadID].Speed = 0;
					}
				}
				else
				{
					g_ParticleArray[ThreadID].Speed = ScaledSpeedStart - LifeTimeRatio * (ScaledSpeedEnd - ScaledSpeedStart);
				}
			}
			// ���Ⱑ 0 �� ��� -> Speed Start, End �� ���ٴ� ��
			else if (Inclination == 0)
			{

			}
		}
		break;
		case 3:
		{
			// �׸��� �α� �Լ����� X �� 0 �� �� �� ����.
			// ���� Lifetime �� 0 �̶�� �ϴ���, 0.01 ������ ����ְ� �����Ѵ�.
			// (�Ʒ��� �ڵ�� ���̻� �ʿ����. x �� ������ -1 �� ����, ��, x ������ - 1 �������� �Լ� �׷����� �̵�)
			// -1 �̶�� x ���� ������ �� ������, 0�̶�� x ���� ���� ������ �� �ִ�.
			if (g_ParticleArray[ThreadID].LifeTime == 0.f)
			{
				g_ParticleArray[ThreadID].LifeTime = 0.001f;
			}

			// y = log a (x + 1) + C
			// X ������ +1 �� �Ѵ�. ��, X �� �������� ���� ���̴�
			// �̷��� �� ���, X �� 1 �϶�, Y���� 0�̾��� ����, X �� 0�϶�, Y���� 0 �� �� ���̴�.
			// �̷��� �ؾ߸�, Y���� Speed Start ���� �����ϵ��� �� �� �ִ�.
			// ��� C �� ���, SpeedStart �� �����Ѵ�.
			// ��, �׷��� hlsl ���� �������ִ� log �Լ��� ���, ���� e, 2, 10 �� ���ѵǾ� �ִ�.
			// log a (X) = log e (X) / log e (a)
			/*
			1)
			log �Լ�
			X ���� : LifeTime
			Y ���� : Speed St -> Ed
			2)
			���� �Լ�
			X ���� : Speed St -> Ed
			Y ���� : LifeTime -> 0 ���� �����ϹǷ�, Y���� ���� X
			- X ���� : (Ed - St) * 0.5 + St
			y = a ^ (x - X ����)
			3) �ǽð� LifeTime x ����
			- �����Լ� y ���� ���� -> x �� ���ϱ�
			- ���� x ����, log �Լ������� y �� ����, ��, Speed ������ ����
			- ��, �߰����� ���� ���� ����
			*/


			// 1. ���� �����ϴ� ���
			if (Inclination > 0)
			{
				// ����
				// y = log a(x + 1) + SpeedStart
				// SpeedEnd = log a (LifeTimeMax + 1) + SpeedStart
				// log a (LifeTimeMax + 1) = (SpeedEnd - SpeedStart)
				// LifeTimeMax + 1 = a ^ (SpeedEnd - SpeedStart)
				// a = pow(LifeTimeMax + 1, 1 / (SpeedEnd - SpeedStart));

				// y = log a (LifeTime + 1) + SpeedStart
				// y = (log e (LifeTime + 1) / log e (a)) + SpeedStart

				float ExponentialBottom = pow(g_ParticleArray[ThreadID].LifeTimeMax + 1, 1 / (ScaledSpeedEnd - ScaledSpeedStart));

				// �켱, ��������� �� a ��� ��. �� 1���� ū�� �ƴ����� �˻����� �ʴ´�.
				// �⺻������ Inclination �� ������, ExponentialBottom �� 1���� ū ���� ���� �� ���� �����̴�.
				// g_ParticleArray[ThreadID].Speed = -1 * pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

				// g_ParticleArray[ThreadID].Speed = (log(g_ParticleArray[ThreadID].LifeTime + 1) / log(ExponentialBottom)) + ScaledSpeedStart;
				g_ParticleArray[ThreadID].Speed = (log(g_ParticleArray[ThreadID].LifeTime + 1 + XInterceptOffet) / log(ExponentialBottom)) + ScaledSpeedStart;

				// �α� �Լ��� ���, ���� y ��ü�� ���� ���Ѵ�
				// ���� �ּ� Speed �� Speed Start �� ����ش�
				if (g_ParticleArray[ThreadID].Speed < ScaledSpeedStart)
				{
					g_ParticleArray[ThreadID].Speed = ScaledSpeedStart;
				}
			}
			else if (Inclination < 0)
			{
				// a �� 1 ���� ū ��
				// ���Ⱑ ����϶��� ���ؼ�, x ������ ��Ī ��Ų ������ ������ �����.
				// y = -1 * log a (x + 1) + SpeedStart
				// SpeedEnd = -1 * log a (LifeTimeMax + 1) + SpeedStart
				// -1 * log  a(LifeTimeMax + 1) = (SpeedEnd - SpeedStart)
				// log a (LifeTimeMax + 1) = (SpeedStart - SpeedEnd)
				// (LifeTimaMax + 1) = a ^ (SpeedStart - SpeedEnd)
				// a = pow(LifeTimeMax + 1, SpeedStart - SpeedEnd);
				float ExponentialBottom = pow(g_ParticleArray[ThreadID].LifeTimeMax + 1, 1 / (ScaledSpeedStart - ScaledSpeedEnd));

				g_ParticleArray[ThreadID].Speed = -1 * (log(g_ParticleArray[ThreadID].LifeTime + 1 + XInterceptOffet) / log(ExponentialBottom)) + ScaledSpeedStart;

				if (g_ParticleArray[ThreadID].Speed < 0.f)
				{
					g_ParticleArray[ThreadID].Speed = 0.f;
				}
			}
			else if (Inclination == 0)
			{

			}
		}
		break;
		}
	}

	if (g_ParticleMove == 1)
	{
		MovePos = g_ParticleArray[ThreadID].Dir *
			g_ParticleArray[ThreadID].Speed * g_DeltaTime;
	}

	return MovePos;
}

// �߷� �����ϱ� 
void ApplyGravity(int ThreadID, float3 MovePos)
{
	// �߷� ����
	if (g_ParticleShare[0].GravityEnable == 1)
	{
		g_ParticleArray[ThreadID].FallTime += g_DeltaTime;

		float	Velocity = 0.f;

		if (g_ParticleArray[ThreadID].Dir.y > 0.f)
			Velocity = g_ParticleArray[ThreadID].Speed * g_ParticleArray[ThreadID].FallTime;

		g_ParticleArray[ThreadID].LocalPos.y = g_ParticleArray[ThreadID].FallStartY +
			(Velocity - 0.5f * GRAVITY * g_ParticleArray[ThreadID].FallTime * g_ParticleArray[ThreadID].FallTime * 10.f);

		// Bounce ȿ���� ���ٸ� 
		if (g_ParticleBounce == 1)
		{
			if (g_ParticleArray[ThreadID].InitLocalPosY >= g_ParticleArray[ThreadID].LocalPos.y)
			{
				g_ParticleArray[ThreadID].FallTime = 0.f;
				// g_ParticleArray[ThreadID.x].Speed *= 0.98f;
				g_ParticleArray[ThreadID].Speed *= g_ParticleBounceResist;
			}
		}

		g_ParticleArray[ThreadID].LocalPos.x += MovePos.x;
		g_ParticleArray[ThreadID].LocalPos.z += MovePos.z;
	}
	else
		g_ParticleArray[ThreadID.x].LocalPos += MovePos;
}

void ApplyInitRotationAccordingToDir(int ThreadID)
{
	// HLSL ��ǥ�踦 ������ ����ϴ°� ?
	//  y �� 20�� ȸ�� ? -> �ݽð�������� 20�� ���� ��
	// �׷��� �޼���ǥ�迡���� 20�� ȸ���� ��, �ð������ ������ �˰� �ִµ� ..?
	// g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += float3(0.f, 20.f , 0.f);

	if (g_SeperateLinerRotate == 1)
	{
		float RotAngle = 0.f;
		float3 DirVector = g_ParticleArray[ThreadID.x].Dir;

		switch (g_ParticleSpecialMoveDirType)
		{
			// y �� 0 -> XZ �������, ������� ������� �ϱ� 
		case 0:
		{
			// �Ʒ��� Y + ������ �ٶ� ���·� ȸ���ϴ� ��
			float3 CenterVector = float3(0.f, 0.f, 1.f);
			DirVector.y = 0.f;

			RotAngle = AngleBetweenTwoVector(CenterVector, DirVector);

			if (DirVector.x > 0.f)
			{
				// RotAngle = (180 - RotAngle);
				RotAngle = (180 - RotAngle) + 180;
			}

			g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += float3(0.f, RotAngle, 0.f);
		}
		break;

		case 1:
		{
			// z �� 0, xy ��� �������� ������� �ϱ� (z �� - ������ ���ϰ� �ְ� �ȴ�.)
			float3 CenterVector = float3(0.f, 1.f, 0.f);
			DirVector.z = 0.f;

			float RotAngle = AngleBetweenTwoVector(CenterVector, DirVector);

			if (DirVector.x > 0.f)
			{
				RotAngle = (180 - RotAngle) + 180;
			}

			g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += float3(0.f, 0.f, RotAngle);

		}
		break;
		}
	}
}

// �ش� �Լ��� ���� ����, �̹� ����ȭ �۾��� ���ļ�
// Ư�� �����尡, �ڱ� ȥ�ڸ� ���� ������ �����ϰ� �ִ� ����
// ���� ������ �߰����� ����ȭ�� �ʿ����. (�ʿ��ϴٸ� �־�� ������)
void ApplyLinearEffect(int ThreadID)
{
	float DistFromCenter = distance(g_ParticleArray[ThreadID].LocalPos, float3(0.f, 0.f, 0.f));
	float MaxDistFromCenter = g_ParticleShare[0].MaxDistFromCenter;

	float ParticleComponentScaleRatio = (g_ParticleCommonWorldScale.x / 3.f + g_ParticleCommonWorldScale.y / 3.f + g_ParticleCommonWorldScale.z / 3.f);

	if (MaxDistFromCenter == 0)
		MaxDistFromCenter = DistFromCenter;

	float SpawnPosRatio = (float)(DistFromCenter / MaxDistFromCenter);

	if (SpawnPosRatio >= 1.f)
		SpawnPosRatio = 0.999f;

	float ScaledLifeTimeMax = g_ParticleLifeTimeMax * ParticleComponentScaleRatio;
	float ScaledLifeTimeMin = g_ParticleLifeTimeMin * ParticleComponentScaleRatio;

	// Life Time
	if (g_ParticleLifeTimeLinear == 1)
	{
		// �̷��Ը� �ϸ� LifeTime ���� ū ���̰� ����.
		// float LifeTimeRatio = (1 - SpawnPosRatio);
		float LifeTimeRatio = pow(1 - SpawnPosRatio, 2);

		g_ParticleArray[ThreadID].LifeTimeMax = LifeTimeRatio * (ScaledLifeTimeMax - ScaledLifeTimeMin) + ScaledLifeTimeMin;
	}

	// Alpha
	if (g_ParticleAlphaLinearFromCenter == 1)
	{
		// �̷��Ը� �ϸ� LifeTime ���� ū ���̰� ����.
		// float LifeTimeRatio = (1 - SpawnPosRatio);
		float AlphaTimeRatio = pow(1 - SpawnPosRatio, 2);
		float AlphaDiff = g_ParticleAlphaEnd - g_ParticleAlphaStart;

		g_ParticleArray[ThreadID].AlphaDistinctStart = AlphaTimeRatio * (g_ParticleAlphaEnd - g_ParticleAlphaStart) + g_ParticleAlphaStart;
	}
}

[numthreads(64, 1, 1)]	// ������ �׷� ������ ���� �����Ѵ�.
void ParticleUpdate(uint3 ThreadID : SV_DispatchThreadID)
{
	g_ParticleShare[0].SpawnEnable = g_ParticleSpawnEnable;
	g_ParticleShare[0].SpawnCountMax = g_ParticleSpawnCountMax;

	g_ParticleShare[0].ScaleMin = g_ParticleScaleMin;
	g_ParticleShare[0].ScaleMax = g_ParticleScaleMax;

	g_ParticleShare[0].ColorStart = g_ParticleColorMin;
	g_ParticleShare[0].ColorStart.a = g_ParticleAlphaStart;

	g_ParticleShare[0].ColorEnd = g_ParticleColorMax;
	g_ParticleShare[0].ColorEnd.a = g_ParticleAlphaEnd;

	g_ParticleShare[0].CommonWorldScale = g_ParticleCommonWorldScale;
	g_ParticleShare[0].ParticleComponentWorldPos = g_ParticleComponentWorldPos;

	g_ParticleShare[0].GravityEnable = g_ParticleGravity;
	g_ParticleShare[0].RotationAngle = g_ParticleRotationAngle;

	g_ParticleShare[0].DisableNewAlive = g_ParticleDisableNewAlive;

	g_ParticleShare[0].UVMoveEnable = g_ParticleUVMoveEnable;
	g_ParticleShare[0].UVRowN = g_ParticleUVRowN;
	g_ParticleShare[0].UVColN = g_ParticleUVColN;


	// �Ź� �ʱ�ȭ ���� ���̴�.
	g_ParticleArray[ThreadID.x].SeperateRotAngleOffset = float3(0.f, 0.f, 0.f);

	// ó������ �ڱ� �ڽ��� Rot Angle �� -1���� ���� (������ �ʱ�ȭ)
	g_ParticleArray[ThreadID.x].CurrentParticleAngle = PARTICLE_INIT_ANGLE;

	// ���۵Ǵ� �������� ���� �����Ǵ� ��ƼŬ�� �ִ� �� ���� ũ�ų� ���ٸ�
	// �߸��� �ε����� �����ϱ� ������ ó���� �����ش�.
	if (g_ParticleSpawnCountMax <= ThreadID.x)
		return;

	// ���� �⺻ ���� ó��, SpawnTime �� ���缭 ���������� Particle�� �����ϰų�
	// �Ͻ������� Restart ��ư�� ���� ���̶��
	if (g_ParticleDisableNewAlive == 0 ||
		g_ParticleResetSharedInfoSpawnCntSum == 1)
	{
		// SpawnTime �� ���缭 ���������� �����ϰԲ� �Ϸ��� 
		// g_ParticleDisableNewAlive == 1 �϶�, ��, �ѹ��� Ȯ �����ϰ�, ���ݱ��� ���� ������ Particle ������
		// g_ParticleSpawnCountMax �� �Ѿ�� �������� �ʴ� �����ε�.
		// �̸� ��ȿȭ �ϱ� ���ؼ�, 
		// ���ݱ��� ���� ������ Particle ������ ��� 0�� �ǰ� �� ���̴�.
		g_ParticleShare[0].CurrentSpawnCountSum = 0;
	}

	// ��ƼŬ�� ����ִ� ��ƼŬ���� �Ǵ��Ѵ�.
	if (g_ParticleArray[ThreadID.x].Alive == 0)
	{
		// SpawnCount ��ŭ �ѹ��� �����ع�����, 
		// ���� ���ο� Particle �� �������� �������� �Ѵٸ�
		if (g_ParticleShare[0].CurrentSpawnCountSum >= g_ParticleSpawnCountMax &&
			g_ParticleDisableNewAlive == 1)
			// if (g_ParticleShare[0].CurrentSpawnCountSum > g_ParticleSpawnCountMax && g_ParticleDisableNewAlive == 1) -> �Ѱ��� �� ������
			return;

		int	SpawnEnable = g_ParticleShare[0].SpawnEnable;
		int	Exchange = 0;

		if (g_ParticleShare[0].SpawnEnable == 1)
		{
			int	InputValue = SpawnEnable - 1;

			// �Լ��� ���ڴ� in, in, out���� �����Ǿ� �ִ�.
			// in�� �Ϲ������� ������ ���� �Ѱ��ٶ� ����Ѵ�.
			// out�� ����¥�� ���۷���ó�� ������� ������ ������ �޾ƿö� ����Ѵ�.
			// InputValue �� g_ParticleShare[0].SpawnEnable �� ���Ӱ� �������ְ�
			// ������ g_ParticleShare[0].SpawnEnable �� ����ִ� ���� ExChange �� �����´�.
			// �׷���, ���� g_ParticleShare[0].SpawnEnable �� �����Ϸ��� ���� 0 �̴�.
			// InputValue ���� 0�� �������ϱ�
			// ���࿡ g_ParticleShare[0].SpawnEnable �� 1 �� ����ִٸ�
			// �ٸ� �����尡 ���� ���پ��ߴٴ� �ǹ��̰�, Exchange �� 1�� ���� ���̴�.
			// ��, ��������� ���� ���� g_ParticleShare[0].SpawnEnable �� ���� Exchange �� �������� ���̴�.
			InterlockedExchange(g_ParticleShare[0].SpawnEnable, InputValue, Exchange);

			if (Exchange == SpawnEnable)
				g_ParticleArray[ThreadID.x].Alive = 1;
		}

		// �Ʒ��� ���İ��ٴ� ����, ���� �ٷ� ������ Particle �̶�� �ǹ��̴�.
		if (g_ParticleArray[ThreadID.x].Alive == 0)
			return;

		// g_ParticleDisableNewAlive �� ����, ���� ����ȭ ���ۿ� CurrentSpawnCountSum �����ϱ� 
		if (g_ParticleDisableNewAlive == 1)
			g_ParticleShare[0].CurrentSpawnCountSum += 1;
		else
		{
			// SpawnTime �� ���缭 ���������� �����ϰԲ� �Ϸ��� 
			// g_ParticleDisableNewAlive == 1 �϶�, ��, �ѹ��� Ȯ �����ϰ�, ���ݱ��� ���� ������ Particle ������
			// g_ParticleSpawnCountMax �� �Ѿ�� �������� �ʴ� �����ε�.
			// �̸� ��ȿȭ �ϱ� ���ؼ�, 
			// ���ݱ��� ���� ������ Particle ������ ��� 0�� �ǰ� �� ���̴�.
			g_ParticleShare[0].CurrentSpawnCountSum = 0;
		}

		// ����� �ϴ� ��ƼŬ�̶�� ��ƼŬ ���������� ������.
		float	key = ThreadID.x / g_ParticleSpawnCountMax;
		float3	RandomPos = float3(Rand(key), Rand(2.142f), Rand(key * 3.f));
		float	Rand = (RandomPos.x + RandomPos.y + RandomPos.z) / 3.f;
		float RandomAngle = 360.f * Rand;

		float3	StartRange = g_ParticleStartMax - g_ParticleStartMin;

		// ParticleStartMin, Max �� ���
		// ParticleComponent �� World ���� �ݿ� X
		// Particle Component Local Space �󿡼��� Min, Max �����̴�.
		// ���� Particle Component �� World Pos ��ŭ �̵���Ű�� ����
		// GeometryShader ������ ���� ���̴�.

		// StartMin, StartMax �� Particle Component �� Rot �� �ݿ��ؼ� �����ؾ� �Ѵ�.
		// �׷���, StartMin, StartMax �� ���� ȸ����, CPU ������ �̸� �� �������� ���Ŀ� �Ѱ��ش�.
		// �� ?
		// 1) �Ʒ��� ���� �Ѵٴ� ����, ���� ���, ���� x ������ -10 �̵��ϰ�, y�� ���� 45 �� ȸ���Ѵٰ� �ϸ�
		// �� �״�� x �� �̵� ����, ������ �״�� �ִ� ���¿���, �ش� ������ �������� y�� ȸ��
		// ��, ��ġ ������ ȿ���� ��Ÿ���� �Ǵ� ���̴�.

		// 2) �ݸ�, ���� �ڵ�� ���� CPU ������ Rot�� �ݿ��Ѵٸ�
		// - ���� y ������ 45�� ȸ�� ��Ű�� -> �� ���� x �� -10 �̵� ��Ų ȿ���� ������ ���̴�. (�츮�� ���ϴ� ��)
		// ����, CPU ������ Scaling, Rot ������ �����ؼ� StartMin, Max ������ �Ѱ��ش�.
		// float3 ParticleStartMaxPos = mul(g_ParticleStartMax, ComputeRotationMatrix(g_ParticleRotationAngle));
		// float3 ParticleStartMinPos = mul(g_ParticleStartMin, ComputeRotationMatrix(g_ParticleRotationAngle));

		float XRand = GetRandomNumber(key); // 0 ���� 1 ������ ���� 
		float YRand = GetRandomNumber((ThreadID.x * 500.f) % (float)(g_ParticleSpawnCountMax * XRand));
		float ZRand = GetRandomNumber((ThreadID.x * 100.f) % (float)(g_ParticleSpawnCountMax * YRand));

		g_ParticleArray[ThreadID.x].LocalPos.x = XRand * (g_ParticleStartMax.x - g_ParticleStartMin.x) + g_ParticleStartMin.x;
		g_ParticleArray[ThreadID.x].LocalPos.y = YRand * (g_ParticleStartMax.y - g_ParticleStartMin.y) + g_ParticleStartMin.y;
		g_ParticleArray[ThreadID.x].LocalPos.z = ZRand * (g_ParticleStartMax.z - g_ParticleStartMin.z) + g_ParticleStartMin.z;

		// �����Ǵ� ������ Local Y Pos
		g_ParticleArray[ThreadID.x].InitLocalPosY = g_ParticleArray[ThreadID.x].LocalPos.y;

		// �����Ǵ� ������ Particle Component WorldPos
		g_ParticleArray[ThreadID.x].InitParticleComponentWorldPos = g_ParticleComponentWorldPos;

		g_ParticleArray[ThreadID.x].FallTime = 0.f;
		g_ParticleArray[ThreadID.x].FallStartY = g_ParticleArray[ThreadID.x].LocalPos.y;

		// x,y,z �� Relative Scale �� �߰� ������ŭ �������ش�.
	// �Ʒ� �ڵ�ó�� �ϸ�, �ʹ� Dramatic �ϰ� ���Ѵ�.
	// float ParticleComponentScaleRatio = g_ParticleCommonWorldScale.x * g_ParticleCommonWorldScale.y * g_ParticleCommonWorldScale.z;
		float ParticleComponentScaleRatio = (g_ParticleCommonWorldScale.x / 3.f + g_ParticleCommonWorldScale.y / 3.f + g_ParticleCommonWorldScale.z / 3.f);

		float ScaledLifeTimeMax = g_ParticleLifeTimeMax * ParticleComponentScaleRatio;
		float ScaledLifeTimeMin = g_ParticleLifeTimeMin * ParticleComponentScaleRatio;

		g_ParticleArray[ThreadID.x].LifeTime = 0.f;
		g_ParticleArray[ThreadID.x].LifeTimeMax = Rand * (ScaledLifeTimeMax - ScaledLifeTimeMin) + ScaledLifeTimeMin;

		// Scale ũ�⵵ �׸�ŭ �����Ѵ�.

		float FinalAppliedRadius = g_ParcticleGenerateRadius * ParticleComponentScaleRatio;

		// Special �� ������� Particle �� ������� �Ѵٸ�
		ApplyInitSpecialParticleGenerateShape(RandomAngle, ThreadID.x, FinalAppliedRadius, Rand);

		// Move �ϴ� Particle �̶��, Speed �� Dir �� �����Ѵ�.
		// LifeTimeMax �� �̹� ���õ� ���¿��߸� ApplyInitParticleMove �� ������ �� �ִ�.
		ApplyInitParticleMove(RandomPos, ThreadID.x, RandomAngle, Rand);

		// �����Ǵ� ���� �� Particle �� Rot ������ �����Ѵ�.
		// ���� ������ Particle ȸ�� ���� ����
		// '=' �� �ƴ϶� '+=' �� ����� �Ѵ�.
		// ApplySpecialParticleGenerateShape ���� �Ϻ� �̸� FinalSeperateRotAngle ���� 
		// ȸ���� Offset ���� ���س��� �����̱� �����̴�.
		// g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += ((g_ParticleSeperateRotAngleMax - g_ParticleSeperateRotAngleMin) * Rand + g_ParticleSeperateRotAngleMin);
		g_ParticleArray[ThreadID.x].FinalSeperateRotAngle = (g_ParticleSeperateRotAngleMax - g_ParticleSeperateRotAngleMin) * Rand + g_ParticleSeperateRotAngleMin;

		// �ڽ��� ���� ���⿡ ���� ȸ���� �߰��Ѵ�.
		ApplyInitRotationAccordingToDir(ThreadID.x);

		// ���� �������� �߽ɿ��� ���� �ָ������� �ִ� Max �Ÿ����� ���� ����ȭ ���ۿ� �����Ѵ�.
		float DistFromCenter = length(g_ParticleArray[ThreadID.x].LocalPos);
		g_ParticleShare[0].MaxDistFromCenter = max(g_ParticleShare[0].MaxDistFromCenter, DistFromCenter);

		// Alpha ���۰��� �����Ѵ�. �⺻������ ������۷� �Ѿ�� g_ParticleAlphaStart ���� �̿��Ѵ�.
		g_ParticleArray[ThreadID.x].AlphaDistinctStart = g_ParticleAlphaStart;

		// �߾����κ��� ���� �ָ� ������ �ִ� ����ŭ ����
		// �Ʒ����� �ѹ� �� 
		ApplyLinearEffect(ThreadID.x);
	}

	// ���� ������ �Ǿ� �ִ� ��ƼŬ�� ���
	// Ȥ�� ó���� �׳� �� �����ع����� �ڵ���
	// if (g_ParticleDisableNewAlive == 1 || g_ParticleArray[ThreadID.x].Alive == 0)
	else
	{
		g_ParticleArray[ThreadID.x].LifeTime += g_DeltaTime;

		// �Ʒ� �Լ��� ����ϱ� ���� LifeTime �� �ð��� �°� �������Ѿ� �Ѵ�.
		// �ֳ��ϸ� ApplyAliveParticleMove ���� LifeTime ������ ����ϱ� �����̴�.
		float3 MovePos = ApplyAliveParticleMove(ThreadID.x);

		// �߷� ����
		ApplyGravity(ThreadID.x, MovePos);

		if (g_ParticleArray[ThreadID.x].LifeTime >= g_ParticleArray[ThreadID.x].LifeTimeMax)
		{
			g_ParticleArray[ThreadID.x].Alive = 0;
		}
	}

}

struct VertexParticle
{
	float3	Pos	: POSITION;
	uint InstanceID : SV_InstanceID;	// �������ۿ� ���� �־��ִ°��� �ƴϴ�.
};

struct VertexParticleOutput
{
	uint	InstanceID : TEXCOORD;
};

StructuredBuffer<ParticleInfo>		g_ParticleArraySRV	: register(t30);
StructuredBuffer<ParticleInfoShared>	g_ParticleShareSRV	: register(t31);


VertexParticleOutput ParticleVS(VertexParticle input)
{
	VertexParticleOutput	output = (VertexParticleOutput)0;

	output.InstanceID = input.InstanceID;

	return output;
}

struct GeometryParticleOutput
{
	float4	Pos : SV_POSITION;
	float4	Color : COLOR;
	float2	UV	: TEXCOORD;
	float4	ProjPos : POSITION;
	float      LifeTimeMax : NUMBER;
	int			InstanceID : NUMBER1;
	float      LifeTimeRatio : RATIO1;
	float      LocalXPlusMoveDir : RATIO2; // �⺻ 1 �� ���� (���� Local Space ����, X ���� �������� �̵��ϸ�, 0���� ����)
};

static float3	g_ParticleLocalPos[4] =
{
	{-0.5f, 0.5f, 0.f},
	{0.5f, 0.5f, 0.f},
	{-0.5f, -0.5f, 0.f},
	{0.5f, -0.5f, 0.f}
};

// in : ���� �Լ� ������ �Ѱ��ٶ�
// out : �Լ��ȿ��� ����� �Ѱܹ�����. �� �Լ� �ȿ��� �� ���� ����� ������ �� �� ����.
// inout : �׳� ���۷���.
[maxvertexcount(6)]
void ParticleGS(point VertexParticleOutput input[1],
	inout TriangleStream<GeometryParticleOutput> output)
{
	// ���� �簢������ Ȯ���Ѵ�.
	uint	InstanceID = input[0].InstanceID;

	if (g_ParticleArraySRV[InstanceID].Alive == 0)
		return;

	float	Ratio = g_ParticleArraySRV[InstanceID].LifeTime / g_ParticleArraySRV[InstanceID].LifeTimeMax;

	GeometryParticleOutput	OutputArray[4] =
	{
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f,
		(GeometryParticleOutput)0.f
	};

	OutputArray[0].UV = float2(0.f, 0.f);
	OutputArray[1].UV = float2(1.f, 0.f);
	OutputArray[2].UV = float2(0.f, 1.f);
	OutputArray[3].UV = float2(1.f, 1.f);

	// UV Animation ����
	if (g_ParticleShareSRV[0].UVMoveEnable == 1)
	{
		int UVRowN = g_ParticleShareSRV[0].UVRowN;
		int UVColN = g_ParticleShareSRV[0].UVColN;

		// ���� UV ���� ũ��
		float WUVSize = 1.f / (float)UVColN;
		// ���� UV ���� ũ�� 
		float HUVSize = 1.f / (float)UVRowN;

		// ex) 5 * 2 => 10��
		// 9 / 10 => 0.9 
		// 10 * 0.9 => 9��° UV => Idx �����δ� 8��° (�� Idx �� 0 ���� 9���� ����) 
		int TotalUVNum = UVRowN * UVColN;
		float FloatCurUVIdx = ((float)g_ParticleArraySRV[InstanceID].LifeTime / (float)(g_ParticleArraySRV[InstanceID].LifeTimeMax)) * (float)TotalUVNum;
		int CurUVIdx = floor(FloatCurUVIdx);
		// CurUVIdx -= 1;

		// row * col = 2 * 5
		// rowS = 0.5 (height)
		// colS  = 0.2 (width)
		// 0.95 * 10 => 9.5 => floor(9.5) => 9;
		// 10���� ĭ �߿���, ���� ������ ��° ĭ�̴�.
		// 9 / 5 => 1��
		// 9 % 5 => 4��
		int RowUVIdx = CurUVIdx / UVColN;
		int ColUVIdx = (CurUVIdx % UVColN);

		float2 UVStartPos = float2(WUVSize * ColUVIdx, HUVSize * RowUVIdx);
		float2 UVEndPos = UVStartPos + float2(WUVSize, HUVSize);

		OutputArray[0].UV = float2(UVStartPos.x, UVStartPos.y);
		OutputArray[1].UV = float2(UVEndPos.x, UVStartPos.y);
		OutputArray[2].UV = float2(UVStartPos.x, UVEndPos.y);
		OutputArray[3].UV = float2(UVEndPos.x, UVEndPos.y);
	}



	float3	Scale = lerp(
		g_ParticleShareSRV[0].ScaleMin * g_ParticleShareSRV[0].CommonWorldScale,
		g_ParticleShareSRV[0].ScaleMax * g_ParticleShareSRV[0].CommonWorldScale,
		float3(Ratio, Ratio, Ratio));

	float4 ColorStart = g_ParticleShareSRV[0].ColorStart;
	ColorStart.a = g_ParticleArraySRV[InstanceID].AlphaDistinctStart;

	float4	Color = lerp(ColorStart, g_ParticleShareSRV[0].ColorEnd,
		float4(Ratio, Ratio, Ratio, Ratio));

	// ȸ�� (��ü ȸ�� + �� Particle ȸ�� ����)
	float3 FinalRotAngle = g_ParticleShareSRV[0].RotationAngle + g_ParticleArraySRV[InstanceID].FinalSeperateRotAngle;

	float3x3 matRot = ComputeRotationMatrix(FinalRotAngle);

	// 4���� ���� ���������� ������ش�.
	for (int i = 0; i < 4; ++i)
	{
		// g_ParticleArraySRV[InstanceID].WorldPos �� Local Space �󿡼��� 
		float3	WorldPos = g_ParticleArraySRV[InstanceID].LocalPos + mul(g_ParticleLocalPos[i] * Scale, matRot);

		// Particle Component �� World Pos �� ���Ѵ�.
		if (g_ParticleFollowComponentPos == 1)
		{
			WorldPos += g_ParticleShareSRV[0].ParticleComponentWorldPos;
		}
		else
		{
			WorldPos += g_ParticleArraySRV[InstanceID].InitParticleComponentWorldPos;
		}

		OutputArray[i].ProjPos = mul(float4(WorldPos, 1.f), g_matVP);
		// OutputArray[i].ProjPos.xyz = mul(OutputArray[i].ProjPos.xyz, matRot);

		OutputArray[i].Pos = OutputArray[i].ProjPos;

		OutputArray[i].Color = Color;

		OutputArray[i].InstanceID = InstanceID;

		OutputArray[i].LifeTimeMax = g_ParticleArraySRV[InstanceID].LifeTimeMax;

		OutputArray[i].LifeTimeRatio = Ratio;

		OutputArray[i].LocalXPlusMoveDir = 1;

		if (g_ParticleArraySRV[InstanceID].Dir.x < 0.f)
			OutputArray[i].LocalXPlusMoveDir = 0;
	}

	output.Append(OutputArray[0]);
	output.Append(OutputArray[1]);
	output.Append(OutputArray[3]);
	output.RestartStrip();

	output.Append(OutputArray[0]);
	output.Append(OutputArray[3]);
	output.Append(OutputArray[2]);
	output.RestartStrip();
}

// LifeTime ������ ����, (Texture ����) �����ʿ���, ���� �������� ���� ������� �ϱ�
void ApplyLinearUVClipping(GeometryParticleOutput input)
{
	// UV Cllipping ȿ��
	// ������ ����, ���� �������� ������ ������� ȿ���� �� ���̴�.
	// �̸� ���ؼ��� LifeTime �� ����ؾ� �Ѵ�.
	// �ٽ���, ���ϴ� ������ ���� �κи� ��� �����ְ� �ϴ� ���̴�.
	// UV ��ǥ 0 ~ 1. �󿡼�, ������ ���� ���� �����Ѵٰ� ����
	// �ش� ���� ���� -> ������, ������ -> ����. ���� �̵� �ϴ� ����
	if (g_ParticleUVClippingReflectingMoveDir == 1)
	{
		// if (input.UV.x > 1 - input.LifeTimeRatio)
		if (input.UV.x < input.LifeTimeRatio)
			clip(-1);
		/*
		if (input.LocalXPlusMoveDir == 1)
		{
			if (input.UV.x > 1 - input.LifeTimeRatio)
				clip(-1);
		}
		else
		{
			if (input.UV.x > 1 - input.LifeTimeRatio)
				clip(-1);
		}
		*/
	}
}

// Apply Noise Texture 
bool ApplyNoiseTextureDestroyEffect(float2 UV, float LifeTimeMax, float LifeTimeRatio, int InstanceID)
{
	// g_ParticleNoiseTextureEffectFilter �� 1���̻����� �ö��� �ʰ� �� ���̴�. (cpu ���� �ش� �ð��� ���� ���̴�)
	if (g_ParticleApplyNoiseTexture == 1)
	{
		// LifeTimeRatio �� g_ParticleNoiseTextureApplyRatio �̻��� �Ǹ� ������� �Ѵ�.
		if (g_ParticleNoiseTextureApplyRatio > LifeTimeRatio)
			return true;

		// InstanceID �� ����, ���� NoiseTexture ���� �����ϴ� UV ������ �ٸ��� �Ѵ�
		// ex) �� 100�� ���� -> 10��° Instance => 0 ~ 0.1 ������ UV ���� �����ϰ� �ϱ�
		float2 NoiseSmpUV = UV + (InstanceID / (g_ParticleSpawnCountMax * 0.1f));

		float4 FinalColor = g_NoiseTexture.Sample(g_BaseSmp, NoiseSmpUV);

		// float ClipLimit = LifeTimeRatio;
		float ClipLimit = (LifeTimeRatio - g_ParticleNoiseTextureApplyRatio) / (1 - g_ParticleNoiseTextureApplyRatio);

		if (FinalColor.x < ClipLimit)
			return false;
	}

	return true;
}

PSOutput_Single ParticlePS(GeometryParticleOutput input)
{
	PSOutput_Single output = (PSOutput_Single)0;

	float4 Color = g_BaseTexture.Sample(g_BaseSmp, input.UV);

	ApplyLinearUVClipping(input);

	if (ApplyNoiseTextureDestroyEffect(input.UV, input.LifeTimeMax, input.LifeTimeRatio, input.InstanceID) == false)
		clip(-1);

	if (Color.a == 0.f || input.Color.a == 0.f)
		clip(-1);

	float2 ScreenUV = input.ProjPos.xy / input.ProjPos.w;
	ScreenUV = ScreenUV * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	int2 TargetPos = (int2) 0;

	TargetPos.x = (int)(ScreenUV.x * g_Resolution.x);
	TargetPos.y = (int)(ScreenUV.y * g_Resolution.y);

	// Soft Particle
	float4 Depth = g_GBufferDepth.Load(TargetPos, 0);
	float Alpha = 1.f;

	if (Depth.a > 0.f)
		Alpha = (Depth.g - input.ProjPos.w) / 0.5f;

	Alpha = clamp(Alpha, 0.f, 1.f);

	// UV Clipping
	// ������ Texture ����, ���� ��������, ������ ������� ����
	// �ϴ���, Geometry Shader ���� �������� ���̴�.

	// Paper Burn
	Color = PaperBurn2D(Color * input.Color, input.UV);

	// output.Color = Color;
	output.Color = Color * input.Color;
	output.Color.a = Color.a * g_MtrlOpacity * Alpha;

	return output;
}