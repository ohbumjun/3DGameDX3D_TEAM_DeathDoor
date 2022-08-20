#pragma once

#include "GameInfo.h"

#define OBJECT_HIERARCHY					"ObjectHierarchy"
#define SCENECOMPONENT_HIERARCHY			"SceneComponentHierarchy"
#define OBJECTCOMPONENT_LIST				"ObjectComponentList"
#define FILE_BROWSER						"FileBrowser"
#define FILE_BROWSERTREE					"FileBrowserTree"
#define LOG_OUTPUT							"LogOutput"
#define INSPECTOR							"Inspector"
#define OBJECTCREATE_POPUPMODAL				"ObjectCreatePopUpModal"
#define SCENECOMPONENT_CREATE_POPUPMODAL	"SceneComponentCreatePopUpModal"
#define OBJECTCOMPONENT_CREATE_POPUPMODAL	"ObjectComponentCreatePopUpModal"
#define ANIMATION_EDITOR					"AnimationEditor"
#define ANIMATION_DISPLAYWINDW		"AnimationDisplayWindow"
#define PARTICLE_EDITOR						"ParticleEditor"
#define PARTICLE_DISPLAYWINDOW						"ParticleDisplayWindow"
#define FBX_CONVERTOR						"FBXConverter"
#define TOOL								"Tool"
#define BEHAVIORTREE_WINDOW					"BehaviorTreeWindow"
#define	COLLISION_PROFILE					"CollisionProfileWindow"
#define SOCKET_WINDOW						"SocketWindow"
#define SCENEMODE_WINDOW					"SceneModeWindow"

#define DIRECTORY_IMAGE						"DirectoryImage"
#define FILE_IMAGE							"FileImage"

#define SINGLELINE_NUMICON	5

#define ICON_SIZEX			100.f
#define ICON_SIZEY			130.f

enum class CreateObject_Type
{
	GameObject,
	Player
};

enum class TileEdit_Mode
{
	Type,
	Frame
};

struct FBXConvertWorkData
{
	std::string FBXFullPath;
};

struct AnimInstanceWorkData
{
	std::string AnimSequenceFileName;
	std::string AnimSequenceFullPath;
};

struct LoadingMessage
{
	float Percent;
	bool Complete;
	std::string Message;
};

static const float ParticleEditorWidth = 600.f;
static const float ParticleEditorHeight = 600.f;
