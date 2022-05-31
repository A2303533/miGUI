﻿#include "Application.h"
#include "framework/mgf.h"

#include <Windows.h>

MGF_LINK_LIBRARY("mgf")


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

