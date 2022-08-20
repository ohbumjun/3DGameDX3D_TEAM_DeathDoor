#pragma once

#include "../../GameInfo.h"

typedef std::vector<std::string> Row;
typedef std::unordered_map<std::string, Row*> Table;

class CExcelData
{
	friend class CExcelManager;

private:
	CExcelData();
	~CExcelData();

public:
	bool Init();
	
	// Label(Column) 삽입
	void AddLabel(const std::string& label);

	// Row 삽입
	void AddRow(const std::string& rowName);

	// Row 삭제
	bool DeleteRow(const std::string& name);

	// Row 이름 변경
	bool RenameRow(const std::string& prevName, const std::string& changeName);

	// Row 삽입 or 변경
	void SetData(const std::string& name, const std::vector<std::string>& data);

	// Row의 Label 지정해 특정 데이터 삽입
	void SetData(const std::string& name, const std::string& label, const std::string& data);
	void SetData(const std::string& name, const std::string& label, const char* data);
	void SetData(const std::string& name, const std::string& label, int data);
	void SetData(const std::string& name, const std::string& label, float data);

	// Bool 변수의 경우 "TRUE"를 true, "FALSE"를 false로 인식
	void SetData(const std::string& name, const std::string& label, bool data);

	// 전체 삭제
	void Clear();

public:
	bool SaveCSV(const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool SaveCSVFullPath(const char* fullPath);
	bool LoadCSV(const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool LoadCSVFullPath(const char* fullPath);

public:
	const std::string& GetName() const
	{
		return m_Name;
	}

	// Label 이름 (Column 이름) 담은 벡터 리턴
	const std::vector<std::string>& GetLabels() const
	{
		return m_vecLabel;
	}

	// Row의 이름들만 담은 벡터 리턴
	void GetRowNames(std::vector<std::string>& outNames) const;

	size_t GetDataSize() const
	{
		return m_Table.size();
	}

	const Table& GetTable() const
	{
		return m_Table;
	}

	Row* GetRow(const std::string& name);

	// Row 이름과 Label 이름으로 Data검색 : String형태로 리턴되는 값을 원하는 형태로 캐스팅하여 사용
	const std::string& FindData(const std::string& name, const std::string& label);
	float FindDataFloat(const std::string& name, const std::string& label);
	int FindDataInt(const std::string& name, const std::string& label);
	bool FindDataBool(const std::string& name, const std::string& label);

public:
	void SetName(const std::string& name)
	{
		m_Name = name;
	}

private:
	int getLabelIndex(const std::string& label);
	std::vector<std::string>* findOrderedData(const std::string& name);
		
private:
	std::string m_Name;

	// Label 이름들
	std::vector<std::string> m_vecLabel;

	// 실제 Excel Data저장
	Table m_Table;

	// 순서 보존 저장을 위한 변수
	// 일반적으로 신경쓸 필요 없음
	std::vector<Row*> m_PushOrderedData;
};

