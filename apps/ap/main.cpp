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
#include "framework/Table.h"

#include "ap.h"
#include "playlist.h"

#include <list>

#define AP_PLAYLISTAREASIZE 180
#define AP_CONTROLAREASIZE 70

AP_global_data g_data;

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

	if (g_data.listboxPlaylist)
		g_data.listboxPlaylist->SetRect(5, 25, AP_PLAYLISTAREASIZE, sz.y - AP_CONTROLAREASIZE);
	if (g_data.tableTracklist)
		g_data.tableTracklist->SetRect(AP_PLAYLISTAREASIZE, 0, sz.x, sz.y - AP_CONTROLAREASIZE);
}

void context_onDraw(mgf::Context* c, mgf::Backend* b)
{
	/*mgRect r;
	mgRectSet(&r, 0, 0, 100, 100);
	mgColor color;
	mgColorSetAsIntegerRGB(&color, 0xFF00FF);
	b->DrawRectangle(0, 0, &r, &color, &color, 0, 0);*/
}

struct FileInfo // info for row
{
	mgf::StringW filename;
	int fileSize = 0;
	mgf::StringW fileType;
	int date = 0;
};
int test_onDrawRow(mgf::Table* tb, void* row, uint32_t col, wchar_t** text, uint32_t* textlen)
{
	if (row)
	{
		FileInfo* fi = (FileInfo*)row;
		static mgf::StringW textBuffer;

		switch (col)
		{
		case 0:
			*text = fi->filename.data();
			*textlen = fi->filename.size();
			break;
		case 1:
			textBuffer.clear();
			textBuffer += fi->fileSize;
			textBuffer += L" Bytes";
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		case 2:
			textBuffer.clear();
			textBuffer += fi->fileType.data();
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		case 3:
			textBuffer.clear();
			textBuffer += fi->date;
			*text = textBuffer.data();
			*textlen = textBuffer.size();
			break;
		}

		return 1;
	}
	return 0;
}

int main()
{
	mgf::Ptr<PlayListManager> playlistMgr = 0;

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

		g_data.style = framework.m_data->GetNewStyle(1);
		g_data.style.listItemBG1.setAsIntegerARGB(0xFFBFDDFF);
		g_data.style.listItemBG2.setAsIntegerARGB(0xFFBFDDFF);
		g_data.style.listItemHoverBG.setAsIntegerARGB(0xFFA8D2FF);

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
		g_data.buttonNewPlaylist->SetDrawBG(false);
		g_data.buttonNewPlaylist->onReleaseLMB = Playlist_BTN_newPL_onRelease;

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
		g_data.listboxPlaylist = window.m_data->AddListBox(0, 0, sizeof(PlaylistListBoxData), listboxFont.m_data);
		g_data.listboxPlaylist->SetRect(50, 200, 200, 300);
		g_data.listboxPlaylist->SetItemHeight(listboxFont.m_data->GetMaxSize().y);
		g_data.listboxPlaylist->SetDrawBG(false);
		g_data.listboxPlaylist->CanEdit(true);
		g_data.listboxPlaylist->SetSelectWithRMB(true);
		g_data.listboxPlaylist->SetDrawItemBG(true);
		g_data.listboxPlaylist->onTextInputCharEnter = Playlist_LB_onTextInputCharEnter;
		g_data.listboxPlaylist->onTextInputEndEdit = Playlist_LB_onTextInputEndEdit;
		g_data.listboxPlaylist->SetTextInputCharLimit(100);
		g_data.listboxPlaylist->NoDeselect(true);
		//g_data.listboxPlaylist->SetData(listboxData, 10);
		
		
		FileInfo** arrFileInfo = (FileInfo**)std::malloc(3 * sizeof(FileInfo*));
		std::list<FileInfo*> lst;
		{
			FileInfo* fi = new FileInfo;
			fi->filename = "E:\\Code\\miGUI\\bin32\\d3d11_demo.txt";
			fi->fileSize = 117;
			fi->fileType = "Text file";
			fi->date = 2022;
			lst.push_back(fi);

			fi = new FileInfo;
			fi->filename = "E:/app.exe";
			fi->fileSize = 3131;
			fi->fileType = "Application";
			fi->date = 2019;
			lst.push_back(fi);

			fi = new FileInfo;
			fi->filename = "E:/ass.png";
			fi->fileSize = 512422;
			fi->fileType = "PNG Image";
			fi->date = 2021;
			lst.push_back(fi);

			int ic = 0;
			for (auto ln : lst)
			{
				arrFileInfo[ic] = ln;
				ic++;
			}
		}
		/*for (int i = 0; i < 3; ++i)
		{
			FileInfo* fi = arrFileInfo[i];
			wprintf(L"FILE: %s\n", fi->filename.data());
		}*/
		g_data.tableTracklist = window.m_data->AddTable(4, listboxFont.m_data);
		g_data.tableTracklist->SetData((void**)arrFileInfo, 3);
		g_data.tableTracklist->onDrawRow = test_onDrawRow;

		playlistMgr = new PlayListManager(g_data.listboxPlaylist);
		g_data.playlistMgr = playlistMgr.m_data;

		g_data.listboxPlaylist->SetUserStyle(&g_data.style);

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