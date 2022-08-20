#include "EngineUtil.h"
#include "PathManager.h"
#include <filesystem>
#include "Resource/Mesh/AnimationMesh.h"


namespace fs = std::filesystem;

std::string CEngineUtil::LightTypeToString(Light_Type Type)
{
	std::string ret = "";
	switch (Type)
	{
	case Light_Type::Dir:
		ret = "Dir";
		break;
	case Light_Type::Point:
		ret = "Point";
		break;
	case Light_Type::Spot:
		ret = "Spot";
		break;
	}
	return ret;
}

Light_Type CEngineUtil::StringToLightType(const std::string& TypeString)
{
	if (TypeString == "Dir")
	{
		return Light_Type::Dir;
	}
	else if (TypeString == "Point")
	{
		return Light_Type::Point;
	}
	else if (TypeString == "Spot")
	{
		return Light_Type::Spot;
	}
	return (Light_Type)(-1);
}

std::string CEngineUtil::FogTypeToString(Fog_Type Type)
{
	std::string ret = "";
	switch (Type)
	{
	case Fog_Type::Depth:
		ret = "Depth";
		break;
	case Fog_Type::Y:
		ret = "Y";
		break;
	}
	return ret;
}

Fog_Type CEngineUtil::StringToFogType(const std::string& TypeString)
{
	if (TypeString == "Depth")
	{
		return Fog_Type::Depth;
	}
	else if (TypeString == "Y")
	{
		return Fog_Type::Y;
	}
	return (Fog_Type)(-1);
}

std::string CEngineUtil::ToonShaderTypeToString(ToonShaderType Type)
{
	std::string ret = "";
	switch (Type)
	{
	case ToonShaderType::Default:
		ret = "Default";
		break;
	case ToonShaderType::Easy:
		ret = "Easy";
		break;
	case ToonShaderType::Light:
		ret = "Light";
		break;
	case ToonShaderType::Warm:
		ret = "Warm";
		break;
	}
	return ret;
}

ToonShaderType CEngineUtil::StringToToonShaderType(const std::string& TypeString)
{
	if (TypeString == "Default")
	{
		return ToonShaderType::Default;
	}
	else if (TypeString == "Easy")
	{
		return ToonShaderType::Easy;
	}
	else if (TypeString == "Light")
	{
		return ToonShaderType::Light;
	}
	else if (TypeString == "Warm")
	{
		return ToonShaderType::Warm;
	}
	return (ToonShaderType)(-1);
}

std::string CEngineUtil::PaperBurnEndEventTypeToString(PaperBurnEndEvent Type)
{
	std::string Ret = "";

	switch (Type)
	{
	case PaperBurnEndEvent::None:
		Ret = "None";
		break;
	case PaperBurnEndEvent::Destroy:
		Ret = "Destroy";
		break;
	case PaperBurnEndEvent::Disable:
		Ret = "Disable";
		break;
	case PaperBurnEndEvent::Return:
		Ret = "Return";
		break;
	case PaperBurnEndEvent::Reset:
		Ret = "Reset";
		break;
	}

	return Ret;
}

PaperBurnEndEvent CEngineUtil::StringToPaperBurnEndEvent(const std::string& TypeString)
{
	PaperBurnEndEvent Type;

	if (TypeString == "None")
	{
		Type = PaperBurnEndEvent::None;
	}
	else if (TypeString == "Destroy")
	{
		Type = PaperBurnEndEvent::Destroy;
	}
	else if (TypeString == "Disable")
	{
		Type = PaperBurnEndEvent::Disable;
	}
	else if (TypeString == "Return")
	{
		Type = PaperBurnEndEvent::Return;
	}
	else if (TypeString == "Reset")
	{
		Type = PaperBurnEndEvent::Reset;
	}

	return Type;
}

