#pragma once

#include <stdint.h>
#include <string>

struct WindowProps
{
	std::string mName;
	uint32_t mWidth;
	uint32_t mHeight;
};

class window
{
public:
	window(WindowProps mProps) : m_Props(mProps) {}

	virtual void Setup() = 0;
	virtual void Destroy() = 0;

	virtual void Update(bool mMaxmize, bool mMinmize) = 0;

	virtual void CreateVkSurface(void* pInstance, void* pSurface) = 0;

	inline const WindowProps& GetWindowProps() const { return m_Props; }

private:
	WindowProps m_Props;
};

window* CreateWindowsWindow(WindowProps mProps);
