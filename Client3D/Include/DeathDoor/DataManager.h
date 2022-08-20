#pragma once

#include "GameInfo.h"

// csv���� �������� �о���� Manager
class CDataManager
{
private:
	std::unordered_map <std::string, class CExcelData*> m_mapData;

public:
	virtual void Init();

private:
	void ReadObjectData();
	void LoadSoundData();

public:
	ObjectData GetObjectData(const std::string& Key);
	
	class CExcelData* FindData(const std::string& Name);

	DECLARE_SINGLE(CDataManager)
};

