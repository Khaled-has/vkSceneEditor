#include "windowsWindow.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <imgui_impl_sdl3.h>

VkInstance _Instance;

void windowsWindow::Setup()
{
	const auto& Props = GetWindowProps();

	// # Create SDL3 window
	m_pWindow = SDL_CreateWindow(
		Props.mName.c_str(), 
		Props.mWidth, Props.mHeight, 
		SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS
	);

	m_CustumTitleBar.Setup(m_pWindow);

	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForVulkan(m_pWindow);
}

void windowsWindow::Destroy()
{
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

void windowsWindow::Update(bool mMaxmize, bool mMinmize)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		m_CustumTitleBar.Process(&event);
		ImGui_ImplSDL3_ProcessEvent(&event);

	}

	// # Window maxmize
	if (mMaxmize)
	{
		m_CustumTitleBar.MaximizeWindow();
	}
	// # Window minmize
	if (mMinmize)
	{
		m_CustumTitleBar.MinimizeWindow();
	}

	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// # ImGui docking space
	UpdateDockingSpace();
}

void windowsWindow::CreateVkSurface(void* pInstance, void* pSurface)
{
	_Instance = static_cast<VkInstance>(pInstance);

	// # Create the surface
	VkSurfaceKHR* Surface = static_cast<VkSurfaceKHR*>(pSurface);
	SDL_Vulkan_CreateSurface(m_pWindow, _Instance, NULL, Surface);
}

void windowsWindow::UpdateDockingSpace()
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		viewport->Flags |= ImGuiViewportFlags_NoDecoration;
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + 55.0f));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - 55.0f));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGui::PopStyleVar();
	// Submit the DockSpace
	// REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
	// MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS ABOVE.
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}
