#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include "EditorLabel.h"

class Editor
{
public:
	Editor(AppContext* pAppContext, EditorContext* pEditorContext);

	void Update();

private:
	std::vector<EditorLabel*> m_EditorLabels;

	AppContext* m_pAppContext;

	void SetupStyle();
};

#endif