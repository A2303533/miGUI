#include "framework/mgf.h"
#include "framework/BackendGDI.h"
#include "framework/BackendOpenGL.h"
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

void window_onDropFiles(uint32_t num, uint32_t i, const wchar_t* fn, int x, int y)
{
	g_data.playlistMgr->AddTrackToEditPlaylist(fn);
}

void context_onDraw(mgf::Context* c, mgf::Backend* b)
{
	/*mgRect r;
	mgRectSet(&r, 0, 0, 100, 100);
	mgColor color;
	mgColorSetAsIntegerRGB(&color, 0xFF00FF);
	b->DrawRectangle(0, 0, &r, &color, &color, 0, 0);*/
}

// TABLE DEMO
//struct FileInfo // info for row
//{
//	//your data, no matter what you have this.
//	uint32_t number;
//	mgf::StringW filename;
//	int fileSize = 0;
//	mgf::StringW fileType;
//	int date = 0;
//	bool isSelected = false;
//};
//int testTable_onDrawRow(mgf::Table* tb, void* row, uint32_t col, wchar_t** text, uint32_t* textlen)
//{
//	if (row)
//	{
//		FileInfo* fi = (FileInfo*)row;
//		static mgf::StringW textBuffer;
//		// col is where you want to show text for current row
//		switch (col)
//		{
//		case 0:
//			textBuffer.clear();
//			textBuffer += fi->number;
//			*text = textBuffer.data();
//			*textlen = textBuffer.size();
//			break;
//		case 1:
//			*text = fi->filename.data();
//			*textlen = fi->filename.size();
//			break;
//		case 2:
//			textBuffer.clear();
//			textBuffer += fi->fileSize;
//			textBuffer += L" Bytes";
//			*text = textBuffer.data();
//			*textlen = textBuffer.size();
//			break;
//		case 3:
//			textBuffer.clear();
//			textBuffer += fi->fileType.data();
//			*text = textBuffer.data();
//			*textlen = textBuffer.size();
//			break;
//		case 4:
//			textBuffer.clear();
//			textBuffer += fi->date;
//			*text = textBuffer.data();
//			*textlen = textBuffer.size();
//			break;
//		}
//
//		return 1;
//	}
//	return 0;
//}
//int testTable_onIsRowSelected(mgf::Table* tb, void* row)
//{
//	FileInfo* fi = (FileInfo*)row;
//	return fi->isSelected == true;
//}
//FileInfo* g_testTable_selectedRow = 0;
//void testTable_onRowClick(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input)
//{
//	static uint32_t oldIndex = 0;
//	FileInfo* fi = (FileInfo*)row;
//
//	void** tableData = tb->GetData();
//
//	// MULTISELECTION
//	if (mouseButton)
//	{
//		if (input->keyboardModifier == MG_KBMOD_SHIFT)
//		{
//			fi->isSelected = true;
//			if (oldIndex != rowIndex)
//			{
//				uint32_t s1 = rowIndex;
//				uint32_t s2 = oldIndex;
//				if (s2 < s1)
//				{
//					s1 = oldIndex;
//					s2 = rowIndex;
//				}
//
//				for (uint32_t i = s1; i <= s2; ++i)
//				{
//					FileInfo* fi2 = (FileInfo*)tableData[i];
//					fi2->isSelected = true;
//
//					if (i == s2)
//						break;
//				}
//			}
//		}
//		else if (input->keyboardModifier & MG_KBMOD_ALT)
//		{
//			fi->isSelected = false;
//			if (oldIndex != rowIndex)
//			{
//				uint32_t s1 = rowIndex;
//				uint32_t s2 = oldIndex;
//				if (s2 < s1)
//				{
//					s1 = oldIndex;
//					s2 = rowIndex;
//				}
//
//				for (uint32_t i = s1; i <= s2; ++i)
//				{
//					FileInfo* fi2 = (FileInfo*)tableData[i];
//					fi2->isSelected = false;
//
//					if (i == s2)
//						break;
//				}
//			}
//		}
//		else
//		{
//			fi->isSelected = fi->isSelected ? false : true;
//		}
//	}
//
//	// select only 1
//	/*if (mouseButton)
//	{
//		if (g_testTable_selectedRow)
//		{
//			if (g_testTable_selectedRow == fi)
//			{
//				g_testTable_selectedRow = 0;
//				fi->isSelected = false;
//			}
//			else
//			{
//				g_testTable_selectedRow->isSelected = 0;
//
//				fi->isSelected = 1;
//				g_testTable_selectedRow = fi;
//			}
//		}
//		else
//		{
//			if (g_testTable_selectedRow)
//				g_testTable_selectedRow->isSelected = 0;
//
//			fi->isSelected = 1;
//			g_testTable_selectedRow = fi;
//		}
//	}*/
//	oldIndex = rowIndex;
//}
//int testTable_onCellClick(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton, mgInputContext_s* input)
//{
//	int r = 0;
//	FileInfo* fi = (FileInfo*)row;
//	//void** tableData = tb->GetData();
//	static uint32_t oldRowIndex = 0;
//	static uint32_t oldCellIndex = 0;
//	//printf("ROW: %u CELL: %u\n", rowIndex, cellIndex);
//
//	if (input->LMBClickCount == 2)
//	{
//		if ((oldRowIndex == rowIndex) && (oldCellIndex == colIndex))
//		{
//			if(colIndex == 1) // edit only filename
//				r = 1;
//
//			fi->isSelected = true;
//		}
//	}
//
//	oldRowIndex = rowIndex;
//	oldCellIndex = colIndex;
//
//	return r;
//}
//const wchar_t* testTable_onCellTextInputActivate(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t colIndex)
//{
//	FileInfo* fi = (FileInfo*)row;
//	//void** tableData = tb->GetData();
//	if (colIndex == 1)
//		return fi->filename.data();
//	return 0;
//}
//wchar_t testTable_onCellTextInputCharEnter(mgf::Table* tb, wchar_t c)
//{
//	if (c == L'\\')
//		return L'/';
//	return c;
//}
//int testTable_onCellTextInputEndEdit(mgf::Table*, int type, const wchar_t* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex)
//{
//	FileInfo* fi = (FileInfo*)row;
//	if (textinputText 
//		&& type == 1 // Enter
//		)
//	{
//		size_t l = wcslen(textinputText);
//		if (l)
//			fi->filename = textinputText;
//	}
//	return 1;
//}
//const wchar_t* testTable_onColTitleText(mgf::Table* tb, uint32_t* textLen, uint32_t colIndex)
//{
//	switch (colIndex)
//	{
//	case 0:
//		*textLen = 1;
//		return L"#";
//	case 1:
//		*textLen = 4;
//		return L"Name";
//	case 2:
//		*textLen = 4;
//		return L"File size";
//	case 3:
//		*textLen = 4;
//		return L"Type";
//	case 4:
//		*textLen = 4;
//		return L"Year";
//	}
//	return 0;
//}
//void testTable_onColTitleClick(mgf::Table* tb, uint32_t colIndex, uint32_t mouseButton)
//{
//	if (mouseButton == 1)
//	{
//		tb->SetActiveColTitle(colIndex);
//	}
//}
//int testTable_onGetUserElementNum(mgf::Table* tb, void* row, uint32_t rowIndex, uint32_t colIndex)
//{
//	if (colIndex == 0)
//	{
//		return 1;
//	}
//	return 0;
//}
//void testTable_onBeginGetUserElement(mgf::Table* tb)
//{
//	g_data.testTable_testButton_currBtn = 0;
//}
//mgf::Element* testTable_onGetUserElement(mgf::Table* tb, uint32_t index, void* row, uint32_t rowIndex, uint32_t colIndex)
//{
//	mgf::Element* e = g_data.tableTracklist_testButton[g_data.testTable_testButton_currBtn];
//	g_data.testTable_testButton_currBtn++;
//	if (g_data.testTable_testButton_currBtn == testTable_buttonsSize)
//		g_data.testTable_testButton_currBtn = 0;
//	e->SetID(rowIndex);
//	return e;
//}
//void testTable_button_onRelease(mgf::Element* e)
//{
//	int id = e->GetID();
//	void** tableData = g_data.tableTracklist->GetData();
//	FileInfo* fi2 = (FileInfo*)tableData[id];
//	wprintf(L"%s\n", fi2->filename.data());
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
	mgf::Ptr<mgf::SystemWindow> mainWindow = 0;
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

		mainWindow = framework.m_data->CreateWindow(
			MGWS_OVERLAPPEDWINDOW,
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

		context.m_data->GetSystemWindow()->SetOnClose(window_OnClose);
		context.m_data->GetSystemWindow()->SetOnSize(window_OnSize);
		context.m_data->GetSystemWindow()->SetOnDropFiles(window_onDropFiles);		
		context.m_data->GetSystemWindow()->Show();
		context.m_data->SetOnDraw(context_onDraw);
	
		guiWindow = context.m_data->CreateWindow();
		guiWindow.m_data->SetTitle(L"Window", 0);
		guiWindow.m_data->WithCloseButton(false);
		guiWindow.m_data->WithCollapseButton(false);
		guiWindow.m_data->WithTitlebar(false);
		guiWindow.m_data->DrawBG(false);
		guiWindow.m_data->CanMove(false);
		guiWindow.m_data->CanResize(false);
		guiWindow.m_data->CanToTop(false);
		guiWindow.m_data->SetSize(
			context.m_data->GetSystemWindow()->GetSize().x, 
			context.m_data->GetSystemWindow()->GetSize().y);
		

		g_data.mainWindow = guiWindow.m_data;

		g_data.playlistArea = guiWindow.m_data->AddRectangle();
		g_data.playlistArea->SetColor(0xFF77ADFF);

		g_data.controlArea = guiWindow.m_data->AddRectangle();
		g_data.controlArea->SetColor(0xFFBAC5C6);
		
		g_data.tracklistArea = guiWindow.m_data->AddRectangle();
		g_data.tracklistArea->SetColor(0xFFE1EEEF);

	//	auto text = guiWindow.m_data->AddText(0,0, L"Hello world", fontImpact.m_data);
	//	text->SetColor(0xFF337722);
		
		g_data.buttonNewPlaylist = guiWindow.m_data->AddButton();
		//g_data.buttonNewPlaylist->SetRect(0, 5, 180, 23);
		g_data.buttonNewPlaylist->SetPositionAndSize(0, 5, 180, 18); //SetPositionAndSize is more comfortable
		g_data.buttonNewPlaylist->SetDrawBG(false);
		g_data.buttonNewPlaylist->onReleaseLMB = Playlist_BTN_newPL_onRelease;

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
		g_data.listboxPlaylist = guiWindow.m_data->AddListBox(listboxFont.m_data);
		g_data.listboxPlaylist->SetRect(50, 200, 200, 300);
		g_data.listboxPlaylist->SetItemHeight(listboxFont.m_data->GetMaxSize().y);
		g_data.listboxPlaylist->SetDrawBG(false);
		g_data.listboxPlaylist->CanEdit(true);
		g_data.listboxPlaylist->SetDrawItemBG(true);
		g_data.listboxPlaylist->onTextInputCharEnter = Playlist_LB_onTextInputCharEnter;
		g_data.listboxPlaylist->onTextInputEndEdit = Playlist_LB_onTextInputEndEdit;
		g_data.listboxPlaylist->SetTextInputCharLimit(100);
		//g_data.listboxPlaylist->SetData(listboxData, 10);
		
		// TABLE DEMO
		/*FileInfo** arrFileInfo = 0;
		std::list<FileInfo*> lst;
		{
			uint32_t fn = 1;
			for (auto& entry : std::filesystem::recursive_directory_iterator("../"))
			{
				auto path = entry.path();
				if (std::filesystem::is_regular_file(path))
				{
					FileInfo* fi = new FileInfo;
					fi->number = fn++;
					fi->filename = path.c_str();
					fi->fileSize = std::filesystem::file_size(path);
					fi->fileType = "File";
					fi->date = 2022;
					lst.push_back(fi);
				}
			}
			if (lst.size())
			{
				arrFileInfo = (FileInfo**)std::malloc(lst.size() * sizeof(FileInfo*));
				int ic = 0;
				for (auto ln : lst)
				{
					arrFileInfo[ic] = ln;
					ic++;
				}
			}
		}
		g_data.tableTracklist = guiWindow.m_data->AddTable(5, listboxFont.m_data);
		g_data.tableTracklist->SetScrollSpeed(50.f);
		g_data.tableTracklist->SetRowHeight(20);
		g_data.tableTracklist->SetData((void**)arrFileInfo, lst.size());
		g_data.tableTracklist->onDrawRow = testTable_onDrawRow;
		g_data.tableTracklist->onIsRowSelected = testTable_onIsRowSelected;
		g_data.tableTracklist->onRowClick = testTable_onRowClick;
		g_data.tableTracklist->onCellClick = testTable_onCellClick;
		g_data.tableTracklist->onCellTextInputActivate = testTable_onCellTextInputActivate;
		g_data.tableTracklist->onCellTextInputCharEnter = testTable_onCellTextInputCharEnter;
		g_data.tableTracklist->onCellTextInputEndEdit = testTable_onCellTextInputEndEdit;
		g_data.tableTracklist->onColTitleText = testTable_onColTitleText;
		g_data.tableTracklist->onColTitleClick = testTable_onColTitleClick;
		g_data.tableTracklist->onGetUserElementNum = testTable_onGetUserElementNum;
		g_data.tableTracklist->onGetUserElement = testTable_onGetUserElement;
		g_data.tableTracklist->onBeginGetUserElement = testTable_onBeginGetUserElement;
		for (int i = 0; i < testTable_buttonsSize; ++i)
		{
			g_data.tableTracklist_testButton[i] = guiWindow.m_data->AddButton();
			g_data.tableTracklist_testButton[i]->SetVisible(false);
			g_data.tableTracklist_testButton[i]->SetRect(20, 2, 90, 15);
			g_data.tableTracklist_testButton[i]->SetText(L"TEST");
			g_data.tableTracklist_testButton[i]->SetFont(listboxFont.m_data);
			g_data.tableTracklist_testButton[i]->onReleaseLMB = testTable_button_onRelease;
		}
		printf("LIST SIZE: %u\n", lst.size());*/
		/*for (auto ln : lst){delete ln;}*/
		g_data.tableTracklist = guiWindow.m_data->AddTable(5, listboxFont.m_data);// initialization in PlayListManager
		

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

