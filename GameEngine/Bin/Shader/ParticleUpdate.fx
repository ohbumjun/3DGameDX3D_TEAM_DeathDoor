#include "ShaderInfo.fx"
#include "ParticleData.fx"

#define	GRAVITY	9.8f

RWStructuredBuffer<ParticleInfo>		g_ParticleArray	: register(u0);
RWStructuredBuffer<ParticleInfoShared> g_ParticleShare : register(u1);

StructuredBuffer<float>		g_ParticleNormalDistValArray	: register(t20);

void ApplyInitSpecialParticleGenerateShape(float RandomAngle, int ThreadID, float FinalAppliedRadius,
	float Rand)
{
	switch (g_ParticleGenerateShapeType)
	{
		// Ring 모양 생성 => Generate Circle 을 하게 되면, Start Min, Max 가 무의미해지게 되는 것이다.
		// 0.f, 0.f, 0.f 중심으로 원에 생성하기
	case 0:
	{
		float CurrentRingAngle = RandomAngle;

		float3 RingPos = float3(0.f, 0.f, 0.f) + float3(
			cos(CurrentRingAngle) * FinalAppliedRadius,
			0.f,
			sin(CurrentRingAngle) * FinalAppliedRadius);

		g_ParticleArray[ThreadID].LocalPos = RingPos;

		// Loop 을 설정하게 되면, 차례대로 만들어지게 한다.
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

			// 현재 Frame 에서 해당 Particle 이 놓인 각도 (중심 기준)
			g_ParticleArray[ThreadID].CurrentParticleAngle = CurrentRingAngle;
		}
	}
	break;
	// Circle
	// 일정 범위 원 안에서 생성되게 하기 (Ring 과 같이 일정 간격에만 생성 X. 원 안에 랜덤 위치에 생성)
	case 1:
	{
		// 생성 각도도 Random
		// 생성 반지름 크기도 Random
		// float RandomRadius = FinalAppliedRadius * Rand;

		float3 CirclePos = float3(0.f, 0.f, 0.f) + float3(
			cos(RandomAngle) * FinalAppliedRadius * Rand,
			0.f,
			sin(RandomAngle) * FinalAppliedRadius * Rand);

		g_ParticleArray[ThreadID].LocalPos = CirclePos;
	}
	break;
	// 사용자 정면을 바라보는 형태로 Ring 생성하게 하기 
	case 2:
	{
		float CurrentRingAngle = RandomAngle;

		float3 RingPos = float3(0.f, 0.f, 0.f) + float3(
			cos(RandomAngle) * FinalAppliedRadius,
			sin(RandomAngle) * FinalAppliedRadius,
			0.f);

		g_ParticleArray[ThreadID.x].LocalPos = RingPos;

		// Loop 을 설정하게 되면, 차례대로 만들어지게 한다.
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

			// 현재 Frame 에서 해당 Particle 이 놓인 각도 (중심 기준)
			g_ParticleArray[ThreadID].CurrentParticleAngle = CurrentRingAngle;
		}
	}
	break;
	// 3차원 구 생성하기
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

		float3 RingPos = float3(X, Y, Z);

		g_ParticleArray[ThreadID.x].LocalPos = RingPos;
	}
	break;
	// 삼각형
	case 4:
	{
		// 먼저, 바닥에 딱 깔려있다는 것을 가정으로 할 것이다.

		// 먼저 Y좌표값 기준 정렬을 한다.
		float MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

		MinX = g_ParticleStartMin.x < g_ParticleStartMax.x ? g_ParticleStartMin.x : g_ParticleStartMax.x;
		MaxX = g_ParticleStartMin.x > g_ParticleStartMax.x ? g_ParticleStartMin.x : g_ParticleStartMax.x;

		MinY = g_ParticleStartMin.y < g_ParticleStartMax.y ? g_ParticleStartMin.y : g_ParticleStartMax.y;
		MaxY = g_ParticleStartMin.y > g_ParticleStartMax.y ? g_ParticleStartMin.y : g_ParticleStartMax.y;

		MinZ = g_ParticleStartMin.z < g_ParticleStartMax.z ? g_ParticleStartMin.z : g_ParticleStartMax.z;
		MaxZ = g_ParticleStartMin.z > g_ParticleStartMax.z ? g_ParticleStartMin.z : g_ParticleStartMax.z;

		// 오른쪽 상단
		float3 Point1 = float3(MaxX, MaxY, MaxZ);

		// 왼쪽 중간
		float3 Point2 = float3(MinX, (MaxY - MinY) * 0.5f + MinY, (MaxZ - MinZ) * 0.5f + MinZ);

		// 오른쪽 하단
		float3 Point3 = float3(MaxX, MinY, MinZ);

		// 하나의 삼각형은 두개의 삼각형을 분할된다.
		// 정점 2가 정점 1,3 보다 왼쪽에 있는 경우를 고려할 것이다.
		
		// 3개 성분의 기울기를 구한다
		float A21 = abs((Point1.z - Point2.z) / (Point1.x - Point2.x));
		float A13 = abs((Point3.z - Point1.z) / (Point3.x - Point1.x));
		float A31 = abs((Point1.z - Point3.z) / (Point1.x - Point3.x));

		float Rand1 = GetRandomNumber((Rand * 1000.f) % ThreadID);
		float Rand2 = GetRandomNumber((Rand * 300.f) % ThreadID);
		float Rand3 = GetRandomNumber((Rand * 720.f) % ThreadID);

		float UpperRandom = GetRandValForParticle(float2(Rand1, Rand2));
		float DownRandom = GetRandValForParticle(float2(Rand, Rand3));

		// 정점2가 정점1,3 보다 왼쪽에 존재한다는 것은
		// 선분12 의 기울기가, 선분 13의 기울기보다 작다는 것을 의미한다.
		// 사실 위에서 임의로 Point 들을 설정해주었기 때문에, 해당 괄호 위치 안쪽으로 들어올 수 밖에 없다.
		if (A21 < A13)
		{
			// 1) 위쪽 삼각형
			// 정점 1의 Z좌표에서, 정점 2의 Z 좌표까지 반복해서 위쪽 삼각형을 그린다
			//	for (int z = (int)Point1.z; z >= (int)Point2.z; z--)
			//	{
			//		// 해당 루프 안에서 가로로 직선을 그리는 루프를 구성한다.
			//		for (int x = (int)Point2.x; x <= (int)Point1.x; ++x)
			//		{
			//			// x,z 위치에 그린다.
			//		}
			//	}

			// 2) 아래쪽 삼각형
			//	for (int z = (int)Point2.z; z >= (int)Point3.z; z--)
			//	{
			//		// 해당 루프 안에서 가로로 직선을 그리는 루프를 구성한다.
			//		for (int x = (int)Point2.x; x <= (int)Point3.x; ++x)
			//		{
			//			// x,z 위치에 그린다.
			//		}
			//	}

			g_ParticleArray[ThreadID.x].LocalPos.x = (Point3.x - Point2.x) * DownRandom + Point2.x;
			g_ParticleArray[ThreadID.x].LocalPos.y = (Point2.y - Point3.y) * DownRandom + Point3.y;
			g_ParticleArray[ThreadID.x].LocalPos.z = (Point2.z - Point3.z) * DownRandom + Point3.z;

			bool IsUpperRandom = UpperRandom > 0.5f ? true : false;
			
			// if (IsUpperRandom)
			// {
			// 	g_ParticleArray[ThreadID.x].LocalPos.x = (Point1.x - Point2.x) * UpperRandom + Point2.x;
			// 	g_ParticleArray[ThreadID.x].LocalPos.y = (Point1.y - Point2.y) * UpperRandom + Point2.y;
			// 	g_ParticleArray[ThreadID.x].LocalPos.z = (Point1.z - Point2.z) * UpperRandom + Point2.z;
			// }
			// else
			// {
			// 	g_ParticleArray[ThreadID.x].LocalPos.x = (Point3.x - Point2.x) * DownRandom + Point2.x;
			// 	g_ParticleArray[ThreadID.x].LocalPos.y = (Point2.y - Point3.y) * DownRandom + Point3.y;
			// 	g_ParticleArray[ThreadID.x].LocalPos.z = (Point2.z - Point3.z) * DownRandom + Point3.z;
			// }

		}
	}
	break;
	}
}

