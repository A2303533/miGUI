#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"
#include "framework/Font.h"
#include "framework/Text.h"
#include "framework/Button.h"
#include "framework/Icons.h"
#include "framework/TextInput.h"
#include "framework/ListBox.h"
#include <stdio.h>

#define AP_PLAYLISTAREASIZE 180
#define AP_CONTROLAREASIZE 70

// for callbacks
struct global_data
{
	mgf::Window* mainWindow = 0;
	mgf::Rectangle* playlistArea = 0;
	mgf::Rectangle* controlArea = 0;
	mgf::Rectangle* tracklistArea = 0;
	mgf::Button* buttonNewPlaylist = 0;
}
g_data;

int window_OnClose(mgf::SystemWindow* w)
{
	/*return MessageBoxA(
		(HWND)w->GetOSData().handle, 
		"Close app", 
		"Are you sure?", 
		MB_ICONQUESTION | MB_YESNO) 
		== IDYES ? 1 : 0;*/
	return 1;
}

void window_OnSize(mgf::SystemWindow* w)
{
	if (!g_data.mainWindow)
		return;
	auto & sz = w->GetSize();
	g_data.mainWindow->SetSize(sz.x, sz.y);
	
	if(g_data.playlistArea)
		g_data.playlistArea->SetRect(0, 0, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if(g_data.controlArea)
		g_data.controlArea->SetRect(0, sz.y - AP_CONTROLAREASIZE, sz.x, sz.y);
	if(g_data.tracklistArea)
		g_data.tracklistArea->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);
}

void context_onDraw(mgf::Context* c, mgf::Backend* b)
{
	/*mgRect r;
	mgRectSet(&r, 0, 0, 100, 100);
	mgColor color;
	mgColorSetAsIntegerRGB(&color, 0xFF00FF);
	b->DrawRectangle(0, 0, &r, &color, &color, 0, 0);*/
}


struct lbData
{
	const wchar_t* text = 0;
	uint32_t flags = 0; /*0x1 - selected or not*/
	std::string someData;
};

wchar_t testLB_onTextInputCharEnter(mgf::ListBox* lb, wchar_t c)
{
	bool good = false;

	if (iswalnum(c))
		good = true;

	if (!good)
	{
		if (c == L'_')
			good = true;
	}

	return good ? c : 0;
}

int onTextInputEndEdit(mgf::ListBox* lb, int i, const wchar_t* str, uint8_t* editItem)
{
	/*
	* i:
	*	1 - Enter
	*   2 - click somewhere
	*   3 - Escape
	*/

	if (str)
	{
		/*wprintf(L"END: %s\n", str);*/
		if (i == 1)
		{
			uint32_t len = wcslen(str);
			if (len < 30)
			{
				lbData* data = (lbData*)editItem;
				/* data->text points to not const wchar_t 
				*   so we can change text.
				*/
				wsprintf((wchar_t*)data->text, L"%s", str);
			}
		}
	}

	return i;
}

