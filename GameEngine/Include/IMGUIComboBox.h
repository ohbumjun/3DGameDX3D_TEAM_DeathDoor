#pragma once

#include "IMGUIWidget.h"

class CIMGUIComboBox :
	public CIMGUIWidget
{
public:
	CIMGUIComboBox();
	virtual ~CIMGUIComboBox();

protected:
	std::vector<std::string>	m_vecItem;
	std::vector<std::string>	m_vecItemUTF8;
	std::string	m_PrevViewName;
	bool		m_Select;
	int			m_SelectIndex;
	int			m_PageItemCount;
	bool		m_Sort;
	ImGuiComboFlags	m_Flag;
	std::function<void(int, const char*)>	m_SelectCallback;

	std::function<void(int, const char*, CIMGUIComboBox*)>	m_SelectPointerCallback;

	// OBJ 추가
	std::function<void(const std::string&)>	m_SelectIndexCallback;
public:
	const std::string& GetItem(int Index)
	{
		return m_vecItem[Index];
	}

	const std::string& GetSelectItem()	const
	{
		return m_vecItem[m_SelectIndex];
	}

	int GetSelectIndex()	const
	{
		return m_SelectIndex;
	}

	int GetItemCount()	const
	{
		return (int)m_vecItem.size();
	}

	size_t GetItemIndex(const std::string& Name)	const
	{
		size_t Count = m_vecItem.size();

		for (size_t i = 0; i < Count; ++i)
		{
			if (m_vecItem[i] == Name)
				return i;
		}

		return -1;
	}

	bool IsSelect() const
	{
		return (m_SelectIndex != -1);
	}

public:
	void AddItem(const std::string& Item)
	{
		m_vecItem.push_back(Item);

		wchar_t	wItem[1024] = {};
		char	ItemUTF8[1024] = {};

		int	Length = MultiByteToWideChar(CP_ACP, 0, Item.c_str(), -1, 0, 0);
		MultiByteToWideChar(CP_ACP, 0, Item.c_str(), -1, wItem, Length);

		// UTF8로 변환한다.
		Length = WideCharToMultiByte(CP_UTF8, 0, wItem, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_UTF8, 0, wItem, -1, ItemUTF8, Length, 0, 0);

		m_vecItemUTF8.push_back(ItemUTF8);

		if (m_Sort)
		{
			std::sort(m_vecItem.begin(), m_vecItem.end());
			std::sort(m_vecItemUTF8.begin(), m_vecItemUTF8.end());
		}
	}

	void SetPageItemCount(int Count)
	{
		m_PageItemCount = Count;
	}

	void DeleteItem(int Index)
	{
		auto	iter = m_vecItem.begin() + Index;

		m_vecItem.erase(iter);

		auto	iter1 = m_vecItemUTF8.begin() + Index;

		m_vecItemUTF8.erase(iter1);
	}

	void Clear()
	{
		m_vecItem.clear();
		m_vecItemUTF8.clear();
	}

	bool Empty()
	{
		return m_vecItem.empty();
	}

	void Sort(bool bSort)
	{
		m_Sort = bSort;

		if (m_Sort)
		{
			std::sort(m_vecItem.begin(), m_vecItem.end());
			std::sort(m_vecItemUTF8.begin(), m_vecItemUTF8.end());
		}
	}

	void AddFlag(ImGuiComboFlags_ Flag)
	{
		m_Flag |= Flag;
	}

	void SetSelectIndex(int Index)
	{
		m_SelectIndex = Index;
		m_PrevViewName = m_vecItemUTF8[Index];

		if (m_SelectIndexCallback && Index > 0 && Index < (int)m_vecItemUTF8.size())
		{
			m_SelectIndexCallback(m_vecItemUTF8[Index]);
		}
	}

	int FindTargetTextIndex(const std::string& Name)
	{
		int TotalSize = (int)m_vecItemUTF8.size();

		for (int i = 0; i < TotalSize; ++i)
		{
			if (m_vecItemUTF8[i] == Name)
				return i;
		}

		return -1;
	}

public:
	virtual bool Init();
	virtual void Render();

public:
	template <typename T>
	void SetSelectPointerCallback(T* Obj, void(T::* Func)(int, const char*, CIMGUIComboBox*))
	{
		m_SelectPointerCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	template <typename T>
	void SetSelectCallback(T* Obj, void(T::* Func)(int, const char*))
	{
		m_SelectCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetSelectIndexCallback(T* Obj, void(T::* Func)(const std::string& Name))
	{
		m_SelectIndexCallback = std::bind(Func, Obj, std::placeholders::_1);
	}
};