void ApplyInitSpecialMoveDirType(int ThreadID, float RandomAngle, float3 OriginDir, float Rand)
{
	float MoveTowardAngle = RandomAngle;

	// 만약 Generate Loop 이라면, 순차적으로 만들어지는 것
	// 그렇다면, 중심 기준 -> 자신이 생성된 위치
	// 해당 각도 방향으로 회전해야 한다.
	if (g_LoopGenerateRing == 1)
	{
		MoveTowardAngle = g_ParticleArray[ThreadID].CurrentParticleAngle;
	}

	// -1 이라면, 아무것도 세팅하지 않은 것
	switch (g_ParticleSpecialMoveDirType)
	{
	case 0:
	{
		// y 는 0 -> XZ 평면으로, 사방으로 뻗어나가게 하기 
		float3 RandDir = float3(0.f, 0.f, 0.f) + float3(
			// cos(RandomAngle) * Rand, 어차피 normalize 를 아래에서 진행해주는데, 굳이 Rand 를 곱해줘야 하는가
			cos(MoveTowardAngle),
			OriginDir.y,
			sin(MoveTowardAngle));
		normalize(RandDir);

		g_ParticleArray[ThreadID].Dir = RandDir;
	}
	break;

	case 1:
	{
		// z 는 0, xy 평면 방향으로 뻗어나가게 하기 
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

// LifeTime 이 Setting 되고 나서, 해당 함수를 실행해야 한다.
void ApplyInitParticleMove(float3 RandomPos, int ThreadID, float RandomAngle, float Rand)
{
	// Rand는 0 에서 1 사이의 값
	// Move 하는 Particle 이라면, Speed 와 Dir 를 세팅한다.
	if (g_ParticleMove == 1)
	{
		float ParticleComponentScaleRatio = (g_TVParticleCommonWorldScale.x / 3.f + g_TVParticleCommonWorldScale.y / 3.f + g_TVParticleCommonWorldScale.z / 3.f);

		// RandomPos.xyz 는 각각 0 에서 1 사이의 값 --> -1 에서 1 사이의 값으로 바꾼다.
		// ex) 360도 => -180 ~ 180 도 사이의 랜덤한 각도로 회전을 한다고 생각하면 된다.
		float3	ConvertAngle = (RandomPos.xyz * 2.f - 1.f) * g_ParticleMoveAngle;

		// Particle Component 의 World Rot Angle 정도도 더해준다.
		// float3x3 matRot = ComputeRotationMatrix(ConvertAngle);
		float3x3 matRot = ComputeRotationMatrix(ConvertAngle + g_ParticleRotationAngle);

		float3 OriginDir = mul(g_ParticleMoveDir, matRot);

		float3	Dir = normalize(mul(g_ParticleMoveDir, matRot));

		float ScaledSpeedStart = g_ParticleSpeedStart * ParticleComponentScaleRatio;
		float ScaledSpeedEnd = g_ParticleSpeedEnd * ParticleComponentScaleRatio;

		g_ParticleArray[ThreadID.x].Speed = Rand * (ScaledSpeedEnd - ScaledSpeedStart) + ScaledSpeedStart;

		// Start 에서 End 로 점점 바뀌어야 한다.
		// 따라서 최초 Speed 는 g_ParticleSpeedStart 로 세팅한다.
		if (g_ParticleSpeedChangeMethod >= 1)
		{
			g_ParticleArray[ThreadID.x].Speed = ScaledSpeedStart;
		}

		g_ParticleArray[ThreadID.x].Dir = Dir;

		// 단순 Dir 이동이 아니라, 특정 방향으로 Special 하게 이동하는 설정을 했다면
		ApplyInitSpecialMoveDirType(ThreadID, RandomAngle, OriginDir, Rand);
	}
}

float3 ApplyAliveParticleMove(int ThreadID)
{
	// Speed 에 Particle Component의 Scale 변화도 반영
	float ParticleComponentScaleRatio = (g_TVParticleCommonWorldScale.x / 3.f + g_TVParticleCommonWorldScale.y / 3.f + g_TVParticleCommonWorldScale.z / 3.f);

	float ScaledSpeedStart = g_ParticleSpeedStart * ParticleComponentScaleRatio;
	float ScaledSpeedEnd = g_ParticleSpeedEnd * ParticleComponentScaleRatio;

	float3	MovePos = (float3)0.f;

	float LifeTimeRatio = g_ParticleArray[ThreadID].LifeTime / g_ParticleArray[ThreadID].LifeTimeMax;

	float Inclination = (ScaledSpeedEnd - ScaledSpeedStart / g_ParticleArray[ThreadID].LifeTimeMax);

	// X 절편 Offset
	// 해당 숫자만큼, 실제 지수함수 그래프에서의 X 절편을 왼쪽으로 댕긴다.
	// 그러면, 실제 그래프에서의 증가 및 감소폭보다 크게 증가 및 감소하게 된다.
	float XInterceptOffet = 0.0f;

	// Start 에서 End 로 점점 바뀌어야 한다.
// 따라서 최초 Speed 는 g_ParticleSpeedStart 로 세팅한다.
	if (g_ParticleSpeedChangeMethod >= 1)
	{
		switch (g_ParticleSpeedChangeMethod)
		{
			// 선형적으로 
		case 1:
		{
			g_ParticleArray[ThreadID].Speed = ScaledSpeedStart + LifeTimeRatio * (ScaledSpeedEnd - ScaledSpeedStart);
			// LifeTimeRatio
		}
		break;

		// 지수 분포
		case 2:
		{
			// y = a^(x - b) + c
			// c 는 Speed Start
			// b 는 LifeTime 중간
			// x,y 그래프에서
			// x 는 0 ~ LifeTimeMax 까지 증가하고
			// y 는 SpeedStart ~ SpeedEnd 까지 증가한다.

			// g_ParticleArray[ThreadID.x].LifeTime 이 x 미지수
			// y 가 Speed 미지수 

			// 1. 점차 증가하는 경우
			if (Inclination > 0)
			{
				// 	y = a ^ (x - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f) + (ScaledSpeedStart)
				// ScaledSpeedEnd = a ^ (g_ParticleArray[ThreadID.x].LifeTime - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f) + (ScaledSpeedStart)
				// a ^ (g_ParticleArray[ThreadID.x].LifeTime - g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f)  = (ScaledSpeedEnd - ScaledSpeedStart)
				// pow(x, y) : x^y
				// a = pow(ScaledSpeedEnd - ScaledSpeedStart), 1 / (g_ParticleArray[ThreadID.x].LifeTimeMax / 2.f));

				// a의 값 (밑)
				// - a 는 항상 1 보다 큰 계수여야 한다.
				// - LifeTimeMax 의 경우, Speed 와 같이 ParticleComponentScaleRatio 가 반영된 상태여야 한다.
				float ExponentialBottom = pow(ScaledSpeedEnd - ScaledSpeedStart, 1 / (g_ParticleArray[ThreadID].LifeTimeMax * 0.5f));

				// a 의 값이 1보다 커야 한다.
				// 이를 확인하기 위해서는 a ^ (x-b) 에서 (x-b) 부분을 1로 만들어주면 된다.
				// 그리고 이때의 y 값은, ScaledSpeedStart + 1 보다 큰 값이어야 한다.
				// ex) y = a ^ (x-5) + 3
				// x 가 5일때, 4
				// x 가 6일때, a + 3 
				// a 가 1 보다 큰 값이라면, a + 3 이, (3 + 1) 인 4 라는 값보다 커야 한다.
				// 그렇지 않으면, x가 증가시, y도 증가하는 지수함수의 밑이, 1 보다 크다라는,
				// 지수함수의 조건과 부합하지 않는다.
				// 따라서, 이 경우에는, 선형적으로 증가하게 세팅한다.
				if (ExponentialBottom + ScaledSpeedStart > ScaledSpeedStart + 1)
				{
					g_ParticleArray[ThreadID].Speed = pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;
					// g_ParticleArray[ThreadID].Speed = pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

					// 아래 코드를 넣게 되면, 부자연스럽다.
					// 지수 함수는 Y 축이 무한대로 증가한다.
					// 이 경우, Speed End 값에 고정시킨다.
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

				// a의 값 (밑)
				// - a 는 항상 1 보다 큰 계수여야 한다.
				// - LifeTimeMax 의 경우, Speed 와 같이 ParticleComponentScaleRatio 가 반영된 상태여야 한다.
				float ExponentialBottom = pow(-1 * (ScaledSpeedEnd - ScaledSpeedStart), 1 / (g_ParticleArray[ThreadID].LifeTimeMax * 0.5f));

				// a 의 값이 1보다 커야 한다.
				// 이를 확인하기 위해서는 a ^ (x-b) 에서 (x-b) 부분을 1로 만들어주면 된다.
				// 그리고 이때의 y 값은, ScaledSpeedStart - 1 보다 작은 값이어야 한다.
				// ex) y = -1 * (a ^ (x-5)) + 3
				// x 가 5일때, 2
				// x 가 6일때, -1 * a + 3 
				// a 가 1 보다 큰 값이라면, -1 * a + 3 이, (3 - 1) 인 2 라는 값보다 작아야 한다.
				// 따라서, 이 경우에는, 선형적으로 감소하게 세팅한다.
				if (-1 * ExponentialBottom + ScaledSpeedStart < ScaledSpeedStart - 1)
				{
					g_ParticleArray[ThreadID].Speed = -1 * pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

					// 지수 함수는 y 값이 무한대로 감소하므로, 0 아래로 내려가지는 않게 한다.
					// 원래는 ScaledSpeedEnd 로 줘야 하는데, 부자연 스러운 느낌이 있을까봐, 우선 0으로 세팅
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
			// 기울기가 0 일 경우 -> Speed Start, End 가 같다는 것
			else if (Inclination == 0)
			{

			}
		}
		break;
		case 3:
		{
			// 그리고 로그 함수에서 X 는 0 이 될 수 없다.
			// 따라서 Lifetime 이 0 이라고 하더라도, 0.01 정도로 잡아주고 시작한다.
			// (아래의 코드는 더이상 필요없다. x 축 절편을 -1 로 세팅, 즉, x 축으로 - 1 방향으로 함수 그래프를 이동)
			// -1 이라는 x 값은 존재할 수 없지만, 0이라는 x 값은 이제 존재할 수 있다.
			if (g_ParticleArray[ThreadID].LifeTime == 0.f)
			{
				g_ParticleArray[ThreadID].LifeTime = 0.001f;
			}

			// y = log a (x + 1) + C
			// X 절편은 +1 로 한다. 즉, X 축 왼쪽으로 당기는 것이다
			// 이렇게 될 경우, X 가 1 일때, Y값이 0이었던 것이, X 가 0일때, Y값이 0 이 될 것이다.
			// 이렇게 해야만, Y값이 Speed Start 부터 시작하도록 할 수 있다.
			// 대신 C 의 경우, SpeedStart 로 세팅한다.
			// 자, 그런데 hlsl 에서 제공해주는 log 함수의 경우, 밑이 e, 2, 10 에 제한되어 있다.
			// log a (X) = log e (X) / log e (a)
			/*
			1)
			log 함수
			X 범위 : LifeTime
			Y 범위 : Speed St -> Ed
			2)
			지수 함수
			X 범위 : Speed St -> Ed
			Y 범위 : LifeTime -> 0 부터 시작하므로, Y절편 존재 X
			- X 절편 : (Ed - St) * 0.5 + St
			y = a ^ (x - X 절편)
			3) 실시간 LifeTime x 값을
			- 지수함수 y 값에 세팅 -> x 값 구하기
			- 구한 x 값을, log 함수에서의 y 로 세팅, 즉, Speed 값으로 세팅
			- 단, 추가적인 범위 제한 세팅
			*/


			// 1. 점차 증가하는 경우
			if (Inclination > 0)
			{
				// 수식
				// y = log a(x + 1) + SpeedStart
				// SpeedEnd = log a (LifeTimeMax + 1) + SpeedStart
				// log a (LifeTimeMax + 1) = (SpeedEnd - SpeedStart)
				// LifeTimeMax + 1 = a ^ (SpeedEnd - SpeedStart)
				// a = pow(LifeTimeMax + 1, 1 / (SpeedEnd - SpeedStart));

				// y = log a (LifeTime + 1) + SpeedStart
				// y = (log e (LifeTime + 1) / log e (a)) + SpeedStart

				float ExponentialBottom = pow(g_ParticleArray[ThreadID].LifeTimeMax + 1, 1 / (ScaledSpeedEnd - ScaledSpeedStart));

				// 우선, 결과적으로 얻어낸 a 라는 밑. 이 1보다 큰지 아닌지는 검사하지 않는다.
				// 기본적으로 Inclination 가 양수라면, ExponentialBottom 도 1보다 큰 값이 나올 것 같기 때문이다.
				// g_ParticleArray[ThreadID].Speed = -1 * pow(ExponentialBottom, g_ParticleArray[ThreadID].LifeTime + XInterceptOffet - g_ParticleArray[ThreadID].LifeTimeMax * 0.5f) + ScaledSpeedStart;

				// g_ParticleArray[ThreadID].Speed = (log(g_ParticleArray[ThreadID].LifeTime + 1) / log(ExponentialBottom)) + ScaledSpeedStart;
				g_ParticleArray[ThreadID].Speed = (log(g_ParticleArray[ThreadID].LifeTime + 1 + XInterceptOffet) / log(ExponentialBottom)) + ScaledSpeedStart;

				// 로그 함수의 경우, 시작 y 자체가 음의 무한대
				// 따라서 최소 Speed 는 Speed Start 로 잡아준다
				if (g_ParticleArray[ThreadID].Speed < ScaledSpeedStart)
				{
					g_ParticleArray[ThreadID].Speed = ScaledSpeedStart;
				}
			}
			else if (Inclination < 0)
			{
				// a 는 1 보다 큰 값
				// 기울기가 양수일때에 대해서, x 축으로 대칭 시킨 개념의 수식을 세운다.
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

// 중력 적용하기 
void ApplyGravity(int ThreadID, float3 MovePos)
{
	// 중력 적용
	if (g_ParticleShare[0].GravityEnable == 1)
	{
		g_ParticleArray[ThreadID].FallTime += g_DeltaTime;

		float	Velocity = 0.f;

		if (g_ParticleArray[ThreadID].Dir.y > 0.f)
			Velocity = g_ParticleArray[ThreadID].Speed * g_ParticleArray[ThreadID].FallTime;

		g_ParticleArray[ThreadID].LocalPos.y = g_ParticleArray[ThreadID].FallStartY +
			(Velocity - 0.5f * GRAVITY * g_ParticleArray[ThreadID].FallTime * g_ParticleArray[ThreadID].FallTime * 10.f);

		// Bounce 효과를 낸다면 
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
	// HLSL 좌표계를 무엇을 사용하는가 ?
	//  y 축 20도 회전 ? -> 반시계방향으로 20도 돌린 것
	// 그런데 왼손좌표계에서는 20도 회전이 곧, 시계방향인 것으로 알고 있는데 ..?
	// g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += float3(0.f, 20.f , 0.f);

	if (g_SeperateLinerRotate == 1)
	{
		float RotAngle = 0.f;
		float3 DirVector = g_ParticleArray[ThreadID.x].Dir;

		switch (g_ParticleSpecialMoveDirType)
		{
			// y 는 0 -> XZ 평면으로, 사방으로 뻗어나가게 하기 
		case 0:
		{
			// 아래는 Y + 방향을 바라본 상태로 회전하는 것
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
			// z 는 0, xy 평면 방향으로 뻗어나가게 하기 (z 는 - 방향을 향하고 있게 된다.)
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

// 해당 함수로 들어올 때는, 이미 동기화 작업을 거쳐서
// 특정 쓰레드가, 자기 혼자만 공유 정보에 접근하고 있는 상태
// 따라서 별도의 추가적인 동기화는 필요없다. (필요하다면 넣어야 하지만)
void ApplyLinearEffect(int ThreadID)
{
	float DistFromCenter = distance(g_ParticleArray[ThreadID].LocalPos, float3(0.f, 0.f, 0.f));
	float MaxDistFromCenter = g_ParticleShare[0].MaxDistFromCenter;

	float ParticleComponentScaleRatio = (g_TVParticleCommonWorldScale.x / 3.f + g_TVParticleCommonWorldScale.y / 3.f + g_TVParticleCommonWorldScale.z / 3.f);

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
		// 이렇게만 하면 LifeTime 에서 큰 차이가 없다.
		// float LifeTimeRatio = (1 - SpawnPosRatio);
		float LifeTimeRatio = pow(1 - SpawnPosRatio, 2);

		g_ParticleArray[ThreadID].LifeTimeMax = LifeTimeRatio * (ScaledLifeTimeMax - ScaledLifeTimeMin) + ScaledLifeTimeMin;
	}

	// Alpha
	if (g_ParticleAlphaLinearFromCenter == 1)
	{
		// 이렇게만 하면 LifeTime 에서 큰 차이가 없다.
		// float LifeTimeRatio = (1 - SpawnPosRatio);
		float AlphaTimeRatio = pow(1 - SpawnPosRatio, 2);
		float AlphaDiff = g_ParticleAlphaEnd - g_ParticleAlphaStart;

		g_ParticleArray[ThreadID].AlphaDistinctStart = AlphaTimeRatio * (g_ParticleAlphaEnd - g_ParticleAlphaStart) + g_ParticleAlphaStart;
	}
}

[numthreads(64, 1, 1)]	// 스레드 그룹 스레드 수를 지정한다.
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

	g_ParticleShare[0].CommonWorldScale = g_TVParticleCommonWorldScale;
	g_ParticleShare[0].ParticleComponentWorldPos = g_TVParticleComponentWorldPos;

	g_ParticleShare[0].GravityEnable = g_ParticleGravity;
	g_ParticleShare[0].RotationAngle = g_ParticleRotationAngle;

	g_ParticleShare[0].DisableNewAlive = g_ParticleDisableNewAlive;

	g_ParticleShare[0].UVMoveEnable = g_ParticleUVMoveEnable;
	g_ParticleShare[0].UVRowN = g_ParticleUVRowN;
	g_ParticleShare[0].UVColN = g_ParticleUVColN;

	// 매번 초기화 해줄 것이다.
	g_ParticleArray[ThreadID.x].SeperateRotAngleOffset = float3(0.f, 0.f, 0.f);

	// 처음에는 자기 자신의 Rot Angle 을 -1으로 세팅 (일종의 초기화)
	g_ParticleArray[ThreadID.x].CurrentParticleAngle = PARTICLE_INIT_ANGLE;

	// 동작되는 스레드의 수가 생성되는 파티클의 최대 수 보다 크거나 같다면
	// 잘못된 인덱스로 동작하기 때문에 처리를 안해준다.
	if (g_ParticleSpawnCountMax <= ThreadID.x)
		return;

	// 원래 기본 설정 처럼, SpawnTime 에 맞춰서 지속적으로 Particle을 생성하거나
	// 일시적으로 Restart 버튼을 누른 것이라면
	// if (g_ParticleDisableNewAlive == 0 || g_TVResetParticleSharedInfoSumSpawnCnt == 1)
	if (g_TVResetParticleSharedInfoSumSpawnCnt == 1)
	{
		// SpawnTime 에 맞춰서 지속적으로 생성하게끔 하려면 
		// g_ParticleDisableNewAlive == 1 일때, 즉, 한번에 확 생성하고, 지금까지 누적 생성된 Particle 개수가
		// g_ParticleSpawnCountMax 를 넘어가면 생성되지 않는 원리인데.
		// 이를 무효화 하기 위해서, 
		// 지금까지 누적 생성된 Particle 개수를 계속 0이 되게 할 것이다.
		g_ParticleShare[0].CurrentSpawnCountSum = 0;
		return;
	}

	// 파티클이 살아있는 파티클인지 판단한다.
	if (g_ParticleArray[ThreadID.x].Alive == 0)
	{
		// SpawnCount 만큼 한번에 생성해버리고, 
		// 차후 새로운 Particle 은 생성하지 않으려고 한다면
		if (g_ParticleShare[0].CurrentSpawnCountSum >= g_ParticleSpawnCountMax &&
			g_ParticleDisableNewAlive == 1)
		{
			// if (g_ParticleShare[0].CurrentSpawnCountSum > g_ParticleSpawnCountMax && g_ParticleDisableNewAlive == 1) -> 한개가 더 생성됨
			return;
		}

		int	SpawnEnable = g_ParticleShare[0].SpawnEnable;
		int	Exchange = 0;

		if (g_ParticleShare[0].SpawnEnable == 1)
		{
			int	InputValue = SpawnEnable - 1;

			// 함수의 인자는 in, in, out으로 구성되어 있다.
			// in은 일반적으로 변수의 값을 넘겨줄때 사용한다.
			// out은 반쪽짜리 레퍼런스처럼 결과값을 여기의 변수에 받아올때 사용한다.
			// InputValue 를 g_ParticleShare[0].SpawnEnable 에 새롭게 세팅해주고
			// 기존에 g_ParticleShare[0].SpawnEnable 에 들어있던 값을 ExChange 로 가져온다.
			// 그러면, 지금 g_ParticleShare[0].SpawnEnable 에 세팅하려는 값은 0 이다.
			// InputValue 에는 0이 들어가있으니까
			// 만약에 g_ParticleShare[0].SpawnEnable 에 1 이 들어있다면
			// 다른 쓰레드가 아직 접근안했다는 의미이고, Exchange 에 1이 들어올 것이다.
			// 즉, 결과적으로 지금 현재 g_ParticleShare[0].SpawnEnable 의 값을 Exchange 로 가져오는 것이다.
			InterlockedExchange(g_ParticleShare[0].SpawnEnable, InputValue, Exchange);

			if (Exchange == SpawnEnable)
				g_ParticleArray[ThreadID.x].Alive = 1;
		}

		// 아래를 거쳐갔다는 것은, 현재 바로 생성된 Particle 이라는 의미이다.
		if (g_ParticleArray[ThreadID.x].Alive == 0)
			return;

		// g_ParticleDisableNewAlive 에 따라서, 공유 구조화 버퍼에 CurrentSpawnCountSum 세팅하기 
		if (g_ParticleDisableNewAlive == 1)
		{
			if (g_TVResetParticleSharedInfoSumSpawnCnt == 1)
			{
				g_ParticleShare[0].CurrentSpawnCountSum = 0;
			}
			else
			{
				g_ParticleShare[0].CurrentSpawnCountSum += 1;
			}
		}
		else
		{
			// SpawnTime 에 맞춰서 지속적으로 생성하게끔 하려면 
			// g_ParticleDisableNewAlive == 1 일때, 즉, 한번에 확 생성하고, 지금까지 누적 생성된 Particle 개수가
			// g_ParticleSpawnCountMax 를 넘어가면 생성되지 않는 원리인데.
			// 이를 무효화 하기 위해서, 
			// 지금까지 누적 생성된 Particle 개수를 계속 0이 되게 할 것이다.
			g_ParticleShare[0].CurrentSpawnCountSum = 0;
		}

		// 살려야 하는 파티클이라면 파티클 생성정보를 만들어낸다.
		float	key = ThreadID.x / g_ParticleSpawnCountMax;
		float3	RandomPos = float3(Rand(key), Rand(2.142f), Rand(key * 3.f));
		float	Rand = (RandomPos.x + RandomPos.y + RandomPos.z) / 3.f;
		float RandomAngle = 360.f * Rand;

		float3	StartRange = g_ParticleStartMax - g_ParticleStartMin;

		// ParticleStartMin, Max 의 기능
		// ParticleComponent 의 World 공간 반영 X
		// Particle Component Local Space 상에서의 Min, Max 범위이다.
		// 실제 Particle Component 의 World Pos 만큼 이동시키는 것은
		// GeometryShader 측에서 해줄 것이다.

		// StartMin, StartMax 도 Particle Component 의 Rot 을 반영해서 세팅해야 한다.
		// 그런데, StartMin, StartMax 에 대한 회전은, CPU 측에서 미리 다 세팅해준 이후에 넘겨준다.
		// 왜 ?
		// 1) 아래와 같이 한다는 것은, 예를 들어, 내가 x 축으로 -10 이동하고, y축 으로 45 도 회전한다고 하면
		// 말 그대로 x 축 이동 이후, 원점은 그대로 있던 상태에서, 해당 원점을 기준으로 y축 회전
		// 즉, 마치 공전의 효과가 나타나게 되는 것이다.

		// 2) 반면, 현재 코드와 같이 CPU 측에서 Rot을 반영한다면
		// - 먼저 y 축으로 45도 회전 시키고 -> 그 다음 x 축 -10 이동 시킨 효과가 나오는 것이다. (우리가 원하는 것)
		// 따라서, CPU 측에서 Scaling, Rot 까지는 적용해서 StartMin, Max 범위를 넘겨준다.
		// float3 ParticleStartMaxPos = mul(g_ParticleStartMax, ComputeRotationMatrix(g_ParticleRotationAngle));
		// float3 ParticleStartMinPos = mul(g_ParticleStartMin, ComputeRotationMatrix(g_ParticleRotationAngle));

		float XRand = GetRandomNumber(key); // 0 에서 1 사이의 숫자 
		float YRand = GetRandomNumber((ThreadID.x * 500.f) % (float)(g_ParticleSpawnCountMax * XRand));
		float ZRand = GetRandomNumber((ThreadID.x * 100.f) % (float)(g_ParticleSpawnCountMax * YRand));

		g_ParticleArray[ThreadID.x].LocalPos.x = XRand * (g_ParticleStartMax.x - g_ParticleStartMin.x) + g_ParticleStartMin.x;
		g_ParticleArray[ThreadID.x].LocalPos.y = YRand * (g_ParticleStartMax.y - g_ParticleStartMin.y) + g_ParticleStartMin.y;
		g_ParticleArray[ThreadID.x].LocalPos.z = ZRand * (g_ParticleStartMax.z - g_ParticleStartMin.z) + g_ParticleStartMin.z;

		// 생성되는 시점의 Local Y Pos
		g_ParticleArray[ThreadID.x].InitLocalPosY = g_ParticleArray[ThreadID.x].LocalPos.y;

		// 생성되는 시점의 Particle Component WorldPos
		g_ParticleArray[ThreadID.x].InitParticleComponentWorldPos = g_TVParticleComponentWorldPos;

		g_ParticleArray[ThreadID.x].FallTime = 0.f;
		g_ParticleArray[ThreadID.x].FallStartY = g_ParticleArray[ThreadID.x].LocalPos.y;

		// x,y,z 의 Relative Scale 의 중간 비율만큼 세팅해준다.
	// 아래 코드처럼 하면, 너무 Dramatic 하게 변한다.
	// float ParticleComponentScaleRatio = g_TVParticleCommonWorldScale.x * g_TVParticleCommonWorldScale.y * g_TVParticleCommonWorldScale.z;
		float ParticleComponentScaleRatio = (g_TVParticleCommonWorldScale.x / 3.f + g_TVParticleCommonWorldScale.y / 3.f + g_TVParticleCommonWorldScale.z / 3.f);

		float ScaledLifeTimeMax = g_ParticleLifeTimeMax * ParticleComponentScaleRatio;
		float ScaledLifeTimeMin = g_ParticleLifeTimeMin * ParticleComponentScaleRatio;

		g_ParticleArray[ThreadID.x].LifeTime = 0.f;
		g_ParticleArray[ThreadID.x].LifeTimeMax = Rand * (ScaledLifeTimeMax - ScaledLifeTimeMin) + ScaledLifeTimeMin;

		// Scale 크기도 그만큼 조정한다.
		float FinalAppliedRadius = g_ParcticleGenerateRadius * ParticleComponentScaleRatio;

		// Special 한 모양으로 Particle 을 만들고자 한다면
		ApplyInitSpecialParticleGenerateShape(RandomAngle, ThreadID.x, FinalAppliedRadius, Rand);

		// Move 하는 Particle 이라면, Speed 와 Dir 를 세팅한다.
		// LifeTimeMax 가 이미 세팅된 상태여야만 ApplyInitParticleMove 를 세팅할 수 있다.
		ApplyInitParticleMove(RandomPos, ThreadID.x, RandomAngle, Rand);

		// 생성되는 순간 각 Particle 의 Rot 정도를 세팅한다.
		// 최종 각각의 Particle 회전 정도 세팅
		// '=' 가 아니라 '+=' 를 해줘야 한다.
		// ApplySpecialParticleGenerateShape 에서 일부 미리 FinalSeperateRotAngle 값에 
		// 회전할 Offset 값을 더해놓은 상태이기 때문이다.
		// g_ParticleArray[ThreadID.x].FinalSeperateRotAngle += ((g_ParticleSeperateRotAngleMax - g_ParticleSeperateRotAngleMin) * Rand + g_ParticleSeperateRotAngleMin);
		float RandRotValue = GetRandValForParticle(float2(XRand, YRand));
		g_ParticleArray[ThreadID.x].FinalSeperateRotAngle = (g_ParticleSeperateRotAngleMax - g_ParticleSeperateRotAngleMin) * RandRotValue + g_ParticleSeperateRotAngleMin;

		// 자신의 진행 방향에 따른 회전을 추가한다.
		ApplyInitRotationAccordingToDir(ThreadID.x);

		// 가장 마지막에 중심에서 가장 멀리떨어져 있는 Max 거리값을 공유 구조화 버퍼에 세팅한다.
		float DistFromCenter = length(g_ParticleArray[ThreadID.x].LocalPos);
		g_ParticleShare[0].MaxDistFromCenter = max(g_ParticleShare[0].MaxDistFromCenter, DistFromCenter);

		// Alpha 시작값을 세팅한다. 기본적으로 상수버퍼로 넘어온 g_ParticleAlphaStart 값을 이용한다.
		g_ParticleArray[ThreadID.x].AlphaDistinctStart = g_ParticleAlphaStart;

		// 중앙으로부터 가장 멀리 떨어져 있는 값만큼 세팅
		// 아래에서 한번 더 
		ApplyLinearEffect(ThreadID.x);
	}

	// 현재 생성이 되어 있는 파티클일 경우
	// 혹은 처음에 그냥 다 생성해버리는 코드라면
	// if (g_ParticleDisableNewAlive == 1 || g_ParticleArray[ThreadID.x].Alive == 0)
	else
	{
		// ObjectPool 에 들어갈 때, Reset 호출
		// 기존에 살아있던 Particle들을 다 Alive False 로 만들어서 Reset 시켜줄 것이다.
		if (g_DestroyAllExistingLivingParticles == 1)
			// if (g_Particle2D == 1)
		{
			g_ParticleArray[ThreadID.x].LifeTime = g_ParticleArray[ThreadID.x].LifeTimeMax;
			g_ParticleArray[ThreadID.x].Alive = 0;
			return;
		}

		g_ParticleArray[ThreadID.x].LifeTime += g_DeltaTime;

		// 아래 함수를 사용하기 전에 LifeTime 을 시간에 맞게 증가시켜야 한다.
		// 왜냐하면 ApplyAliveParticleMove 에서 LifeTime 비율을 계산하기 때문이다.
		float3 MovePos = ApplyAliveParticleMove(ThreadID.x);

		// 중력 적용
		ApplyGravity(ThreadID.x, MovePos);

		if (g_ParticleArray[ThreadID.x].LifeTime >= g_ParticleArray[ThreadID.x].LifeTimeMax)
		{
			g_ParticleArray[ThreadID.x].Alive = 0;
		}
	}
}