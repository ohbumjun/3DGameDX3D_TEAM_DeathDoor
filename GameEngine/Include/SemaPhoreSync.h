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
			// ��ȯ�ϰ��� �ϴ� ���������� �ڵ�
			*m_SemaPhore,
			// ��ȯ�Ѵٴ� ����, ���� ���� ī��Ʈ�� ������ �ǹ��Ѵ�.
			// �ش� ���ڸ� ���� ������ų ���� ũ�⸦ ������ �� �ִ�.
			// ���� �������� ���� �� ������ �ִ� ī��Ʈ ��(CreateSemaphore 3��° ����)
			// �� �����ϰ� �Ǹ�, ī��Ʈ�� ������� �ʰ� FALSE �� ��ȯ
			1,
			// ����Ǳ� �� �������� ī��Ʈ ���� ������ ������ ����
			NULL
		);
	}

private:
	// CRITICAL_SECTION* m_Crt;
	HANDLE* m_SemaPhore;
};

