#include "framework/mgf.h"
#include "framework/BackendGDI.h"

int window_OnClose(mgf::SystemWindow* w)
{
	return MessageBoxA(
		(HWND)w->GetOSData().handle, 
		"Close app", 
		"Are you sure?", 
		MB_ICONQUESTION | MB_YESNO) 
		== IDYES ? 1 : 0;
}

int main()
{
	mgf::Ptr<mgf::Framework> framework = 0;
	mgf::Ptr<mgf::Context> context = 0;
	try
	{
		framework = mgf::InitFramework();
		context = framework.m_data->CreateContext(mgf::SystemWindow::Type::type_default,
			mgPoint(0,0), mgPoint(800, 600), new mgf::BackendGDI);

		context.m_data->GetWindow()->SetOnClose(window_OnClose);
		context.m_data->GetWindow()->Show();

		bool sleep = true;

		while (framework.m_data->Run())
		{
			if (sleep)
				Sleep(1);

			framework.m_data->DrawAll();
		}
	}
	catch (const char* str)
	{
		MessageBoxA(0, str, "Error", 0);
	}

	return 0;
}