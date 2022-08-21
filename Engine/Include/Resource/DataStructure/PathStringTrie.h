#pragma once

#include "../../GameInfo.h"

// C:\Users\dhsys\Desktop\DeathDoor_Clone\GameEngine\Include\Resource

class CPathStringTrie
{
	// �ҹ��ھ��ĺ�(26) + �빮�ھ��ĺ�(26) + ����(10) + '\'(1) + ':' (1) + '_'(1) + '.'(1)
	static const int PathStringTrieSize = 66;
public :
	CPathStringTrie();
	~CPathStringTrie();
private :
	// ������ ���� ǥ���ϱ� 
	bool m_Finish;
	CPathStringTrie* m_Next[PathStringTrieSize];
public :
	// �ݵ�� �ش� �Լ��� ����ϱ� ������ ��� �ҹ��� ���·� �ٲ㼭 �����ؾ� �Ѵ�.
	void insert(char* key);
	bool find(char* key);
};

