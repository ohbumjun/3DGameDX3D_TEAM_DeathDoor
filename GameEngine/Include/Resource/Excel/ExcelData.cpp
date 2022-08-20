#include "ExcelData.h"
#include "../../PathManager.h"
#include "../../Engine.h"
#include "../../EngineUtil.h"

#include <fstream>

CExcelData::CExcelData()
{
}

CExcelData::~CExcelData()
{
	{
		auto iter = m_Table.begin();
		auto iterEnd = m_Table.end();

		for (; iter != iterEnd; ++iter)
		{
			SAFE_DELETE(iter->second);
		}

		m_Table.clear();
	}
	{
		size_t size = m_PushOrderedData.size();

		for (size_t i = 0; i < size; ++i)
		{
			SAFE_DELETE(m_PushOrderedData[i]);
		}

		m_PushOrderedData.clear();
	}
}

bool CExcelData::Init()
{
	return true;
}

void CExcelData::AddLabel(const std::string& label)
{
	size_t size = m_vecLabel.size();

	for (size_t i = 0; i < size; ++i)
	{
		if (label ==  m_vecLabel[i])
		{
			return;
		}
	}

	m_vecLabel.push_back(label);

	size = m_vecLabel.size();

	// 모든 Row 순회하며 Size재할당
	auto iter = m_Table.begin();
	auto iterEnd = m_Table.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->resize(size);
	}

	auto iterOrdered = m_PushOrderedData.begin();
	auto iterOrderedEnd = m_PushOrderedData.end();

	for (; iterOrdered != iterOrderedEnd; ++iterOrdered)
	{
		(*iterOrdered)->resize(size + 1);
	}
}

void CExcelData::AddRow(const std::string& rowName)
{
	std::vector<std::string>* found = GetRow(rowName);

	if (found)
	{
		return;
	}

	found = new Row;
	found->resize(m_vecLabel.size());
	m_Table.insert(std::make_pair(rowName, found));
	
	std::vector<std::string>* orderedData = new std::vector<std::string>;
	orderedData->push_back(rowName);
	orderedData->resize(m_vecLabel.size() + 1);
	m_PushOrderedData.push_back(orderedData);
}

void CExcelData::SetData(const std::string& name, const std::vector<std::string>& data)
{
	Row* found = GetRow(name);

	if (!found)
	{
		found = new Row;
		m_Table.insert(std::make_pair(name, found));

		// Ordered Data 에는 Row의 Name도 들어간다.
		std::vector<std::string>* orderedData = new std::vector<std::string>;
		orderedData->push_back(name);
		
		m_PushOrderedData.push_back(orderedData);
	}

	found->clear();

	size_t size = data.size();
	for (size_t i = 0; i < size; ++i)
	{
		// 빈 셀일 경우 공백을 넣어준다.
		if (0 == data[i].length())
		{
			found->push_back(" ");
		}
		else
		{
			found->push_back(data[i]);
		}
	}

	std::vector<std::string>* orderedData = findOrderedData(name);

	if (orderedData->size() < size + 1)
	{
		orderedData->resize(size + 1);
	}

	for (size_t i = 1; i < size + 1; ++i)
	{
		(*orderedData)[i] = data[i - 1];
	}
}

void CExcelData::SetData(const std::string& name, const std::string& label, const std::string& data)
{
	Row* found = GetRow(name);

	// name에 해당하는 Row가 없을 경우, 새로 Row를 생성해 넣어준다.
	// label에 해당하는 column에만 데이터를 기록하고, 나머지는 모두 공백으로 채워야 함
	if (!found)
	{
		found = new Row;
		found->resize(m_vecLabel.size());

		// 공백을 넣어 빈 셀로 만든다.
		size_t size = found->size();
		for (size_t i = 0; i < size; ++i)
		{
			(*found)[i] = std::string(" ");
		}
		m_Table.insert(std::make_pair(name, found));

		std::vector<std::string>* orderedData = new std::vector<std::string>;
		orderedData->push_back(name);

		size = found->size();
		for (size_t i = 0; i < size; ++i)
		{
			orderedData->push_back((*found)[i]);
		}
		
		m_PushOrderedData.push_back(orderedData);
	}

	int labelIdx = getLabelIndex(label);
	if (-1 == labelIdx)
	{
		return;
	}

	(*found)[labelIdx] = data;

	std::vector<std::string>* orderedData = findOrderedData(name);
	(*orderedData)[labelIdx + 1] = data;
}

void CExcelData::SetData(const std::string& name, const std::string& label, const char* data)
{
	std::string str = data;
	SetData(name, label, str);
}