std::optional<std::string> CEngineUtil::CheckAndExtractFullPathOfTargetFile(std::string_view PathName, std::string_view TargetFileName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName.data());

	if (!Info)
		return  std::nullopt;

	char FolderName[MAX_PATH] = {};

	strcpy_s(FolderName, Info->PathMultibyte);

	fs::path TargetFolder(FolderName);

	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(TargetFolder))
	{
		const std::string& FileName = FilterFileName(entry.path().u8string());

		if (strcmp(FileName.c_str(), TargetFileName.data()) == 0)
		{
			return entry.path().u8string();
		}
	}

	return std::nullopt;
}

bool CEngineUtil::IsFileExistInDir(const std::string& PathName, const std::string& TargetFileName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (!Info)
		return  false;

	char FolderName[MAX_PATH] = {};

	strcpy_s(FolderName, Info->PathMultibyte);

	fs::path TargetFolder(FolderName);

	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(TargetFolder))
	{
		const std::string& FileName = FilterFileName(entry.path().u8string());

		if (strcmp(FileName.c_str(), TargetFileName.c_str()) == 0)
		{
			return true;
		}
	}

	return false;
}

bool CEngineUtil::CopyFileToOtherDirectory(const PathInfo* CurrentPathInfo, const PathInfo* TargetPathInfo, const std::string& FileName)
{
	if (!CurrentPathInfo || !TargetPathInfo)
		return false;

	// 현재 디렉토리 정보를 만든다.
	std::string CurrentPath = CurrentPathInfo->PathMultibyte;
	CurrentPath.append(FileName);

	std::string TargetPath = TargetPathInfo->PathMultibyte;
	TargetPath.append(FileName);

	fs::path FromFilePath(CurrentPath);
	fs::path ToFilePath(TargetPath);

	// 둘중 하나라도 파일이 아니라면 X (둘중 하나라도 Directory라면)
	if (fs::is_directory(FromFilePath) || fs::is_directory(ToFilePath))
		return false;

	fs::copy(FromFilePath, ToFilePath);

	return true;
}

bool CEngineUtil::GetPathInfoBeforeFileName(const std::string& FilePath, std::string& ExtractedPathInfo)
{
	int FilePathLength = (int)FilePath.size();

	for (int i = FilePathLength - 1; i >= 0; --i)
	{
		if (FilePath[i] == '\\')
		{
			ExtractedPathInfo = FilePath.substr(0, (size_t)i + 1);
			return true;
		}
	}

	ExtractedPathInfo = FilePath;

	return true;
}

std::string CEngineUtil::ExtractFilePathFromFullPath(const std::string& FullPath, const std::string& PathName)
{
	const PathInfo* Info = nullptr;
	Info = CPathManager::GetInst()->FindPath(PathName);

	if (!Info)
	{
		return "";
	}

	std::string Path = Info->PathMultibyte;

	// Path에서 가장 말단 폴더 이름을 찾는다.
	// Ex) Editor\\Mesh\\ -> Mesh를 찾음
	std::string EndDir;
	int PathSize = (int)Path.length();

	int EndDirStrStart;
	for (int i = PathSize - 3; i >= 0; --i)
	{
		if (Path[i] == '\\' || Path[i] == '\/')
		{
			EndDirStrStart = i + 1;
			break;
		}
	}

	// \\ 를 제외한 문자열만 추출 (ex) Mesh\\ -> Mesh)
	EndDir = Path.substr(EndDirStrStart, PathSize - 1 - EndDirStrStart);

	int FullPathLen = (int)FullPath.length();
	int SlashCount = 0;
	int DirIndcies[2] = {};
	std::string SameDirName;

	// FullPath에서 \\ 와 \\ 사이의 폴더 혹은 파일 이름만 검색해서
	// 위에서 추출한 말단 디렉토리와 같은 이름의 디렉토리가 있는 경우 그 뒤를 파일 이름으로 추출
	int FilePathStartIdx = 0;
	for (int i = FullPathLen - 1; i >= 0; --i)
	{
		if (FullPath[i] == '\\' || FullPath[i] == '\/')
		{
			if (SlashCount == 1)
			{
				DirIndcies[1] = i + 1;
				SlashCount = 0;

				if (FullPath[i - 1] == '\\' || FullPath[i - 1] == '\/')
				{
					--i;
				}

				SameDirName = FullPath.substr(DirIndcies[1], DirIndcies[0] - DirIndcies[1] + 1);

				if (SameDirName == EndDir)
				{
					if (FullPath[DirIndcies[0] + 1] != '\\' && FullPath[DirIndcies[0] + 1] != '\/')
					{
						FilePathStartIdx = DirIndcies[0] + 1;
					}
					else
					{
						FilePathStartIdx = DirIndcies[0] + 2;
					}

					break;
				}
				else
				{
					DirIndcies[0] = DirIndcies[1] - 2;
					SlashCount = 1;
				}
			}
			else
			{
				if (FullPath[i - 1] == '\\' || FullPath[i - 1] == '\/')
				{
					--i;
				}
				DirIndcies[0] = i - 1;
				++SlashCount;
			}
		}
	}

	std::string FilePath = FullPath.substr(FilePathStartIdx, FullPathLen - FilePathStartIdx + 1);

	return FilePath;
}


