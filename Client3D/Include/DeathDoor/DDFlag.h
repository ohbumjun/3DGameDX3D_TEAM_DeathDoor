#pragma once

enum class DDActionNode
{
	Move,
	Idle,
	NormalAttack,
	Death,
	RotateAttackDirection,
	ReadyToShoot,
	ShootNode,
	CancleShootNode,
	AddFallingFloorCallback,
	Lockstone3TriggerBoxAction,
	FindPath,
	ClimbDown,
	ClimbDownEnd,
	ClimbDownStart,
	ClimbUp,
	ClimbUpEnd,
	ClimbUpStart,
	ClimbPause,
	BossKnightContinueAttack,
	BossKnightFinalAttack,
	BossKnightJumpAttack,
	BossKnightIdle,
	BossKnightMeleeAttack,
	BossKnightSlamEnd,
	BossKnightWalk,
	BossKnightCutScenePlay,
	ClearPathList,
	HitBack,
	PlayerRoll,
	// Monster
	ChasePlayer,
	MeleeAttack,
	MonsterHitBack,
	// BossBetty
	BossBettyAngryAttack,
	BossBettyChangeAttackDir,
	BossBettyCloseAttack,
	BossBettyJumpAttack,
	BossBettySpinAttack,
	BossBettyThrowAttack,
	BossBettyIntro,
	BossBettyIdleBeast,
	// CrowBoss
	CrowBossCutScenePlayNode,
	CrowBossRunNode,
	CrowBossSpinNode,
	CrowBossJump,
	CrowBossFly,
	CrowBossMoveShootSpot,
	CrowBossResetShootState,
	CrowBossShoot,
	CrowBossSlide,
	CrowBossSpitting,
	// SporeBoomer
	SporeBoomerShoot,
	// HeadRoller
	HeadRollerStun,
	HeadRollerRoll,
	// Dodger
	DodgerFirstAttack,
	DodgerSecondAttack,
	DodgerPostAttack,
	DodgerDash,
	// PlagueKnight
	PlagueKnightShoot,
	// Bat
	BatRecognize,
	// Lurker
	LurkerHop,
	LurkerMeleeAttackPrep,
	// Grunt Monster Common
	GruntCommonAttack,
	GruntCommonWalk,
	GruntCommonTrace,
	GruntCommonHit,
	GruntCommonSpawn,
	Max
};

enum class DDConditionNode
{
	MoveInputCheckNode,
	MouseLButtonCheckNode,
	NoInterruptNode,
	AttackTargetCheck,
	MouseRButtonCheckNode,
	MouseRButtonUpCheckNode,
	Lockstone3TriggerBoxHitCheck,
	CheckDetectRange,
	PlayerEnterZoneCheck,
	PlayerEnterTriggerIsEnable,
	MeleeAttackRangeCheck,
	PostAttackDelayCheck,
	AttackCoolTimeCheck,
	PathFindExecuteCheck,
	BossKnightFinalAttackCheck,
	BossKnightJumpAttackRangeCheck,
	BossKnightCutScenePlayCheck,
	BossKnightPlayerEnterZoneCheck,
	BossKnightContinueAttackCheck,
	HPCheck,
	DeathCheck,
	IsCombatCheck,
	PathFindEnableCheck,
	PathListEmptyCheck,
	HitCheckNode,
	RollInputCheck,
	UpdateInputQueue,
	SKeyInputCheck,
	WKeyInputCheck,
	IsClimbingCheck,
	ClimbDownContinueCheck,
	ClimbDownEndCheck,
	ClimbDownStartCheck,
	ClimbUpContinueCheck,
	ClimbUpEndCheck,
	ClimbUpStartCheck,
	ClimbKeyEnableCheck,
	StraightPathCheck,
	// BossBetty
	BossBettyCheckAttackDir,
	BossBettyCheckFarAttackType,
	BossBettyCheckHPState,
	BossBettyCheckThrowAttack,
	BossBettyCheckIdle,
	BossBettyCheckIntro,
	// CrowBoss
	CrowBossCutScenePlayCheck,
	CrowBossPhasePickNode,
	CrowBossBypassCheck,
	CrowBossJumpStartCheck,
	CrowBossLandingCheck,
	CrowBossJumpAttackRangeCheck,
	CrowBossDoneCheck,
	CrowBossFirstShootCheck,
	CrowBossShootCheck,
	CrowBossShootEndCheck,
	CrowBossShootReadyCheck,
	// HeadRoller
	HeadRollerStunCheck,
	// Dodger
	DodgerDashCheck,
	DodgerSecondAttackCheck,
	// Bat
	BatRecognizeStartCheck,
	BatRecognizeEndCheck,
	// Bat
	LurkerHopStartCheck,
	// Grunt Common
	GruntCommonSpawnCheck,
	GruntCommonStartActCheck,
	Max
};

enum class DDSceneComponentType
{
	PlayerNormalAttackCheckCollider,
	EyeLaserComponent,
	PlayerHookComponent,
	PlayerBowComponent,
	LadderCollider,
	CrackedBlockCollider,
	ArrowCollisionFireCollider,
	Max
};

enum class DDObjectComponentType
{
	ObjectDataComponent,
	PlayerDataComponent,
	GameStateComponent,
	MonsterData,
	KnightData,
	BossBettyData,
	GruntCommonData,
	CrowBossData,
	LurkerData,
	MonsterNavAgent,
	ProjectileComponent,
	ArrowComponent,
	TinyCrowDataComponent,
	PlayerBombComponent,
	SporeBoomerData,
	FirePlantData,
	MonsterBulletData,
	HeadRollerData,
	DodgerData,
	TriggerBoxData,
	TriggerFireLamp,
	PlagueKnightData,
	BatData,
	Max
};


enum class HookResult
{
	// 날아가고 있는중
	OnShoot,
	NoCollision,
	Collision,
	CollisionEnd
};

enum class CrowBossShootState
{
	Ready,
	ReadyEnd,
	Teleport,
	Shoot,
	ShootEnd,
	Fly,
	Done,
	Max
};

// TODO : Death Door SceneMode 추가시마다 업데이트
enum class DDSceneModeType
{
	DDSceneMode,
	DDBossSceneMode,
	DDInstanceSceneMode,
	DDPuzzleSceneMode,
	DDLogoSceneMode,
	Max
};

enum DDPlayerRollDirection
{
	None = 0x0,
	Left = 0x1,
	Right = 0x2,
	Up = 0x4,
	Down = 0x8,
	Max = 0x10
};