void CExcelData::SetData(const std::string& name, const std::string& label, int data)
{
	SetData(name, label, std::to_string(data));
}

void CExcelData::SetData(const std::string& name, const std::string& label, float data)
{
	SetData(name, label, std::to_string(data));
}

void CExcelData::SetData(const std::string& name, const std::string& label, bool data)
{
	std::string str = data ? "TRUE" : "FALSE";
	SetData(name, label, str);
}

bool CExcelData::DeleteRow(const std::string& name)
{
	auto iter = m_Table.find(name);
	
	if (iter == m_Table.end())
	{
		return false;
	}

	SAFE_DELETE(iter->second);
	m_Table.erase(iter);

	auto iterVec = m_PushOrderedData.begin();
	auto iterVecEnd = m_PushOrderedData.end();

	for (; iterVec != iterVecEnd; ++iterVec)
	{
		if ((**iterVec)[0] == name)
		{
			SAFE_DELETE(*iterVec);
			m_PushOrderedData.erase(iterVec);
			break;
		}
	}

	return true;
}

bool CExcelData::RenameRow(const std::string& prevName, const std::string& changeName)
{
	auto iter = m_Table.find(prevName);
	
	if (iter == m_Table.end())
	{
		return false;
	}

	Row* data = iter->second;
	m_Table.erase(iter);
	m_Table.insert(std::make_pair(changeName, data));

	auto iterVec = m_PushOrderedData.begin();
	auto iterVecEnd = m_PushOrderedData.end();

	for (; iterVec != iterVecEnd; ++iterVec)
	{
		if ((**iterVec)[0] == prevName)
		{
			(**iterVec)[0] = changeName;
			break;
		}
	}

	return true;
}

void CExcelData::Clear()
{
	m_vecLabel.clear();

	auto iter = m_Table.begin();
	auto iterEnd = m_Table.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_Table.clear();	
	
	size_t size = m_PushOrderedData.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(m_PushOrderedData[i]);
	}

	m_PushOrderedData.clear();
}

bool CExcelData::SaveCSV(const char* fileName, const std::string& pathName)
{
	const PathInfo* path = CPathManager::GetInst()->FindPath(pathName);

	char fullPath[MAX_PATH] = {};

	if (path)
	{
		strcpy_s(fullPath, path->PathMultibyte);
	}
	strcat_s(fullPath, fileName);

	return SaveCSVFullPath(fullPath);
}

bool CExcelData::SaveCSVFullPath(const char* fullPath)
{
	FILE* fp = nullptr;
	fopen_s(&fp, fullPath, "wt");

	if (!fp)
	{
		assert(false);
		return false;
	}

	char buf[1024] = {};

	// Name
	fputs(m_Name.c_str(), fp);
	fputs("\n", fp);
	
	// Label
	fputs(",", fp);
	size_t labelCount = m_vecLabel.size();
	for (size_t i = 0; i < labelCount; ++i)
	{
		int length = (int)m_vecLabel[i].length();
		strcpy_s(buf, length + 1, m_vecLabel[i].c_str());
		if (i == labelCount - 1)
		{
			strcat_s(buf, "\n");
		}
		else
		{
			strcat_s(buf, ",");
		}
		fputs(buf, fp);
	}

	// Data
	size_t colSize = m_PushOrderedData.size();
	for (size_t i = 0; i < colSize; ++i)
	{
		std::vector<std::string>* row = m_PushOrderedData[i];
		
		size_t rowSize = row->size();

		for (size_t j = 0; j < rowSize; ++j)
		{
			int length = (int)(*row)[j].length();
			strcpy_s(buf, length + 1, (*row)[j].c_str());

			if (j == rowSize - 1)
			{
				strcat_s(buf, "\n");
			}
			else
			{
				strcat_s(buf, ",");
			}
			fputs(buf, fp);
		}
	}

	fclose(fp);
	return true;
}

bool CExcelData::LoadCSV(const char* fileName, const std::string& pathName)
{
	const PathInfo* path = CPathManager::GetInst()->FindPath(pathName);

	char fullPath[MAX_PATH] = {};

	if (path)
	{
		strcpy_s(fullPath, path->PathMultibyte);
	}
	strcat_s(fullPath, fileName);

	return LoadCSVFullPath(fullPath);
}