bool CEngineUtil::GetFileNameAfterSlash(const std::string& FilePath, std::string& ExtractedFileName)
{
	int FilePathLength = (int)FilePath.size();

	for (int i = FilePathLength - 1; i >= 0; --i)
	{
		if (FilePath[i] == '\\')
		{
			ExtractedFileName = FilePath.substr((size_t)i + 1, FilePath.size());
			return true;
		}
	}

	ExtractedFileName = FilePath;

	return true;
}

bool CEngineUtil::GetFileExt(const std::string& FileName, std::string& ExtractedExt)
{
	size_t i = FileName.find('.');

	if (i != std::string::npos)
	{
		ExtractedExt = FileName.substr(i + 1, FileName.length() - i);
		return true;
	}

	return false;
}

bool CEngineUtil::GetFileNameOnly(const std::string& FullFileName, std::string& ExtractedFileName)
{
	size_t i = FullFileName.find('.');

	if (i != std::string::npos)
	{
		ExtractedFileName = FullFileName.substr(0, i);
		return true;
	}

	return false;
}


bool CEngineUtil::CopyFileToOtherDirectory(const std::string& OriginFullPath, const std::string& TargetFullPath,
	std::string& FinalCopyPath, bool Recursvie)
{
	fs::path originalPath = OriginFullPath;
	fs::path copiedPath = TargetFullPath;

	// 같은 경로라면, _(num) 을 붙여가면서, 해당 경로에 같은 이름이 없을 때까지 반복해가며 새로운 이름으로 만들어낸다.
	// if (originalPath == copiedPath)
	if (fs::exists(copiedPath))
	{
		std::string FolderPathStr;
		std::string FileNameStr;
		std::string FileNameOnlyStr;
		std::string FileExtStr;

		GetPathInfoBeforeFileName(OriginFullPath, FolderPathStr);
		GetFileNameAfterSlash(OriginFullPath, FileNameStr);
		GetFileExt(FileNameStr, FileExtStr);
		GetFileNameOnly(FileNameStr, FileNameOnlyStr);

		fs::path FolderPathInfo = FolderPathStr;

		int ExtraNameNum = 0;

		while (true)
		{
			std::string SavedCopyPath;
			SavedCopyPath.reserve(FolderPathStr.length() * 2);

			SavedCopyPath.append(FolderPathStr);
			SavedCopyPath.append("\\");
			SavedCopyPath.append(FileNameOnlyStr);
			SavedCopyPath.append("_" + std::to_string(ExtraNameNum));
			SavedCopyPath.append("." + FileExtStr);

			// 만약 현재 폴더 경로에 해당 이름으로 된 파일이 없다면
			if (fs::exists(SavedCopyPath) == false)
			{
				copiedPath = SavedCopyPath;
				break;
			}

			++ExtraNameNum;
		}
	}

	if (!Recursvie)
	{
		fs::copy(originalPath, copiedPath);
	}
	else
	{
		fs::copy(originalPath, copiedPath, fs::copy_options::recursive);
	}

	FinalCopyPath = copiedPath.string();

	return true;
}