int main()
{
	mgf::Ptr<mgf::Framework> framework = 0;
	mgf::Ptr<mgf::Context> context = 0;
	mgf::Ptr<mgf::Window> window = 0;
	mgf::Ptr<mgf::Font> fontImpact = 0;
	mgf::Ptr<mgf::Font> popupFont = 0;
	mgf::Ptr<mgf::Font> listboxFont = 0;
	mgf::Ptr<mgf::Icons> icons = 0;
	try
	{
		framework = mgf::InitFramework();

		context = framework.m_data->CreateContext(
			MGWS_OVERLAPPEDWINDOW,
			mgPoint(MGCW_USEDEFAULT, 0),
			mgPoint(MGCW_USEDEFAULT, 0),
			new mgf::BackendGDI
		);

		fontImpact = context.m_data->GetBackend()->CreateFont(L"Impact", 20, true, false);
		popupFont = context.m_data->GetBackend()->CreateFont(L"Arial", 9, false, false);
		listboxFont = context.m_data->GetBackend()->CreateFont(L"Arial", 9, false, false);
		context.m_data->SetDefaultPopupFont(popupFont.m_data);

		context.m_data->GetSystemWindow()->SetOnClose(window_OnClose);
		context.m_data->GetSystemWindow()->SetOnSize(window_OnSize);
		context.m_data->GetSystemWindow()->Show();
		context.m_data->SetOnDraw(context_onDraw);
		

		window = context.m_data->CreateWindow();
		window.m_data->SetTitle(L"Window");
		window.m_data->WithCloseButton(false);
		window.m_data->WithCollapseButton(false);
		window.m_data->WithTitlebar(false);
		window.m_data->DrawBG(false);
		window.m_data->CanMove(false);
		window.m_data->CanResize(false);
		window.m_data->CanToTop(false);
		window.m_data->SetSize(
			context.m_data->GetSystemWindow()->GetSize().x, 
			context.m_data->GetSystemWindow()->GetSize().y);
		

		g_data.mainWindow = window.m_data;

		g_data.playlistArea = window.m_data->AddRectangle();
		g_data.playlistArea->SetColor(0xFF77ADFF);

		g_data.controlArea = window.m_data->AddRectangle();
		g_data.controlArea->SetColor(0xFFBAC5C6);
		
		g_data.tracklistArea = window.m_data->AddRectangle();
		g_data.tracklistArea->SetColor(0xFFE1EEEF);

	//	auto text = window.m_data->AddText(0,0, L"Hello world", fontImpact.m_data);
	//	text->SetColor(0xFF337722);
		
		g_data.buttonNewPlaylist = window.m_data->AddButton();
		g_data.buttonNewPlaylist->SetRect(0, 5, 180, 20);
		//g_data.buttonNewPlaylist->SetText(L"GO!!!");
		g_data.buttonNewPlaylist->SetDrawBG(false);
		//g_data.buttonNewPlaylist->SetEnabled(false);

		icons = framework.m_data->CreateIcons("../data/ap/icons.png", context.m_data->GetBackend());
		if (icons.m_data)
		{
			int iconID1 = icons.m_data->Add(0, 0, 180, 20);
			int iconID2 = icons.m_data->Add(0, 19, 180, 20);
			g_data.buttonNewPlaylist->SetIcons(icons.m_data, iconID1, iconID2, iconID1, iconID1);
		}

		//auto window2 = context.m_data->CreateWindow();
		/*auto textInput = window.m_data->AddTextInput(fontImpact.m_data);
		textInput->SetRect(50, 200, 500, 250);
		textInput->SetText(L"Hello world");*/

		
		wchar_t strings[10][30];
		for (int i = 0; i < 10; ++i)
		{
			wsprintf(strings[i], L"Item%i", i);
		}
		lbData listboxData[] =
		{
			{strings[0], 0, "string"},
			{strings[1], mgf::ListBoxFlag_select, "string"},
			{strings[2], mgf::ListBoxFlag_select, "string"},
			{strings[3], 0, "some text"},
			{strings[4], 0, "string"},
			{strings[5], 0, "data"},
			{strings[6], 0, "blabla"},
			{strings[7], 0, "string"},
			{strings[8], 0, "string"},
			{strings[9], 0, "string"},
		};
		auto list = window.m_data->AddListBox(listboxData, 10, sizeof(lbData), listboxFont.m_data);
		list->SetRect(50, 200, 200, 300);
		list->SetItemHeight(listboxFont.m_data->GetMaxSize().y);
		list->SetMultiselect(true);
		list->SetDrawBG(true);
		list->CanEdit(true);
		list->onTextInputCharEnter = testLB_onTextInputCharEnter;
		list->onTextInputEndEdit = onTextInputEndEdit;

		// also rebuild all gui
		context.m_data->GetSystemWindow()->OnSize();

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