bool CExcelData::LoadCSVFullPath(const char* fullPath)
{
	FILE* fp = nullptr;

	fopen_s(&fp, fullPath, "rt");

	if (!fp)
	{
		// OBJ : 현재 해당 파일의 Excel 을 열어놓고 있는 상태면, 에러가 난다.
		// MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("현재 Excel 파일이 열려 있는지 확인하세요"), NULL, MB_OK);
		return false;
	}

	char name[64] = {};
	fgets(name, 64, fp);

	int len = (int)strlen(name);
	name[len - 1] = '\0'; // , 제거
	m_Name = name;

	m_Name.erase(std::remove(m_Name.begin(), m_Name.end(), ','), m_Name.end());

	// label
	char buf[1024] = {};
	fgets(buf, 1024, fp);
	char* context = nullptr;
	char* element = strtok_s(buf, ",", &context);
	while (NULL != element)
	{
		m_vecLabel.push_back(element);
		element = strtok_s(NULL, ",", &context);
	}
	// 공백 제거
	size_t labelSize = m_vecLabel.size();
	m_vecLabel[labelSize - 1].erase(std::remove(m_vecLabel[labelSize - 1].begin(), m_vecLabel[labelSize - 1].end(), '\n'),
		m_vecLabel[labelSize-1].end());

	// data
	std::string dataName;
	Row* row = nullptr;
	std::vector<std::string>* rowOrdered = nullptr;
	while (0 == feof(fp))
	{
		row = new Row;
		rowOrdered = new std::vector<std::string>;

		fgets(buf, 1024, fp);
		element = strtok_s(buf, ",", &context);
		dataName = element;
		rowOrdered->push_back(element);
		
		while (NULL != element)
		{
			if (strcmp("\n", element) == 0)
			{
				break;
			}

			element = strtok_s(NULL, ",", &context);

			if (!element)
			{
				break;
			}
			row->push_back(element);
			rowOrdered->push_back(element);
		}

		if (row->size() == 0)
		{
			SAFE_DELETE(row);
			SAFE_DELETE(rowOrdered);
			break;
		}

		// 공백 제거
		size_t labelSize = row->size();
		(*row)[labelSize - 1].erase(std::remove((*row)[labelSize - 1].begin(), (*row)[labelSize - 1].end(), '\n'),
		(*row)[labelSize-1].end());
		labelSize = rowOrdered->size();
		(*rowOrdered)[labelSize - 1].erase(std::remove((*rowOrdered)[labelSize - 1].begin(), (*rowOrdered)[labelSize - 1].end(), '\n'),
		(*rowOrdered)[labelSize-1].end());

		m_Table.insert(std::make_pair(dataName, row));
		m_PushOrderedData.push_back(rowOrdered);
	}
	
	fclose(fp);
	return true;
}

void CExcelData::GetRowNames(std::vector<std::string>& outNames) const
{
	size_t size = m_PushOrderedData.size();

	for (size_t i = 0; i < size; ++i)
	{
		outNames.push_back((*m_PushOrderedData[i])[0]);
	}
}

Row* CExcelData::GetRow(const std::string& name)
{
	auto iter = m_Table.find(name);
	
	if (iter == m_Table.end())
	{
		return nullptr;
	}

	return iter->second;
}

const std::string& CExcelData::FindData(const std::string& name, const std::string& label)
{
	std::vector<std::string>* row = GetRow(name);
	int idx = getLabelIndex(label);

	if (idx == -1)
	{
		return "";
	}

	return (*row)[idx];
}

float CExcelData::FindDataFloat(const std::string& name, const std::string& label)
{
	const std::string& Find = FindData(name, label);

	if (Find.empty())
	{
		return 1.f;
	}

	return std::stof(Find);
}

int CExcelData::FindDataInt(const std::string& name, const std::string& label)
{
	const std::string& Find = FindData(name, label);

	if (Find.empty())
	{
		return -1;
	}

	return std::stoi(Find);
}

bool CExcelData::FindDataBool(const std::string& name, const std::string& label)
{
	std::string Find = FindData(name, label);

	if (Find.empty())
	{
		return false;
	}

	bool Ret = CEngineUtil::StringToBool(Find);

	return Ret;
}

int CExcelData::getLabelIndex(const std::string& label)
{
	int idx = 0;
	int size = (int)m_vecLabel.size();

	for (; idx < size; ++idx)
	{
		if (label == m_vecLabel[idx])
		{
			return idx;
		}
	}

	return -1;
}

std::vector<std::string>* CExcelData::findOrderedData(const std::string& name)
{
	size_t size = m_PushOrderedData.size();

	for (size_t i = 0; i < size; ++i)
	{
		if (name == (*m_PushOrderedData[i])[0])
		{
			return m_PushOrderedData[i];
		}
	}
	return nullptr;
}
