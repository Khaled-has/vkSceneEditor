#ifndef WINDOWSCUSTUMTITLEBAR_H
#define WINDOWSCUSTUMTITLEBAR_H

#include <SDL3/SDL.h>

class WIN32_CustumTitleBar
{
public:
	WIN32_CustumTitleBar() {}

	void Setup(SDL_Window* pWindow);

	void Process(SDL_Event* pEvent);

	void MinimizeWindow();
	void MaximizeWindow();

private:
	SDL_Window* m_pWindow;
};

#endif