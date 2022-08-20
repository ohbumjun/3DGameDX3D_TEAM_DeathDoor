#include "ExcelManager.h"
#include "../../PathManager.h"

CExcelManager::CExcelManager()
{
}

CExcelManager::~CExcelManager()
{
	auto iter = mMapExcelData.begin();
	auto iterEnd = mMapExcelData.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	mMapExcelData.clear();
}

bool CExcelManager::Init()
{
	return true;
}

bool CExcelManager::CreateCSV(const std::string& name)
{
	CExcelData* data = FindCSV(name);

	if (data)
	{
		return true;
	}
	
	data = new CExcelData;
	if (!data->Init())
	{
		SAFE_DELETE(data);
		return false;
	}

	data->SetName(name);
	mMapExcelData.insert(std::make_pair(name, data));
	return true;
}

CExcelData* CExcelManager::FindCSV(const std::string& name)
{
	auto iter = mMapExcelData.find(name);

	if (iter == mMapExcelData.end())
	{
		return nullptr;
	}
	return iter->second;
}

bool CExcelManager::DeleteCSV(const std::string& name)
{
	auto iter = mMapExcelData.find(name);

	if (iter == mMapExcelData.end())
	{
		return false;
	}
	
	SAFE_DELETE(iter->second);
	mMapExcelData.erase(iter);
	return true;
}

bool CExcelManager::SaveCSV(const std::string& name, const char* fileName, const std::string& pathName)
{
	CExcelData* excelData = FindCSV(name);

	if (!excelData)
	{
		return false;
	}

	return excelData->SaveCSV(fileName, pathName);
}

bool CExcelManager::SaveCSVFullPath(const std::string& name, const char* fullPath)
{
	CExcelData* excelData = FindCSV(name);

	if (!excelData)
	{
		return false;
	}

	return excelData->SaveCSVFullPath(fullPath);
}

bool CExcelManager::LoadCSV(const char* fileName, const std::string& pathName)
{
	CExcelData* data = new CExcelData;

	if (!data->LoadCSV(fileName, pathName))
	{
		SAFE_DELETE(data);
		return false;
	}

	// 이미 해당 데이터가 로딩되어 있는 경우
	std::string name = data->GetName();
	if (FindCSV(name))
	{
		SAFE_DELETE(data);
		return true;
	}
	
	mMapExcelData.insert(std::make_pair(data->GetName(), data));
	return true;
}

bool CExcelManager::LoadCSV(std::string& outName, const char* fileName, const std::string& pathName)
{
	CExcelData* data = new CExcelData;

	if (!data->LoadCSV(fileName, pathName))
	{
		SAFE_DELETE(data);
		return false;
	}

	// 이미 해당 데이터가 로딩되어 있는 경우
	std::string name = data->GetName();
	if (FindCSV(name))
	{
		outName = name;
		SAFE_DELETE(data);
		return true;
	}
	
	outName = name;
	mMapExcelData.insert(std::make_pair(data->GetName(), data));
	return true;
}

bool CExcelManager::LoadCSVFullPath(const char* fullPath)
{
	CExcelData* data = new CExcelData;

	if (!data->LoadCSVFullPath(fullPath))
	{
		SAFE_DELETE(data);
		return false;
	}
	
	std::string name = data->GetName();

	if (FindCSV(name))
	{
		SAFE_DELETE(data);
		return true;
	}
	
	mMapExcelData.insert(std::make_pair(name, data));

	return true;
}

bool CExcelManager::LoadCSVFullPath(std::string& outName, const char* fullPath)
{
	CExcelData* data = new CExcelData;

	if (!data->LoadCSVFullPath(fullPath))
	{
		SAFE_DELETE(data);
		return false;
	}
	
	std::string name = data->GetName();
	if (FindCSV(name))
	{
		outName = name;
		SAFE_DELETE(data);
		return true;
	}
	
	outName = name;
	mMapExcelData.insert(std::make_pair(name, data));
	return true;
}
