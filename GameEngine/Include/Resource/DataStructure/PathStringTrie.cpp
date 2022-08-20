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

void CPathStringTrie::insert(const char* key)
{
	// 문자열의 끝이라면
	if (*key == '\0')
	{
		m_Finish = true;
		return;
	}

	// 알파벳(26) + 숫자(10) + '\'(1) + ':' (1) + '_'(1)
	int curr;

	if (*key == '\\')
		curr = 36;

	// int curr = *key - 'a';

	if (!m_Next[curr])
		m_Next[curr] = new CPathStringTrie;

	m_Next[curr]->insert(key + 1);
}

bool CPathStringTrie::find(const char* key)
{
	if (*key == '\0')


	return false;
}
