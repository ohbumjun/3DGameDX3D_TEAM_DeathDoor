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

		// 세마 포어 생성
		m_Semaphore = CreateSemaphore(
			NULL, // 디폴트 보안 괸리자

			// 해당 값을 기반으로 임계 영역에 접근 가능한 쓰레드의 개수를 제한한다. 
			1,  // 세마 포어 초기값

			// 세마포어가 지닐 수 있는 값의 최대 크기
			// 해당 값이 1 이 될 경우, 뮤텍스와 동일한 기능을 하는
			// 바이너리 세마포어가 구성된다.
			// 기본적으로 2번째 인자로 전달되는 값보다 커야 한다.
			1, // 세마 포어 최대값

			// 세마포어의 이름
			NULL // unamed 세마 포어 구성
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
	int		m_Head;	// 가장 처음 추가된 곳의 이전 인덱스
	int		m_Tail;	// 마지막으로 추가된 곳의 인덱스
	
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

