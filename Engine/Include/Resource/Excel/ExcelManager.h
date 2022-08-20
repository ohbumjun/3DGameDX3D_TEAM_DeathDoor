#pragma once

#include "ExcelData.h"

class CExcelManager
{
	friend class CResourceManager;

private:
	CExcelManager();
	~CExcelManager();

public:
	bool Init();

public:
	bool CreateCSV(const std::string& name);
	CExcelData* FindCSV(const std::string& name);
	bool DeleteCSV(const std::string& name);

public:
	bool SaveCSV(const std::string& name, const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool SaveCSVFullPath(const std::string& name, const char* fullPath);
	bool LoadCSV(const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool LoadCSV(std::string& outName, const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool LoadCSVFullPath(const char* fullPath);
	bool LoadCSVFullPath(std::string& outName, const char* fullPath);

private:
	std::unordered_map<std::string, class CExcelData*> mMapExcelData;
};