void CEngineUtil::CheckAndMakeDirectory(const PathInfo* Info)
{
	if (!Info)
		return;

	fs::path dir(Info->PathMultibyte);

	// 이미 존재하면 return
	if (fs::exists(dir))
		return;

	// 해당 디렉토리까지 타고 들어가는, 부모 디렉토리들까지 한번에 만들어준다.
	fs::create_directories(dir);
}

std::string CEngineUtil::FilterFileName(const std::string& FullPath)
{
	size_t len = FullPath.length();

	std::string FileName;

	for (size_t i = len - 1; i >= 0; --i)
	{
		if (FullPath[i] == '\\' || FullPath[i] == '/')
		{
			FileName = FullPath.substr(i + 1);
			return FileName;
		}
	}

	return FileName;
}

std::string CEngineUtil::CollisionChannelToString(Collision_Channel eChannnel)
{
	std::string Out;

	switch (eChannnel)
	{
	case Collision_Channel::Object:
		Out = "Object";
		break;
	case Collision_Channel::Player:
		Out = "Player";
		break;
	case Collision_Channel::Monster:
		Out = "Monster";
		break;
	case Collision_Channel::PlayerAttack:
		Out = "PlayerAttack";
		break;
	case Collision_Channel::MonsterAttack:
		Out = "MonsterAttack";
		break;
	case Collision_Channel::MapObject:
		Out = "MapObject";
		break;
	case Collision_Channel::Custom2:
		Out = "Custom2";
		break;
	case Collision_Channel::Custom3:
		Out = "Custom3";
		break;
	case Collision_Channel::Custom4:
		Out = "Custom4";
		break;
	case Collision_Channel::Custom5:
		Out = "Custom5";
		break;
	case Collision_Channel::Custom6:
		Out = "Custom6";
		break;
	case Collision_Channel::Custom7:
		Out = "Custom7";
		break;
	case Collision_Channel::Custom8:
		Out = "Custom8";
		break;
	case Collision_Channel::Custom9:
		Out = "Custom9";
		break;
	case Collision_Channel::Custom10:
		Out = "Custom10";
		break;
	case Collision_Channel::Custom11:
		Out = "Custom11";
		break;
	case Collision_Channel::Custom12:
		Out = "Custom12";
		break;
	case Collision_Channel::Custom13:
		Out = "Custom13";
		break;
	case Collision_Channel::Custom14:
		Out = "Custom14";
		break;
	case Collision_Channel::Custom15:
		Out = "Custom15";
		break;
	case Collision_Channel::Custom16:
		Out = "Custom16";
		break;
	default:
		assert(false);
		break;
	}

	return Out;
}

Collision_Channel CEngineUtil::StringToCollisionChannel(const std::string& String)
{
	if (String == "Object")
	{
		return Collision_Channel::Object;
	}
	else if (String == "Player")
	{
		return Collision_Channel::Player;
	}
	else if (String == "PlayerAttack")
	{
		return Collision_Channel::PlayerAttack;
	}
	else if (String == "Monster")
	{
		return Collision_Channel::Monster;
	}
	else if (String == "MonsterAttack")
	{
		return Collision_Channel::MonsterAttack;
	}
	else if (String == "MapObject")
	{
		return Collision_Channel::MapObject;
	}
	//else if (String == "MonsterPathFind")
	//{
	//	return Collision_Channel::MonsterPathFind;
	//}
	else if (String == "Custom3")
	{
		return Collision_Channel::Custom3;
	}
	else if (String == "Custom4")
	{
		return Collision_Channel::Custom4;
	}
	else if (String == "Custom5")
	{
		return Collision_Channel::Custom5;
	}
	else if (String == "Custom6")
	{
		return Collision_Channel::Custom6;
	}
	else if (String == "Custom7")
	{
		return Collision_Channel::Custom7;
	}
	else if (String == "Custom8")
	{
		return Collision_Channel::Custom8;
	}
	else if (String == "Custom9")
	{
		return Collision_Channel::Custom9;
	}
	else if (String == "Custom10")
	{
		return Collision_Channel::Custom10;
	}
	else if (String == "Custom11")
	{
		return Collision_Channel::Custom11;
	}
	else if (String == "Custom12")
	{
		return Collision_Channel::Custom12;
	}
	else if (String == "Custom13")
	{
		return Collision_Channel::Custom14;
	}
	else if (String == "Custom14")
	{
		return Collision_Channel::Custom14;
	}
	else if (String == "Custom15")
	{
		return Collision_Channel::Custom15;
	}
	else if (String == "Custom16")
	{
		return Collision_Channel::Custom16;
	}
	return (Collision_Channel)(-1);
}

