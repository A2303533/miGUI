#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/Window.h"
#include <stdio.h>
// for callbacks
struct global_data
{
	mgf::Window* mainWindow = 0;
}
g_data;

int window_OnClose(mgf::SystemWindow* w)
{
	return MessageBoxA(
		(HWND)w->GetOSData().handle, 
		"Close app", 
		"Are you sure?", 
		MB_ICONQUESTION | MB_YESNO) 
		== IDYES ? 1 : 0;
}

void window_OnSize(mgf::SystemWindow* w)
{
	if (!g_data.mainWindow)
		return;
	auto & sz = w->GetSize();
	g_data.mainWindow->SetSize(sz.x, sz.y);
}

int main()
{
	mgf::Ptr<mgf::Framework> framework = 0;
	mgf::Ptr<mgf::Context> context = 0;
	mgf::Ptr<mgf::Window> w1 = 0;
	try
	{
		framework = mgf::InitFramework();

		context = framework.m_data->CreateContext(
			MGWS_OVERLAPPEDWINDOW,
			mgPoint(MGCW_USEDEFAULT, 0),
			mgPoint(MGCW_USEDEFAULT, 0),
			new mgf::BackendGDI
		);

		context.m_data->GetSystemWindow()->SetOnClose(window_OnClose);
		context.m_data->GetSystemWindow()->SetOnSize(window_OnSize);
		context.m_data->GetSystemWindow()->Show();
		
		w1 = context.m_data->CreateWindow();
		w1.m_data->SetTitle(L"Window");
		w1.m_data->WithCloseButton(false);
		w1.m_data->WithCollapseButton(false);
		w1.m_data->WithTitlebar(false);
		w1.m_data->CanMove(false);
		w1.m_data->SetSize(
			context.m_data->GetSystemWindow()->GetSize().x, 
			context.m_data->GetSystemWindow()->GetSize().y);

		g_data.mainWindow = w1.m_data;

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