#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/BackendOpenGL.h"
#include "framework/Array.h"

#include "ap.h"

#include <vector>

int main(int argc, char* argv[])
{
	backend_type backend = backend_type::Gdi;
	if (argc)
	{
		for (int i = 0; i < argc; ++i)
		{
			if (strcmp(argv[i], "-b") == 0)
			{
				++i;
				if (i < argc)
				{
					if (strcmp(argv[i], "ogl_old") == 0)
						backend = backend_type::OpenGLOld;
				}
			}
		}
	}

	AP_application* app = new AP_application;

	if (app->Init(backend))
	{
		app->Run();
	}

	return 0;
}

