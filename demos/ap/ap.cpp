#include "framework/mgf.h"
#include "framework/DND.h"
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
#include "framework/CURL.h"
#include "framework/Cryptography.h"

#include "framework/AudioEngine_waveOut.h"

#include "ap.h"
#include "playlist.h"

#include <list>
#include <iostream>
#include <filesystem>

AP_application* g_app = 0;

#define AP_PLAYLISTAREASIZE 180
#define AP_CONTROLAREASIZE 70

class AP_DNDCallback : public mgf::DNDCallback
{
	bool m_dragState = false;
public:
	AP_DNDCallback() {}
	virtual ~AP_DNDCallback() {}
	virtual bool OnDragEnter(int x, int y, Nature* nature)
	{
		m_dragState = true;
		switch (nature->m_type)
		{
		default:
			break;
		case NatureType::NatureType_file:
		{
			for (int i = 0; i < nature->_f.fileListSize; ++i)
			{
				std::filesystem::path pth = nature->_f.fileList[i];
				std::filesystem::path ext = pth.extension();
				if (ext.native() != L".wav" && ext.native() != L".mp3")
				{
					m_dragState = false;
					return false;
				}
		//		printf("FILE: %s\n", nature->_f.fileList[i]);
			}
		}break;
		}

		return true;
	}
	virtual bool OnDragOver(int x, int y) 
	{
		if (m_dragState)
		{
			return true;
		}

		return false;
	};
	
	virtual void OnDrop(Nature* nature)
	{
		switch (nature->m_type)
		{
		default:
			break;
		case NatureType::NatureType_file:
		{
			for (int i = 0; i < nature->_f.fileListSize; ++i)
			{
				std::filesystem::path pth = nature->_f.fileList[i];
				std::filesystem::path ext = pth.extension();
				if (ext.native() == L".wav" || ext.native() == L".mp3")
				{
					g_app->m_playlistMgr->AddTrackToEditPlaylist(pth.c_str());
				}
			}
		}break;
		}
	}
};

class ButtonNewPlaylist : public mgf::Button
{
public:
	ButtonNewPlaylist(mgf::Window* w):mgf::Button(w){}
	virtual ~ButtonNewPlaylist() {}
	virtual void OnReleaseLMB() override 
	{
		if (g_app->m_playlistMgr)
			g_app->m_playlistMgr->AddNew();
	}
};

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
		GetOSData()->hWnd,
		"Close app",
		"Are you sure?",
		MB_ICONQUESTION | MB_YESNO)
		== IDYES ? 1 : 0;
}

void WindowMain::OnSize()
{
	AP_application* app = static_cast<AP_application*>(GetUserData());

	if (!app->m_guiWindow)
		return;
	mgf::SystemWindow::OnSize();
	auto & sz = GetSize();
	app->m_guiWindow->SetSize(sz.x, sz.y);
	
	if(app->m_playlistArea)
		app->m_playlistArea->SetRect(0, 0, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if(app->m_controlArea)
		app->m_controlArea->SetRect(0, sz.y - AP_CONTROLAREASIZE, sz.x, sz.y);
	if(app->m_tracklistArea)
		app->m_tracklistArea->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);

	if (app->m_listboxPlaylist)
		app->m_listboxPlaylist->SetRect(5, 25, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if (app->m_tableTracklist)
		app->m_tableTracklist->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);
}

//void window_onDropFiles(uint32_t num, uint32_t i, const wchar_t* fn, int x, int y)
//{
//	g_data.playlistMgr->AddTrackToEditPlaylist(fn);
//}

AP_application::~AP_application()
{
	if (m_dndCallback)
		delete m_dndCallback;
	if (m_dnd)
		delete m_dnd;
}

