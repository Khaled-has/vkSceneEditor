#include "Application.h"

// Remove latter
#include <Entity.h> 
#include <Components.h>
// <|

Application::Application()
{
	m_AppContext = AppContext{
		.mProjectName = "Nothing for now",
		.mProjectPath = "Nothing for now",

		.mCloseApp = false,
		.mMaxmizeApp = false,
		.mMinmizeApp = false
	};

	// # Create window
	WindowProps WindowProps = {
		.mName = "vkSceneEditor",
		.mWidth = 1440,
		.mHeight = 720
	};
	m_pWindow = CreateWindowsWindow(WindowProps);
	m_pWindow->Setup();

	// # Create renderer
	m_pRenderer = gfx::CreateRenderer(
		[&](void* pInstance, void* pSurface) {
			m_pWindow->CreateVkSurface(pInstance, pSurface);
		}
	);
	m_pRenderer->Setup();

	// # Open scene
	m_pScene = new Scene;

	Entity EntityTest(m_pScene->CreateEntity(), m_pScene);

	EntityTest.AddComponent<TagComponent>(
		"Object-Test"
	);

	EntityTest.AddComponent<TransformComponent>(
		glm::vec3(0, 0, 0),
		glm::vec3(1, 1, 1),
		glm::vec3(0, 0, 0)
	);

	m_EditorContext.mSelectedEntity = -1;
	m_EditorContext.pScene = m_pScene;

	// # Create editor
	m_pEditor = new Editor(&m_AppContext, &m_EditorContext);
}

Application::~Application()
{
	m_pRenderer->Destroy();
	m_pWindow->Destroy();
}

void Application::Run()
{
	// # Application loop
	while (!m_AppContext.mCloseApp)
	{
		// # Update window
		m_pWindow->Update(m_AppContext.mMaxmizeApp, m_AppContext.mMinmizeApp);

		// # Update editor
		m_pEditor->Update();

		// # Update renderer
		m_pRenderer->Render();
	}
}
