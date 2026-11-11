#include "window.h"
#include "windows/windowsWindow.h"

window* CreateWindowsWindow(WindowProps mProps)
{
	return new windowsWindow(mProps);
}