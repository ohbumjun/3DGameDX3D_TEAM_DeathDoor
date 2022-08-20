#pragma once

#include "GameInfo.h"
#include "DDFlag.h"
#include "Flag.h"

class CDDUtil
{
public:
	// TODO : Death Door Condition Node Type 추가시마다 추가
	static std::string DDConditionNodeTypeToString(DDConditionNode NodeType);
	static DDConditionNode StringToDDConditionNodeType(const std::string& Str);

	// TODO : Death Door Action Node Type 추가시마다 추가
	static std::string DDActionNodeTypeToString(DDActionNode NodeType);
	static DDActionNode StringToDDActionNodeType(const std::string& Str);

	// TODO : Death Door SceneComponent 추가시마다 추가
	static std::string DDSceneComponentTypeToString(DDSceneComponentType Type);
	static DDSceneComponentType StringToDDSceneComponentType(const std::string& Str);
	static size_t DDSceneComponentTypeToTypeID(DDSceneComponentType Type);

	// TODO : Death Door ObjectComponent 추가시마다 추가
	static std::string DDObjectComponentTypeToString(DDObjectComponentType Type);
	static DDObjectComponentType StringToDDObjectComponentType(const std::string& Str);
	static size_t DDObjectComponentTypeToTypeID(DDObjectComponentType Type);

	// TODO : Death Door SceneMode 추가시마다 업데이트
	static std::string DDSceneModeTypeToString(DDSceneModeType Type);
	static DDSceneModeType StringToDDSceneModeType(const std::string& Str);
	static size_t DDSceneModeTypeToTypeID(DDSceneModeType Type);

	// 두 컬러를 시간에 따라 보간해주는 함수
	static Vector4 LerpColor(const Vector4& ColorStart, const Vector4& ColorEnd, float ElapsedTime, float MaxTime);
};

