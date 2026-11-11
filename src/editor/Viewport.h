#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "EditorLabel.h"
#include <Renderer.h>

class Viewport : public EditorLabel
{
public:
	Viewport() {}

	virtual void OnUpdate() override;

private:
	gfx::RenderTarget* 
};

#endif