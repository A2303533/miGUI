#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/BackendOpenGL.h"
#include "framework/SystemWindow.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"
#include "framework/Font.h"
#include "framework/Text.h"
#include "framework/Button.h"
#include "framework/Icons.h"
#include "framework/TextInput.h"
#include "framework/ListBox.h"
#include "framework/Table.h"
#include "framework/TextBuffer.h"
#include "framework/Popup.h"
#include "framework/AudioEngine.h"

#include "ap.h"
#include "playlist.h"

#include <list>
#include <filesystem>

#define AP_PLAYLISTAREASIZE 180
#define AP_CONTROLAREASIZE 70

AP_global_data g_data;

class WindowMain : public mgf::SystemWindow
{
public:
	WindowMain(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
	virtual ~WindowMain() {}
	virtual bool OnClose() override;
	virtual void OnSize() override;
};

WindowMain::WindowMain(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize)
	:
	mgf::SystemWindow(windowFlags, windowPosition, windowSize)
{
	GetSizeMinimum()->x = 700;
	GetSizeMinimum()->y = 500;
}

bool WindowMain::OnClose()
{
	return MessageBoxA(
		(HWND)GetOSData()->handle,
		"Close app",
		"Are you sure?",
		MB_ICONQUESTION | MB_YESNO)
		== IDYES ? 1 : 0;
}

void WindowMain::OnSize()
{
	if (!g_data.mainWindow)
		return;
	auto & sz = GetSize();
	g_data.mainWindow->SetSize(sz.x, sz.y);
	
	if(g_data.playlistArea)
		g_data.playlistArea->SetRect(0, 0, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if(g_data.controlArea)
		g_data.controlArea->SetRect(0, sz.y - AP_CONTROLAREASIZE, sz.x, sz.y);
	if(g_data.tracklistArea)
		g_data.tracklistArea->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);

	if (g_data.listboxPlaylist)
		g_data.listboxPlaylist->SetRect(5, 25, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if (g_data.tableTracklist)
		g_data.tableTracklist->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);
}

//void window_onDropFiles(uint32_t num, uint32_t i, const wchar_t* fn, int x, int y)
//{
//	g_data.playlistMgr->AddTrackToEditPlaylist(fn);
//}

enum class backend_type
{
	Gdi,
	OpenGLOld,
};

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "en_US.utf8");
	printf("locale: %s\n", setlocale(LC_ALL, NULL));

	//{
	//	mgf::TextBuffer textBuf;
	//	std::setlocale(LC_ALL, "en_US.utf8");
	//	textBuf.FromFile(L"E:\\новая папка\\月曜日\\New Текстовый документ OpenDocument.odt");
	//	mgf::StringW wstr;
	//	wprintf(L"%s\n", L"BEGIN");
	//	while (!textBuf.IsEnd())
	//	{
	//		textBuf.GetLine(&wstr);
	//		wprintf(L"%s\n", wstr.data());

	//		//textBuf.GetToken(&wstr, 0);
	//		//if(wstr.size())
	//		//	wprintf(L"\'%s\' ", wstr.data());
	//	}
	//	wprintf(L"%s\n", L"END");
	//}

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

	mgf::Ptr<PlayListManager> playlistMgr = 0;

	mgf::Ptr<mgf::Framework> framework = 0;
	mgf::Ptr<WindowMain> mainWindow = 0;
	mgf::Ptr<mgf::Context> context = 0;
	mgf::Ptr<mgf::Window> guiWindow = 0;
	mgf::Ptr<mgf::Font> fontImpact = 0;
	mgf::Ptr<mgf::Font> popupFont = 0;
	mgf::Ptr<mgf::Font> listboxFont = 0;
	mgf::Ptr<mgf::Icons> icons = 0;
	try
	{
		framework = mgf::InitFramework();
		g_data.framework = framework.m_data;

		mainWindow = new WindowMain(MGWS_OVERLAPPEDWINDOW,
			mgPoint(MGCW_USEDEFAULT, 0),
			mgPoint(MGCW_USEDEFAULT, 0));

		g_data.style = framework.m_data->GetNewStyle(1);
		g_data.style.listItemBG1.setAsIntegerARGB(0xFFBFDDFF);
		g_data.style.listItemBG2.setAsIntegerARGB(0xFFBFDDFF);
		g_data.style.listItemHoverBG.setAsIntegerARGB(0xFFA8D2FF);

		mgf::AudioEngine* ae = new mgf::AudioEngine;

		mgf::Backend* be = 0;
		switch (backend)
		{
		case backend_type::Gdi:
			be = new mgf::BackendGDI;
			break;
		case backend_type::OpenGLOld:
		{
			mgf::BackendOpenGLParams oglParams;
			be = new mgf::BackendOpenGL(oglParams);
		}break;
		default:
			break;
		}

		context = framework.m_data->CreateContext(mainWindow.m_data, be);
		g_data.context = context.m_data;

		fontImpact = context.m_data->GetBackend()->CreateFont(L"Impact", 20, true, false);
		popupFont = context.m_data->GetBackend()->CreateFont(L"Arial", 9, false, false);
		listboxFont = context.m_data->GetBackend()->CreateFont(L"Arial", 9, false, false);
		context.m_data->SetDefaultPopupFont(popupFont.m_data);

		g_data.popupFont = popupFont.m_data;

		mainWindow.m_data->SetVisible(true);
	
		guiWindow = context.m_data->CreateWindow();
		guiWindow.m_data->SetTitle(L"Window", 0);
		guiWindow.m_data->SetWithCloseButton(false);
		guiWindow.m_data->SetWithCollapseButton(false);
		guiWindow.m_data->SetWithTitlebar(false);
		guiWindow.m_data->SetDrawBG(false);
		guiWindow.m_data->SetCanMove(false);
		guiWindow.m_data->SetCanResize(false);
		guiWindow.m_data->SetCanToTop(false);
		guiWindow.m_data->SetSize(
			context.m_data->GetSystemWindow()->GetSize().x, 
			context.m_data->GetSystemWindow()->GetSize().y);
		

		g_data.mainWindow = guiWindow.m_data;

		/*g_data.playlistArea = guiWindow.m_data->AddRectangle();
		g_data.playlistArea->SetColor(0xFF77ADFF);

		g_data.controlArea = guiWindow.m_data->AddRectangle();
		g_data.controlArea->SetColor(0xFFBAC5C6);
		
		g_data.tracklistArea = guiWindow.m_data->AddRectangle();
		g_data.tracklistArea->SetColor(0xFFE1EEEF);*/

		//g_data.buttonNewPlaylist = guiWindow.m_data->AddButton();
		//g_data.buttonNewPlaylist->SetPositionAndSize(0, 5, 180, 18); //SetPositionAndSize is more comfortable
		//g_data.buttonNewPlaylist->SetDrawBG(false);
		//g_data.buttonNewPlaylist->onReleaseLMB = Playlist_BTN_newPL_onRelease;

		icons = framework.m_data->CreateIcons("../data/ap/icons.png", context.m_data->GetBackend());
		if (icons.m_data)
		{
			int iconID1 = icons.m_data->Add(0, 0, 180, 17);
			int iconID2 = icons.m_data->Add(0, 19, 180, 17);
			g_data.buttonNewPlaylist->SetIcons(icons.m_data, iconID1, iconID2, iconID1, iconID1);
		}

		//auto window2 = context.m_data->CreateWindow();
		/*auto textInput = guiWindow.m_data->AddTextInput(fontImpact.m_data);
		textInput->SetRect(50, 200, 500, 250);
		textInput->SetText(L"Hello world");*/

		
		/*wchar_t strings[10][30];
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
		};*/
	///*	g_data.listboxPlaylist = guiWindow.m_data->AddListBox(listboxFont.m_data);
	//	g_data.listboxPlaylist->SetRect(50, 200, 200, 300);
	//	g_data.listboxPlaylist->SetItemHeight(listboxFont.m_data->GetMaxSize().y);
	//	g_data.listboxPlaylist->SetDrawBG(false);
	//	g_data.listboxPlaylist->CanEdit(true);
	//	g_data.listboxPlaylist->SetDrawItemBG(true);
	//	g_data.listboxPlaylist->onTextInputCharEnter = Playlist_LB_onTextInputCharEnter;
	//	g_data.listboxPlaylist->onTextInputEndEdit = Playlist_LB_onTextInputEndEdit;
	//	g_data.listboxPlaylist->SetTextInputCharLimit(100);*/
		//g_data.listboxPlaylist->SetData(listboxData, 10);
		
	//	g_data.tableTracklist = guiWindow.m_data->AddTable(5, listboxFont.m_data);// initialization in PlayListManager
		

		playlistMgr = new PlayListManager();
		g_data.playlistMgr = playlistMgr.m_data;

		g_data.listboxPlaylist->SetUserStyle(&g_data.style);

		/*mgPopupItemInfo_s popupItems[] =
		{
			{0, L"Make first", 0, 0, mgPopupItemType_default, 0, L"Ctrl+A", 1},
			{0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
			{0, L"Unpin", 0, 0, mgPopupItemType_default, 0, L"remove", 1},
			{0, L"Close", 0, 0, mgPopupItemType_default, 0, L"hide", 1},
		};
		mgf::Popup* testPopup = context.m_data->CreatePopup(listboxFont.m_data, popupItems, 4);*/

		// also rebuild all gui
		context.m_data->GetSystemWindow()->OnSize();

		bool sleep = true;

		while (framework.m_data->Run())
		{
			if (sleep)
				Sleep(1);

			/*if (context.m_data->m_input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
			{
				testPopup->Show(context.m_data->m_input->mousePosition.x, context.m_data->m_input->mousePosition.y);
			}*/

			framework.m_data->DrawAll();
		}

		
	}
	catch (const char* str)
	{
		MessageBoxA(0, str, "Error", 0);
	}

	return 0;
}

