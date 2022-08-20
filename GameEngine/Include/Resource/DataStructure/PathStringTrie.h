#pragma once

#include "../../GameInfo.h"

// C:\Users\dhsys\Desktop\DeathDoor_Clone\GameEngine\Include\Resource

class CPathStringTrie
{
	// 알파벳(26) + 숫자(10) + '\'(1) + ':' (1) + '_'(1)
	static const int PathStringTrieSize = 40;
public :
	CPathStringTrie();
	~CPathStringTrie();
private :
	// 끝나는 지점 표시하기 
	bool m_Finish;
	CPathStringTrie* m_Next[PathStringTrieSize];
public :
	// 반드시 해당 함수를 사용하기 이전에 모두 소문자 형태로 바꿔서 전달해야 한다.
	void insert(const char* key);
	bool find(const char* key);
};

