#include "framework/mgf.h"
#include "framework/BackendGDI.h"

int main()
{
	mgf::Ptr<mgf::Framework> framework = 0;
	mgf::Ptr<mgf::Context> context = 0;
	try
	{
		framework = mgf::InitFramework();
		context = framework.m_data->CreateContext(mgf::SystemWindow::Type::type_default,
			mgPoint(0,0), mgPoint(800, 600), new mgf::BackendGDI);

		context.m_data->GetWindow()->Show();

		//while (framework.m_data->Run())
		{
			MSG msg;
			while (GetMessage(&msg, nullptr, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	catch (const char* str)
	{
		MessageBoxA(0, str, "Error", 0);
	}

	return 0;
}