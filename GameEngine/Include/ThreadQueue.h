#pragma once

#include "GameInfo.h"
#include "SemaPhoreSync.h"
#include <assert.h>

template <typename T, int SIZE = 200>
class CThreadQueue
{
public:
	CThreadQueue()
	{
		m_Size = 0;
		m_Head = 0;
		m_Tail = 0;
		m_Capacity = SIZE + 1;

		// InitializeCriticalSection(&m_Crt);

		// ���� ���� ����
		m_Semaphore = CreateSemaphore(
			NULL, // ����Ʈ ���� ������

			// �ش� ���� ������� �Ӱ� ������ ���� ������ �������� ������ �����Ѵ�. 
			1,  // ���� ���� �ʱⰪ

			// ������� ���� �� �ִ� ���� �ִ� ũ��
			// �ش� ���� 1 �� �� ���, ���ؽ��� ������ ����� �ϴ�
			// ���̳ʸ� ������� �����ȴ�.
			// �⺻������ 2��° ���ڷ� ���޵Ǵ� ������ Ŀ�� �Ѵ�.
			1, // ���� ���� �ִ밪

			// ���������� �̸�
			NULL // unamed ���� ���� ����
		);
	}

	~CThreadQueue()
	{
		CloseHandle(m_Semaphore);
		// DeleteCriticalSection(&m_Crt);
	}

private:
	T		m_Queue[SIZE + 1];
	int		m_Capacity;
	int		m_Size;
	int		m_Head;	// ���� ó�� �߰��� ���� ���� �ε���
	int		m_Tail;	// ���������� �߰��� ���� �ε���
	
	// CRITICAL_SECTION	m_Crt;
	HANDLE 	m_Semaphore;

public:
	void push(const T& data)
	{
		// CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		int	Tail = (m_Tail + 1) % m_Capacity;

		if (Tail == m_Head)
			return;

		m_Queue[Tail] = data;

		m_Tail = Tail;

		++m_Size;
	}

	T& front()
	{
		if (empty())
			assert(false);

		// // CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		int	Head = (m_Head + 1) % m_Capacity;

		return m_Queue[Head];
	}

	void pop()
	{
		if (empty())
			assert(false);

		// CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		m_Head = (m_Head + 1) % m_Capacity;

		--m_Size;
	}

	int size()
	{
		// CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		return m_Size;
	}

	bool empty()
	{
		// CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		return m_Size == 0;
	}

	void clear()
	{
		// CSync	sync(&m_Crt);
		CSemaPhoreSync	sync(&m_Semaphore);

		m_Head = 0;
		m_Tail = 0;
		m_Size = 0;
	}
};

