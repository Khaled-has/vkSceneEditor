#ifndef APPLICATION_H
#define APPLICATION_H

#include <window.h>
#include <Renderer.h>
#include <Scene.h>

#include <Editor.h>

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	window* m_pWindow			= nullptr;
	gfx::Renderer* m_pRenderer	= nullptr;

	Editor* m_pEditor			= nullptr;
	AppContext m_AppContext;
	EditorContext m_EditorContext;

	Scene* m_pScene				= nullptr;
};

#endif