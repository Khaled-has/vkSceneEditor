#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "../EditorLabel.h"

#include <Entity.h>
#include <Components.h>

class Hierarchy : public EditorLabel
{
public:
	Hierarchy(EditorContext* pEditorContext) 
		: m_pEditorContext(pEditorContext) 
	{}

	virtual void OnUpdate() override;

private:
	EditorContext* m_pEditorContext;

	void DrawEntityHierarchy(Entity& mEntity);
};

#endif