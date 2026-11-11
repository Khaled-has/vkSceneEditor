#include "Application.h"

int main(int argc, char* argv[])
{
	Application* pApp = new Application();
	pApp->Run();
	delete pApp;

	return 0;
}