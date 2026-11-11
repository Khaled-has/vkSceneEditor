#ifndef EDITORLABEL_H
#define EDITORLABEL_H

#include <iostream>
#include <string>

#include <Scene.h>

struct AppContext
{
	std::string mProjectName;
	std::string mProjectPath;

	bool mCloseApp;
	bool mMaxmizeApp;
	bool mMinmizeApp;
};

struct EditorContext
{
	Scene* pScene;
	int32_t mSelectedEntity;
};

class EditorLabel
{
public:
	~EditorLabel() = default;

	virtual void OnUpdate() = 0;

	bool IsOpen = true;
};

#endif