bool AP_application::Init(backend_type bt)
{
	g_app = this;
	m_framework = mgf::InitFramework();
	m_sysWindow = new WindowMain(MGWS_OVERLAPPEDWINDOW,
		mgPoint(MGCW_USEDEFAULT, 0),
		mgPoint(MGCW_USEDEFAULT, 0));
	m_sysWindow->SetUserData(this);

	{
		mgf::CryptographyMD a;
		a.GenerateMD5("Test", 4);
		a.Print(stdout, "MD5:[", "]\n");
		a.GenerateMD4("Test", 4);
		a.Print(stdout, "MD4:[", "]\n");
		a.GenerateMD2("Test", 4);
		a.Print(stdout, "MD2:[", "]\n");
		a.GenerateMDC2("Test", 4);
		a.Print(stdout, "MDC2:[", "]\n");
	}
	
	m_style = m_framework->GetNewStyle(1);
	m_style.listItemBG1.setAsIntegerARGB(0xFFBFDDFF);
	m_style.listItemBG2.setAsIntegerARGB(0xFFBFDDFF);
	m_style.listItemHoverBG.setAsIntegerARGB(0xFFA8D2FF);

	m_audio = new mgf::AudioEngine_waveOut;

	m_backend = 0;
	switch (bt)
	{
	case backend_type::Gdi:
		m_backend = new mgf::BackendGDI;
		break;
	case backend_type::OpenGLOld:
	{
		mgf::BackendOpenGLParams oglParams;
		m_backend = new mgf::BackendOpenGL(oglParams);
	}break;
	default:
		break;
	}
	
	m_context = m_framework->CreateContext(m_sysWindow, m_backend);
	m_fontImpact = m_context->GetBackend()->CreateFont(L"Impact", 20, true, false);
	m_popupFont = m_context->GetBackend()->CreateFont(L"Arial", 9, false, false);
	m_listboxFont = m_context->GetBackend()->CreateFont(L"Arial", 9, false, false);
	m_context->SetDefaultPopupFont(m_popupFont);

	m_sysWindow->SetVisible(true);

	m_dndCallback = new AP_DNDCallback;
	m_dnd = new mgf::DNDWin32(m_dndCallback, m_sysWindow->GetOSData()->hWnd);
	
	m_guiWindow = new mgf::Window(m_context);
	m_guiWindow->SetTitle(L"Window", 0);
	m_guiWindow->SetWithCloseButton(false);
	m_guiWindow->SetWithCollapseButton(false);
	m_guiWindow->SetWithTitlebar(false);
	m_guiWindow->SetDrawBG(false);
	m_guiWindow->SetCanMove(false);
	m_guiWindow->SetCanResize(false);
	m_guiWindow->SetCanToTop(false);
	m_guiWindow->SetSize(
		m_context->GetSystemWindow()->GetSize().x,
		m_context->GetSystemWindow()->GetSize().y);
	
	m_playlistArea = new mgf::Rectangle(m_guiWindow);
	m_playlistArea->SetColor(0xFF77ADFF);

	m_controlArea = new mgf::Rectangle(m_guiWindow);
	m_controlArea->SetColor(0xFFBAC5C6);
		
	m_tracklistArea = new mgf::Rectangle(m_guiWindow);
	m_tracklistArea->SetColor(0xFFE1EEEF);

	m_buttonNewPlaylist = new ButtonNewPlaylist(m_guiWindow);
	m_buttonNewPlaylist->SetPositionAndSize(0, 5, 180, 18); //SetPositionAndSize is more comfortable
	m_buttonNewPlaylist->SetDrawBG(false);
	//m_buttonNewPlaylist->onReleaseLMB = Playlist_BTN_newPL_onRelease;
	
	m_icons = m_framework->CreateIcons("../data/ap/icons.png", m_context->GetBackend());
	if (m_icons)
	{
		int iconID1 = m_icons->Add(0, 0, 180, 17);
		int iconID2 = m_icons->Add(0, 19, 180, 17);
		m_buttonNewPlaylist->SetIcons(m_icons, iconID1, iconID2, iconID1, iconID1);
	}
	
	//auto window2 = context.m_data->CreateWindow();
	/*auto textInput = guiWindow.m_data->AddTextInput(fontImpact.m_data);
	textInput->SetRect(50, 200, 500, 250);
	textInput->SetText(L"Hello world");*/
	
	m_listboxPlaylist = new ListboxPlaylist(m_guiWindow);// guiWindow.m_data->AddListBox(listboxFont.m_data);
	m_listboxPlaylist->SetRect(50, 200, 200, 300);
	m_listboxPlaylist->SetItemHeight(m_listboxFont->GetMaxSize().y);
	m_listboxPlaylist->SetDrawBG(false);
	m_listboxPlaylist->CanEdit(true);
	m_listboxPlaylist->SetDrawItemBG(true);
	m_listboxPlaylist->SetTextInputCharLimit(100);
		//g_data.listboxPlaylist->SetData(listboxData, 10);
		
	m_tableTracklist = new TableTracklist(m_guiWindow, 5);// initialization in PlayListManager
	m_playlistMgr = new PlayListManager();
	
	if(m_listboxPlaylist)
		m_listboxPlaylist->SetUserStyle(&m_style);
	
	// also rebuild all gui
	m_context->GetSystemWindow()->OnSize();

	mgf::CURL c;

	return true;
}

void AP_application::Run()
{
	bool sleep = true;

	mgf::AudioSource* sound = m_audio->Load("D:/1_16bit_mono.wav");

	while (m_framework->Run())
	{
		if (sleep)
			Sleep(1);
	
		if (mgIsKeyHit(m_context->m_input, MG_KEY_Q))
		{
			if (sound)
				m_audio->Play(sound);
		}

		/*if (context.m_data->m_input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
		{
			testPopup->Show(context.m_data->m_input->mousePosition.x, context.m_data->m_input->mousePosition.y);
		}*/

		m_framework->DrawAll();
	}
}
