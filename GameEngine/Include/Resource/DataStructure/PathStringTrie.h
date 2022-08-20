#pragma once

#include "../../GameInfo.h"

// C:\Users\dhsys\Desktop\DeathDoor_Clone\GameEngine\Include\Resource

class CPathStringTrie
{
	// ���ĺ�(26) + ����(10) + '\'(1) + ':' (1) + '_'(1)
	static const int PathStringTrieSize = 40;
public :
	CPathStringTrie();
	~CPathStringTrie();
private :
	// ������ ���� ǥ���ϱ� 
	bool m_Finish;
	CPathStringTrie* m_Next[PathStringTrieSize];
public :
	// �ݵ�� �ش� �Լ��� ����ϱ� ������ ��� �ҹ��� ���·� �ٲ㼭 �����ؾ� �Ѵ�.
	void insert(const char* key);
	bool find(const char* key);
};

