#pragma once

#include "GameInfo.h"

class CSemaPhoreSync
{
public:
	CSemaPhoreSync(HANDLE* SemaPhore)
	{
		m_SemaPhore = SemaPhore;
		WaitForSingleObject(*m_SemaPhore, INFINITE);
	}

	~CSemaPhoreSync()
	{
		ReleaseSemaphore(
			// 반환하고자 하는 세마포어의 핸들
			*m_SemaPhore,
			// 반환한다는 것은, 세마 포어 카운트의 증가를 의미한다.
			// 해당 인자를 통해 증가시킬 값의 크기를 결정할 수 있다.
			// 만약 세마포어 생성 시 결정한 최대 카운트 값(CreateSemaphore 3번째 인자)
			// 를 전달하게 되면, 카운트는 변경되지 않고 FALSE 만 반환
			1,
			// 변경되기 전 세마포어 카운트 값을 저장할 변수를 지정
			NULL
		);
	}

private:
	// CRITICAL_SECTION* m_Crt;
	HANDLE* m_SemaPhore;
};

