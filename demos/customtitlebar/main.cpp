#include "Application.h"
#include "framework/mgf.h"

#include <Windows.h>

#ifdef MG_DEBUG
#pragma comment(lib, "mgfd.lib")
#else
#pragma comment(lib, "mgf.lib")
#endif

int main()
{
	Application* app = 0;
	app = new Application;
	if (app->Init())
	{
		app->Run();
		delete app;
	}
	return 0;
}