Collision_Interaction CEngineUtil::StringToCollisionInteraction(const std::string& String)
{
	if (String == "Collision")
	{
		return Collision_Interaction::Collision;
	}
	else if (String == "CollisionRigid")
	{
		return Collision_Interaction::CollisionRigid;
	}
	else
	{
		return Collision_Interaction::Ignore;
	}
}

std::string CEngineUtil::CollisionInteractionToString(Collision_Interaction eInteraction)
{
	switch (eInteraction)
	{
	case Collision_Interaction::Ignore:
		return "Ignore";
	case Collision_Interaction::Collision:
		return "Collision";
	case Collision_Interaction::CollisionRigid:
		return "CollisionRigid";
	}

	return "";
}

std::string CEngineUtil::BoolToString(bool Bool)
{
	std::string Ret = Bool ? "TRUE" : "FALSE";
	return Ret;
}

bool CEngineUtil::StringToBool(const std::string& Str)
{
	if (Str == "TRUE")
	{
		return true;
	}

	return false;
}

void CEngineUtil::CalculateBazierTargetPoses(const Vector3& D1, const Vector3& D2, 
	const Vector3& D3, const Vector3& D4, std::queue<Vector3>& queuePoses, int DetailNum)
{
	// 먼저 기존에 채워져있던 Pos 정보를 전부 지워준다.
	while (!queuePoses.empty())
	{
		queuePoses.pop();
	}

	for (int i = 1; i < DetailNum; ++i)
	{
		float amt = i / (float)DetailNum;

		// 선형 보간 법
		const Vector3& Q1 = D1 * (1 - amt) + D2 * amt;
		const Vector3& Q2 = D2 * (1 - amt) + D3 * amt;
		const Vector3& Q3 = D3 * (1 - amt) + D4 * amt;

		const Vector3& R1 = Q1 * (1 - amt) + Q2 * amt;
		const Vector3& R2 = Q2 * (1 - amt) + Q3 * amt;

		const Vector3& TargetPos = R1 * (1 - amt) + R2 * amt;

		queuePoses.push(TargetPos);
	}

	// 처음 한개를 뽑아둔다.
	/*
	if (!m_queueBazierMovePos.empty())
	{
		Vector3 NextPos = m_queueBazierMovePos.front();
		m_queueBazierMovePos.pop();

		m_ParticleNextMovePos = NextPos;

		m_ParticleMoveDir = m_ParticleNextMovePos - GetWorldPos();
		m_ParticleMoveDir.Normalize();

		const Vector3& CurrentWorldPos = GetWorldPos();

		m_BazierMoveTargetDist = m_ParticleNextMovePos.Distance(CurrentWorldPos);

		m_BazierMoveCurDist = 0.f;
	}
	*/
}

