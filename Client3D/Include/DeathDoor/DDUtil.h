#pragma once

#include "GameInfo.h"
#include "DDFlag.h"
#include "Flag.h"

class CDDUtil
{
public:
	// TODO : Death Door Condition Node Type �߰��ø��� �߰�
	static std::string DDConditionNodeTypeToString(DDConditionNode NodeType);
	static DDConditionNode StringToDDConditionNodeType(const std::string& Str);

	// TODO : Death Door Action Node Type �߰��ø��� �߰�
	static std::string DDActionNodeTypeToString(DDActionNode NodeType);
	static DDActionNode StringToDDActionNodeType(const std::string& Str);

	// TODO : Death Door SceneComponent �߰��ø��� �߰�
	static std::string DDSceneComponentTypeToString(DDSceneComponentType Type);
	static DDSceneComponentType StringToDDSceneComponentType(const std::string& Str);
	static size_t DDSceneComponentTypeToTypeID(DDSceneComponentType Type);

	// TODO : Death Door ObjectComponent �߰��ø��� �߰�
	static std::string DDObjectComponentTypeToString(DDObjectComponentType Type);
	static DDObjectComponentType StringToDDObjectComponentType(const std::string& Str);
	static size_t DDObjectComponentTypeToTypeID(DDObjectComponentType Type);

	// TODO : Death Door SceneMode �߰��ø��� ������Ʈ
	static std::string DDSceneModeTypeToString(DDSceneModeType Type);
	static DDSceneModeType StringToDDSceneModeType(const std::string& Str);
	static size_t DDSceneModeTypeToTypeID(DDSceneModeType Type);

	// �� �÷��� �ð��� ���� �������ִ� �Լ�
	static Vector4 LerpColor(const Vector4& ColorStart, const Vector4& ColorEnd, float ElapsedTime, float MaxTime);
};

