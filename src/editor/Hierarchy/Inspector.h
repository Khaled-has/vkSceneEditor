#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "../EditorLabel.h"

class Inspector : public EditorLabel
{
public:
	Inspector(EditorContext* pEditorContext)
		: m_pEditorContext(pEditorContext)
	{}

	virtual void OnUpdate() override;

private:
	EditorContext* m_pEditorContext;
};

#endif