void CEngineUtil::CalculateBazierTargetPoses(const Vector3& D1, const Vector3& D2, const Vector3& D3, const Vector3& D4, 
	std::vector<Vector3>& vecPoses, int DetailNum)
{
	// 먼저 기존에 채워져있던 Pos 정보를 전부 지워준다.
	vecPoses.clear();

	for (int i = 1; i < DetailNum; ++i)
	{
		float amt = i / (float)DetailNum;

		// 선형 보간 법
		const Vector3& Q1 = D1 * (1 - amt) + D2 * amt;
		const Vector3& Q2 = D2 * (1 - amt) + D3 * amt;
		const Vector3& Q3 = D3 * (1 - amt) + D4 * amt;

		const Vector3& R1 = Q1 * (1 - amt) + Q2 * amt;
		const Vector3& R2 = Q2 * (1 - amt) + Q3 * amt;

		const Vector3& TargetPos = R1 * (1 - amt) + R2 * amt;

		vecPoses.push_back(TargetPos);
	}

	// 처음 한개를 뽑아둔다.
/*
if (!m_queueBazierMovePos.empty())
{
	Vector3 NextPos = m_queueBazierMovePos.front();
	m_queueBazierMovePos.pop();

	m_ParticleNextMovePos = NextPos;

	m_ParticleMoveDir = m_ParticleNextMovePos - GetWorldPos();
	m_ParticleMoveDir.Normalize();

	const Vector3& CurrentWorldPos = GetWorldPos();

	m_BazierMoveTargetDist = m_ParticleNextMovePos.Distance(CurrentWorldPos);

	m_BazierMoveCurDist = 0.f;
}
*/
}

Vector3 CEngineUtil::QuarternionToEulerAngles(const XMVECTOR& Qut)
{
	Vector3 Angle;

	Vector4 q;
	q.Convert(Qut);

	// roll (x-axis rotation)
	// double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	Angle.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		Angle.y = std::copysign(3.14159f / 2, sinp); // use 90 degrees if out of range
	else
		Angle.y = std::asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	Angle.z = std::atan2(siny_cosp, cosy_cosp);

	return Angle;
}


// V1 : 왼쪽 하단 (x,y 둘다 가장 작은 값 )
// V2 : 왼쪽 상단
// V3 : 오른쪽 상단 (x,y 둘다 가장 큰 값)
// V4: 오른쪽 하단
bool CEngineUtil::CheckInsideSquare(const Vector2& V1, const Vector2& V2, const Vector2& V3, const Vector2& V4, const Vector2& TargetPos)
{
	// x 축 (가로 비교)
	if (TargetPos.x < V1.x)
		return false;

	if (TargetPos.x > V3.x)
		return false;
	
	// y 축 (세로 비교)
	if (TargetPos.y > V2.y)
		return false;

	if (TargetPos.y < V1.y)
		return false;

	return true;
}

float CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithBottom(float Bottom, float CurTime, float InitSpeed)
{
	// 밑은 1 보다 큰 값이어야만 한다.
	assert(Bottom > 1);

	// Bottom ^ CurTime + InitSpeed
	return pow(Bottom, CurTime) + InitSpeed;
}

float CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithSpeed(float FullTime, float InitSpeed, float EndSpeed, float CurTime)
{
	// 속도가 0보다 작으면 안된다.
	if (InitSpeed < 0.f || EndSpeed < 0.f)
		assert(false);

	// if (CurTime > FullTime)
	// 	assert(false);

	float Inclination = (EndSpeed - InitSpeed / FullTime);

	// 서서히 증가
	if (Inclination > 0.f)
	{
		float ExponentialBottom = pow(EndSpeed - InitSpeed, 1 / (FullTime * 0.5f));
		return pow(ExponentialBottom, CurTime - FullTime * 0.5f) + InitSpeed;
	}

	// 서서히 감소
	else if (Inclination < 0.f)
	{
		float ExponentialBottom = pow(-1 * (EndSpeed - InitSpeed), 1 / (FullTime * 0.5f));
		float Speed = -1 * pow(ExponentialBottom, CurTime + 0.1f  - FullTime * 0.5f) + InitSpeed;

		if (Speed < 10.f)
		{
			bool i = 1;
		}
		if (Speed <= EndSpeed)
		{
			bool i = 1;
		}

		// 지수 함수는 y 값이 무한대로 감소하므로, 0 아래로 내려가지는 않게 한다.
		if (Speed < 0)
		{
			Speed = 0;
		}

		return Speed;
	}

	// 같은 속도
	// else if (Inclination == 0.f)
	else 
	{
		return InitSpeed;
	}
}
