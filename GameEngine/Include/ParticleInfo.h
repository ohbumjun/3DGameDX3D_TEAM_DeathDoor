#pragma once

struct	ParticleCBuffer
{
	unsigned int	SpawnEnable;	// 현재 파티클 생성 여부
	Vector3	StartMin;		// 파티클이 생성될 영역의 Min

	Vector3	StartMax;		// 파티클이 생성될 영역의 Max
	unsigned int	SpawnCountMax;	// 생성될 파티클의 최대

	Vector3	ScaleMin;		// 생성될 파티클 크기의 Min
	float	LifeTimeMin;	// 생성될 파티클이 살아있을 최소시간

	Vector3	ScaleMax;		// 새성될 파티클 크기의 Max
	float	LifeTimeMax;	// 생성될 파티클이 살아있을 최대시간

	Vector4	ColorMin;		// 생성될 파티클의 색상 Min
	Vector4	ColorMax;		// 생성될 파티클의 색상 Max

	float	SpeedStart;		// 파티클의 최소 이동속도
	float	SpeedEnd;		// 파티클의 최대 이동속도
	int		Move;			// 이동을 하는지 안하는지
	int		Gravity;		// 중력 적용을 받는지 안받는지

	Vector3	MoveDir;		// 이동을 한다면 기준이 될 이동 방향
	int		DestroyExistingAllLivingParticles;			// 2D용 파티클인지

	Vector3	MoveAngle;	// 이동을 한다면 기준이 될 방향으로부터 x, y, z 에 저장된 각도만큼 틀어진 랜덤한 방향을 구한다.
	int ParticleBounce;

	// 모든 Particle 들에 동일하게 적용할 Rotation Angle
	// ParticleComponent PostUpdate 에서 WorldRot 정보로 세팅해주게 된다.
	Vector3 RotationAngle;
	float	ParticleBounceResistance;

	int ParticleShapeType;
	float GenerateRadius;
	int IsLoopGenerateRing;
	int SpecialMoveDirType;
	// int     ParticleBounce;
	// float	 ParticleBounceResistance;

	float AlphaEnd;
	float AlphaStart;
	int IsLifeTimeLinearFromCenter;
	int SeperateLinerRotate;

	int UVMoveEnable;
	int UVRowN;
	int UVColN;
	int ApplyLinearEmissiveChange;

	// 각 Particle 별로 다르게 Rotation Angle을 주는 경우
	Vector3 MinSeperateRotAngle;
	// 한번에 처음 SpawnCount 개수만큼 생성되게 세팅하기 (확 생성되고, 확 사라지고)
	// SpawnTime 이 지나면 새로 만들어지거나 하는 효과 X
	int DisableNewAlive;

	Vector3 MaxSeperateRotAngle;
	int AlphaLinearFromCenter;

	// Particle Component 상에서 적용하는 Scale 정보
	// Vector3 CommonWorldScale;
	Vector3 StartEmissiveColor;
	// 진행 방향으로 점점 UV Clipping 되는 효과 구현하기 
	int LinearUVClippingEnable;

	// Vector3 CommonParticleComponentWorldPos;
	Vector3 EndEmissiveColor;
	int SpeedChangeMethod; // Linear, 지수 분포

	int ApplyNoiseTexture; // Pixel Shader 에서 매순간 Noise Texture 로 부터, Sampling 을 해서 Color, Alpha 값 등을 바꾸는 것
	float NoiseTextureApplyRatio; // 사라지기 시작하는 Ratio 범위 
	int LinearClippingReverseDir; // Liner UV Clipping 방향 반대
	int FollowRealTimeParticleComponentPos; // 실시간으로 Particle Component 의 위치를 따라가게 할 것인가 

	Vector3 ParticleEmptyInfo1;
	int RotateXYDistFromCenter; // 원뿔 모양을 만들어내기 위해서, 가운데 생성 지점으로부터 떨어져 있는 구간을 고려해서 Rotate 시켜주기 

	Vector3 ParticleEmptyInfo2;
	float ParticleInfo2;
};

struct	ParticleTempValCBuffer
{
	Vector3 CommonWorldScale;
	int ResetParticleSharedInfoSumSpawnCnt;

	Vector3 CommonParticleComponentWorldPos; //
	int ParticleEmpty1;

	Vector3 RotatedAxisX;
	int ParticleEmpty2;

	Vector3 RotatedAxisY;
	int ParticleEmpty3;

	Vector3 RotatedAxisZ;
	int ParticleEmpty4;
};

struct ParticleInfo
{
	Vector3	WorldPos;
	Vector3	Dir;
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

	Vector3 SeperateRotAngleOffset;
	Vector3 FinalSeperateRotAngle;

	float  InitLocalPosY;
	Vector3 InitParticleComponentWorldPos;

	float DestroyedDuringAlive;
	float SingleInfoEmpty1;
	float SingleInfoEmpty2;
	Vector3 SingleEmptyInfo3;
};

struct ParticleInfoShared
{
	unsigned int	SpawnEnable;
	unsigned int	SpawnCountMax;

	// 지금까지 생성된 Particle 개수
	unsigned int	CurrentSpawnCountSum;

	// 한번에 처음 SpawnCount 개수만큼 생성되게 세팅하기 
	// SpawnTime 이 지나면 새로 만들어지거나 하는 효과 X
	int DisableNewAlive;

	Vector3	ScaleMin;
	Vector3	ScaleMax;

	Vector4	ColorMin;
	Vector4	ColorMax;

	Vector3	CommonRelativeScale;

	int		GravityEnable;

	Vector3  CommonRotationAngle;
	Vector3  SeperateMinRotAngle;
	Vector3  SeperateMaxRotAngle;

	Vector3  CommonParticleCompWorldPos;

	float MaxDistFromCenter;

	float  PrevRingAngle;

	int    UVMoveEnable;
	int    UVRowN;
	int    UVColN;

	Vector3 ShareEmptyInfo1;
	Vector3 ShareEmptyInfo2;
};


enum class ParticleSpeedChangeMethod
{
	Linear = 1,
	Exponential,  // 속도 변화가 점점 크게
	Log, /// 속도 변화가 점점 작게 
	Max
};



enum class ParitcleShapeType
{
	YUpDirRing = 0, // 위를 바라보는 형태로 즉, Y 를 바라보는 형태로 Ring 생성
	Circle,
	ZMinusDirRing, // 사용자 정면을 바라보는 형태로 Ring 생성
	Sphere,
	Max
};

enum class ParticleSpecialMoveDir
{
	XZSpread = 0, // xz 평명 방향으로 이동 y는 0
	XYSpread, // xy 평명 방향으로 이동 z 는 0
	Max
};
