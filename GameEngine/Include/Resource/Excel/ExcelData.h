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
	
	// Label(Column) ����
	void AddLabel(const std::string& label);

	// Row ����
	void AddRow(const std::string& rowName);

	// Row ����
	bool DeleteRow(const std::string& name);

	// Row �̸� ����
	bool RenameRow(const std::string& prevName, const std::string& changeName);

	// Row ���� or ����
	void SetData(const std::string& name, const std::vector<std::string>& data);

	// Row�� Label ������ Ư�� ������ ����
	void SetData(const std::string& name, const std::string& label, const std::string& data);
	void SetData(const std::string& name, const std::string& label, const char* data);
	void SetData(const std::string& name, const std::string& label, int data);
	void SetData(const std::string& name, const std::string& label, float data);

	// Bool ������ ��� "TRUE"�� true, "FALSE"�� false�� �ν�
	void SetData(const std::string& name, const std::string& label, bool data);

	// ��ü ����
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

	// Label �̸� (Column �̸�) ���� ���� ����
	const std::vector<std::string>& GetLabels() const
	{
		return m_vecLabel;
	}

	// Row�� �̸��鸸 ���� ���� ����
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

	// Row �̸��� Label �̸����� Data�˻� : String���·� ���ϵǴ� ���� ���ϴ� ���·� ĳ�����Ͽ� ���
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

	// Label �̸���
	std::vector<std::string> m_vecLabel;

	// ���� Excel Data����
	Table m_Table;

	// ���� ���� ������ ���� ����
	// �Ϲ������� �Ű澵 �ʿ� ����
	std::vector<Row*> m_PushOrderedData;
};

