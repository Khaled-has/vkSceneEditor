#ifndef WINDOWSWINDOW_H
#define WINDOWSWINDOW_H

#include "../window.h"
#include <vulkan/vulkan.h>
#include <ImGui/ImGuiImage.h>

#include "windowsCustumTitleBar.h"

class windowsWindow : public window
{
public:
	windowsWindow(WindowProps mProps) : window(mProps) {}

	virtual void Setup() override;
	virtual void Destroy() override;

	virtual void Update(bool mMaxmize, bool mMinmize) override;

	virtual void CreateVkSurface(void* pInstance, void* pSurface) override;

private:
	struct SDL_Window* m_pWindow;
	VkSurfaceKHR m_Surface;

	WIN32_CustumTitleBar m_CustumTitleBar;
	ImGuiImage m_TBarImage;

	void UpdateDockingSpace();
};

#endif