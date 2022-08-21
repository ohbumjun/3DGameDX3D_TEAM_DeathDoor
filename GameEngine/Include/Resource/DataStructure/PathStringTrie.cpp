#include "PathStringTrie.h"

CPathStringTrie::CPathStringTrie() :
	m_Finish(false)
{
	memset(m_Next, 0, sizeof(m_Next));
}

CPathStringTrie::~CPathStringTrie()
{
	for (int i = 0; i < PathStringTrieSize; ++i)
	{
		SAFE_DELETE(m_Next[i]);
	}
}

void CPathStringTrie::insert(char* key)
{
	// ���ڿ��� ���̶��
	if (*key == '\0')
	{
		m_Finish = true;
		return;
	}

	// �ҹ��� ���ĺ�(26) + �빮�� ���ĺ�(26) + ����(10) + '\'(1) + ':' (1) + '_'(1)
	int curr = -1;

	if (*key == '\\')
		curr = 62;
	else if (*key == ':')
		curr = 63;
	else if (*key == '_')
		curr = 64;
	else if (*key == '.')
		curr = 65;
	else if (*key == '-')
		curr = 66;
	else if (isdigit(*key))
		curr = 52 + (*key - '0');
	else if (isupper(*key))
		curr = 26 + *key - 'A';
	else if (islower(*key))
		curr = *key - 'a';

	// ���� �߿��� �ϳ��� �ɷ��� �Ѵ�.
	if (curr == -1)
		assert(false);

	if (!m_Next[curr])
		m_Next[curr] = new CPathStringTrie;

	m_Next[curr]->insert(key + 1);
}

bool CPathStringTrie::find(char* key)
{
	// finish ��� ���� true
	if (m_Finish)
		return true;

	if (*key == '\0')
		return false;

	int curr = -1;

	if (*key == '\\')
		curr = 62;
	else if (*key == ':')
		curr = 63;
	else if (*key == '_')
		curr = 64;
	else if (*key == '.')
		curr = 65;
	else if (*key == '-')
		curr = 66;
	else if (isdigit(*key))
		curr = 52 + (*key - '0');
	else if (isupper(*key))
		curr = 26 + *key - 'A';
	else if (islower(*key))
		curr = *key - 'a';

	if (m_Next[curr] == nullptr)
		return false;

	return m_Next[curr]->find(key + 1);
}
