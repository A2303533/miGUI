#include "Application.h"
#include "framework/GS.h"
#include "framework/GSD3D11.h"

#include "miGUI.h"

#include "framework/BackendD3D11.h"
#include "framework/BackendGDI.h"
#include "framework/Window.h"
#include "framework/Font.h"
#include "framework/Icons.h"
#include "framework/Archive.h"
#include "framework/Unicode.h"

#include <filesystem>

#include "WindowMain.h"
#include "WindowMainMenu.h"

class MyBackend : public mgf::BackendGDI
{
public:
	MyBackend(mgf::SystemWindow* sw)
	:
		mgf::BackendGDI(sw)
	{}
	virtual ~MyBackend() {};

	virtual void DrawRectangle(
		int reason, 
		void* object, 
		mgRect* rct, 
		mgColor* color1, 
		mgColor* color2,
		mgTexture* texture, 
		mgVec4* UVRegion)
	{
		switch (reason)
		{
		default:
			mgf::BackendGDI::DrawRectangle(reason, object, rct, color1, color2, texture, UVRegion);
			break;
		case mgDrawRectangleReason_popupBG:
		{
			mgRect r1 = *rct;
			mgRect r2 = *rct;
			mgColor cb;
			cb.setAsIntegerRGB(0);
			mgf::BackendGDI::DrawRectangle(0, object, &r2, &cb, &cb, texture, UVRegion);

			r1.left += 1;
			r1.right -= 1;
			r1.bottom -= 1;
			r1.top += 1;
			mgf::BackendGDI::DrawRectangle(0, object, &r1, color1, color2, texture, UVRegion);
		}break;
		}
	}
};

Application::Application()
{
	
}

Application::~Application()
{
	if (m_guiIcons) m_guiIcons->Release();
	
	if (m_windowBG) m_windowBG->Release();

	if (m_mainMenuWindow) m_mainMenuWindow->Release();

	if (m_backend)
	{
		/*if (m_menuFont)
			m_menuFont->Release();*/
	}

	if (m_GUIContext) m_GUIContext->Release();
	if (m_windowMain) m_windowMain->Release();
	if (m_backend) m_backend->Release();
	if (m_framework) m_framework->Release();
	if (m_gs) delete m_gs;
}

bool Application::Init()
{
	m_colorWhite.setAsIntegerRGB(0xFFFFFF);
	m_colorRed.setAsIntegerRGB(0xFF0000);
	m_colorBlue.setAsIntegerRGB(0x0000FF);
	m_colorLime.setAsIntegerRGB(0x4CFF00);

	m_framework = mgf::InitFramework();
	m_windowMain = new WindowMain(this,
		MGWS_OVERLAPPEDWINDOW,
		mgPoint(MGCW_USEDEFAULT, 0),
		mgPoint(MGCW_USEDEFAULT, 0));

	m_windowMain->SetUseCustomTitleBar(true);
	m_windowMain->SetCustomTitleBarSize(22);

	mgf::StringA stra;
	mgf::StringA stra2;
	mgf::StringW strw;
	mgf::StringW strw2;
	mgf::UnicodeConverter uConv;

	stra.push_back('A');
	stra.push_back('B');
	stra.push_back('C');
	stra.push_back('[');
	stra.push_back(0xD0);
	stra.push_back(0x90);
	stra.push_back(0xD0);
	stra.push_back(0x91);
	stra.push_back(0xD0);
	stra.push_back(0x92);
	stra.push_back(0xE2);
	stra.push_back(0x82);
	stra.push_back(0xAC);
	stra.push_back(0xF0);
	stra.push_back(0x90);
	stra.push_back(0x8D);
	stra.push_back(0x88);
	stra.push_back(0xF0);// chinese \four bytes
	stra.push_back(0xA0);
	stra.push_back(0x81);
	stra.push_back(0xB3);
	stra.push_back(0xF0);// emoji
	stra.push_back(0x9F);
	stra.push_back(0x98);
	stra.push_back(0x9D);
	stra.push_back(']');

	uConv.CharToWchar(stra, strw);
	uConv.WcharToChar(strw, stra2);

	stra2.clear();
	mgf::UnicodeString ustr(stra);
	ustr.Append((char32_t)0x24B62);
	ustr.Get(stra2);
	ustr.Get(strw2);
	stra2.clear();


	/*const char* big_buffer = "Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA \
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGB1IGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data B5iBigBIGBIGBIG DATA\
Big Data BiBigBIGwBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BigBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIxGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA\
Big Data BiBigBIGBIGBIG DATABig Data BiBigBIGBIGBIG DATA";

	mgf::ArchiveSystem archive;
	mgf::CompressionInfo cmpInf;
	cmpInf.m_dataUncompressed = (uint8_t*)big_buffer;
	cmpInf.m_sizeUncompressed = strlen(big_buffer);
	if (archive.Compress(&cmpInf))
	{
		FILE * f = fopen("compressed.data", "wb");
		if (f)
		{
			fwrite(cmpInf.m_dataCompressed, cmpInf.m_sizeCompressed, 1, f);
			fclose(f);
			
			mgf::CompressionInfo dcmpInf;

			dcmpInf.m_sizeUncompressed = cmpInf.m_sizeUncompressed;
			dcmpInf.m_dataUncompressed = (uint8_t*)malloc(dcmpInf.m_sizeUncompressed);
			dcmpInf.m_dataCompressed = cmpInf.m_dataCompressed;
			dcmpInf.m_sizeCompressed = cmpInf.m_sizeCompressed;
			if (archive.Decompress(&dcmpInf))
			{
				FILE* f = fopen("decompressed.data", "wb");
				if (f)
				{
					fwrite(dcmpInf.m_dataUncompressed, cmpInf.m_sizeUncompressed, 1, f);
					fclose(f);
				}
			}

			free(dcmpInf.m_dataUncompressed);
		}

		free(cmpInf.m_dataCompressed);
	}*/

	_InitThemes();
	SetTheme(&m_themeDark);

	//m_backend = new mgf::BackendGDI();
	m_backend = new MyBackend(m_windowMain);
	m_GUIContext = m_framework->CreateContext(m_windowMain, m_backend);


	/*if(std::filesystem::exists("..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf"))
		m_menuFont = m_backend->CreateFontPrivate(U"..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf", 11, false, false, U"LT Internet");
	else
		m_menuFont = m_backend->CreateFont(U"Arial", 11, false, false);*/

	m_windowBG = new mgf::Window(m_GUIContext);
	m_windowBG->SetCanDock(false);
	m_windowBG->SetCanMove(false);
	m_windowBG->SetCanResize(false);
	m_windowBG->SetCanToTop(false);
	m_windowBG->SetWithCloseButton(false);
	m_windowBG->SetWithCollapseButton(false);
	m_windowBG->SetWithTitlebar(false);
	m_windowBG->SetDrawBG(true);
	m_windowBG->SetPosition(0, 0);
	m_windowBG->SetUserStyle(&m_windowBGStyle);
	
	auto wn = new mgf::Window(m_GUIContext);
	wn->SetPosition(100, 100);

	m_mainMenuWindow = new WindowMainMenu(this);// 
	m_mainMenuWindow->SetNoMenuBG(true);
	m_mainMenuWindow->SetWithTitlebar(false);
	m_mainMenuWindow->SetCanToTop(false);
	m_mainMenuWindow->SetCanDock(false);
	m_mainMenuWindow->SetCanMove(false);
	m_mainMenuWindow->SetCanResize(false);
	m_mainMenuWindow->SetDrawBG(false);
	m_mainMenuWindow->SetUserStyle(&m_mainMenuWindowStyle);
	m_mainMenuWindow->SetPosition(MainMenuPositionXWindowed, 0);

	//m_mainMenuWindow->Hide();
	{
		m_mainMenuWindow->UseMenu(true, true);

		m_mainMenuWindow->BeginMenu(U"File");
		{
			m_mainMenuWindow->BeginSubMenu(U"New", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Project", 0, U"Ctrl+Shift+N");
				m_mainMenuWindow->AddMenuItem(U"Repository", 0);
				m_mainMenuWindow->AddMenuItem(U"File", 0, U"Ctrl+N");
				m_mainMenuWindow->AddMenuItem(U"Project from", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Open", WindowMainMenu::MenuItemID_Open, false);
			{
				m_mainMenuWindow->AddMenuItem(U"Project/Solution", 0, U"Ctrl+Shift+O");
				m_mainMenuWindow->AddMenuItem(U"Folder", 0, U"Ctrl+Shift+Alt+O");
				m_mainMenuWindow->AddMenuItem(U"Web site", 0, U"Shift+Alt+O");
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"File", 0, U"Ctrl+O");
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(U"Clone rep", 0);
			m_mainMenuWindow->AddMenuItem(U"Start window", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(U"Add", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"New Project", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Existing project", 0);
				m_mainMenuWindow->AddMenuItem(U"Existing web site", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Close", 0);
			m_mainMenuWindow->AddMenuItem(U"Close solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Start live session", 0);
			m_mainMenuWindow->AddMenuItem(U"Join live session", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Save", WindowMainMenu::MenuItemID_Save);
			m_mainMenuWindow->AddMenuItem(U"Save as", WindowMainMenu::MenuItemID_SaveAs);
			m_mainMenuWindow->AddMenuItem(U"Save all", WindowMainMenu::MenuItemID_SaveAll);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Page setup", 0);
			m_mainMenuWindow->AddMenuItem(U"Print", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Account settings", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(U"Recent files", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App1.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App2.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App3.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App4.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App5.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App6.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App7.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App8.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App9.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App10.cpp", 0);
				m_mainMenuWindow->AddMenuItem(U"1 E:\\Code\\mixer\\src\\kkrooo\\App11.cpp", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"recent projects", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"1 Kkrooo", 0);
				m_mainMenuWindow->AddMenuItem(U"2 WindowsProject", 0);
				m_mainMenuWindow->AddMenuItem(U"3 Wx", 0);
				m_mainMenuWindow->AddMenuItem(U"4 test", 0);
				m_mainMenuWindow->AddMenuItem(U"5 dnd", 0);
				m_mainMenuWindow->AddMenuItem(U"6 popup", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Exit", WindowMainMenu::MenuItemID_Exit);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Edit");
		{
			m_mainMenuWindow->BeginSubMenu(U"Go to", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Go To Line", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To All", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To File", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Recent", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Type", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Member", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Symbol", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Go To Next", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Prev", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Go To Last", 0, U"Ctrl+G");
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Find and replace", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Quik find", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Quik replace", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Find in files", 0, U"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(U"Replace in files", 0, U"Ctrl+G");
				m_mainMenuWindow->EndSubMenu();
			}
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Undo", WindowMainMenu::MenuItemID_Undo, U"Ctrl+Z");
			m_mainMenuWindow->AddMenuItem(U"Redo", WindowMainMenu::MenuItemID_Redo, U"Ctrl+Y");
			m_mainMenuWindow->AddMenuItem(U"Undo last", 0);
			m_mainMenuWindow->AddMenuItem(U"Redo last", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Cut", 0);
			m_mainMenuWindow->AddMenuItem(U"Copy", 0);
			m_mainMenuWindow->AddMenuItem(U"Paste", 0);
			m_mainMenuWindow->AddMenuItem(U"Show clipboard", 0);
			m_mainMenuWindow->AddMenuItem(U"Duplicate", 0);
			m_mainMenuWindow->AddMenuItem(U"Delete", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Select all", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Insert file", 0);

			m_mainMenuWindow->BeginSubMenu(U"Advanced", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Format Document", 0);
				m_mainMenuWindow->AddMenuItem(U"Format Selection", 0);
				m_mainMenuWindow->AddMenuItem(U"Tabify", 0);
				m_mainMenuWindow->AddMenuItem(U"UnTabify", 0);
				m_mainMenuWindow->AddMenuItem(U"Make upper", 0);
				m_mainMenuWindow->AddMenuItem(U"Make lower", 0);
				m_mainMenuWindow->AddMenuItem(U"Move selected up", 0);
				m_mainMenuWindow->AddMenuItem(U"Move selected down", 0);
				m_mainMenuWindow->AddMenuItem(U"delete horizontal", 0);
				m_mainMenuWindow->AddMenuItem(U"expand selection to", 0);
				m_mainMenuWindow->AddMenuItem(U"expand selection", 0);
				m_mainMenuWindow->AddMenuItem(U"Contract selection", 0);
				m_mainMenuWindow->AddMenuItem(U"view white space", 0);
				m_mainMenuWindow->AddMenuItem(U"word wrap", 0);
				m_mainMenuWindow->AddMenuItem(U"incremental search", 0);
				m_mainMenuWindow->AddMenuItem(U"toggle line comment", 0);
				m_mainMenuWindow->AddMenuItem(U"toggle block comment", 0);
				m_mainMenuWindow->AddMenuItem(U"Sort lines", 0);
				m_mainMenuWindow->AddMenuItem(U"join lines", 0);
				m_mainMenuWindow->AddMenuItem(U"Comment selection", 0);
				m_mainMenuWindow->AddMenuItem(U"Uncomment selection", 0);
				m_mainMenuWindow->AddMenuItem(U"Increase line indent", 0);
				m_mainMenuWindow->AddMenuItem(U"decrease line indent", 0);

				m_mainMenuWindow->BeginSubMenu(U"set end of line", 0);
				{
					m_mainMenuWindow->AddMenuItem(U"CRLF", 0, 0);
					m_mainMenuWindow->AddMenuItem(U"LF", 0, 0);
					m_mainMenuWindow->AddMenuItem(U"CR", 0, 0);
					m_mainMenuWindow->EndSubMenu();
				}

				m_mainMenuWindow->BeginSubMenu(U"set indentation", 0);
				{
					m_mainMenuWindow->AddMenuItem(U"Spaces", WindowMainMenu::MenuItemID_Spaces, 0);
					m_mainMenuWindow->AddMenuItem(U"Tabs", WindowMainMenu::MenuItemID_Tabs, 0);
					m_mainMenuWindow->EndSubMenu();
				}

				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Bookmarks", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Toggle bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"Enable all", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"Enable bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"prev bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"next bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"clear bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"prev bookmark in", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"next bookmark in", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"prev bookmark in doc", 0, 0);
				m_mainMenuWindow->AddMenuItem(U"next bookmark in doc", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Add task list shortcut", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Outlining", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Collapse block: ;", 0);
				m_mainMenuWindow->AddMenuItem(U"Collapse all in: ;", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Toggle outlining exp", 0);
				m_mainMenuWindow->AddMenuItem(U"Toggle all outlining", 0);
				m_mainMenuWindow->AddMenuItem(U"Stop outlining", 0);
				m_mainMenuWindow->AddMenuItem(U"Stop hiding current", 0);
				m_mainMenuWindow->AddMenuItem(U"Collapse to definition", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Intellisense", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"List members", 0);
				m_mainMenuWindow->AddMenuItem(U"Parameter info", 0);
				m_mainMenuWindow->AddMenuItem(U"quick info", 0);
				m_mainMenuWindow->AddMenuItem(U"Complete word", 0);
				m_mainMenuWindow->AddMenuItem(U"Switch between", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Surround with", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Multiple carets", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Insert next", 0);
				m_mainMenuWindow->AddMenuItem(U"Insert carets", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(U"Refactor", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Convert to", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"View");
		{
			m_mainMenuWindow->AddMenuItem(U"Code", 0);
			m_mainMenuWindow->AddMenuItem(U"Open", 0);
			m_mainMenuWindow->AddMenuItem(U"Open with", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Solution explorer", 0);
			m_mainMenuWindow->AddMenuItem(U"Git changes", 0);
			m_mainMenuWindow->AddMenuItem(U"Git repository", 0);
			m_mainMenuWindow->AddMenuItem(U"Team expl", 0);
			m_mainMenuWindow->AddMenuItem(U"Server expl", 0);
			m_mainMenuWindow->AddMenuItem(U"Test expl", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Bookmark window", 0);
			m_mainMenuWindow->AddMenuItem(U"Call hier", 0);
			m_mainMenuWindow->AddMenuItem(U"Class view", 0);
			m_mainMenuWindow->AddMenuItem(U"Code def wind", 0);
			m_mainMenuWindow->AddMenuItem(U"Obj brow", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(U"Error list", 0);
			m_mainMenuWindow->AddMenuItem(U"Output", 0);
			m_mainMenuWindow->AddMenuItem(U"Task list", 0);
			m_mainMenuWindow->AddMenuItem(U"Toolbox", 0);
			m_mainMenuWindow->AddMenuItem(U"Notifications", 0);
			m_mainMenuWindow->AddMenuItem(U"Terminal", 0);

			m_mainMenuWindow->BeginSubMenu(U"Other windows", 0);
			{
				m_mainMenuWindow->AddMenuItem(U"Command Window", 0);
				m_mainMenuWindow->AddMenuItem(U"Web browser", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Load test", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Application insight", 0);
				m_mainMenuWindow->AddMenuItem(U"Live share", 0);
				m_mainMenuWindow->AddMenuItem(U"Web publish", 0);
				m_mainMenuWindow->AddMenuItem(U"task runner", 0);
				m_mainMenuWindow->AddMenuItem(U"Package manager", 0);
				m_mainMenuWindow->AddMenuItem(U"Browser link", 0);
				m_mainMenuWindow->AddMenuItem(U"Document outline", 0);
				m_mainMenuWindow->AddMenuItem(U"Property manager", 0);
				m_mainMenuWindow->AddMenuItem(U"Resource view", 0);
				m_mainMenuWindow->AddMenuItem(U"F# interactive", 0);
				m_mainMenuWindow->AddMenuItem(U"C# interactive", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(U"Code metrics", 0);

				m_mainMenuWindow->EndSubMenu();
			}
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(U"Toolbars", 0);
			{

				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Git", WindowMainMenu::MenuID_Git);
		{
			m_mainMenuWindow->AddMenuItem(U"Clone rep", 0);
			m_mainMenuWindow->AddMenuItem(U"local reps", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Commit", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Fetch", 0);
			m_mainMenuWindow->AddMenuItem(U"Pull", 0);
			m_mainMenuWindow->AddMenuItem(U"Push", 0);
			m_mainMenuWindow->AddMenuItem(U"Sync", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"New branch", 0);
			m_mainMenuWindow->AddMenuItem(U"view branch history", 0);
			m_mainMenuWindow->AddMenuItem(U"Manage branches", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Github", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"open in file explorer", 0);
			m_mainMenuWindow->AddMenuItem(U"open in cmd", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Manage remotes", 0);
			m_mainMenuWindow->AddMenuItem(U"Settings", 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Project");
		{
			m_mainMenuWindow->AddMenuItem(U"Retarget solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Add module", 0);
			m_mainMenuWindow->AddMenuItem(U"Add class", 0);
			m_mainMenuWindow->AddMenuItem(U"class wizard", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Add new item", 0);
			m_mainMenuWindow->AddMenuItem(U"Add existing item", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Exclude from project", 0);
			m_mainMenuWindow->AddMenuItem(U"Show all files", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Rescan solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Display", 0);

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Build");
		{
			m_mainMenuWindow->AddMenuItem(U"Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Debug");
		{
			m_mainMenuWindow->AddMenuItem(U"Debug: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Debug: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Test");
		{
			m_mainMenuWindow->AddMenuItem(U"Test: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Test: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Analyze");
		{
			m_mainMenuWindow->AddMenuItem(U"Analyze: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Analyze: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Tools");
		{
			m_mainMenuWindow->AddMenuItem(U"Tools: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Tools: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Extensions");
		{
			m_mainMenuWindow->AddMenuItem(U"Extensions: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions:  solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Extensions: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Window");
		{
			m_mainMenuWindow->AddMenuItem(U"Window: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Window: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(U"Help");
		{
			m_mainMenuWindow->AddMenuItem(U"View help", 0);
			m_mainMenuWindow->AddMenuItem(U"Get started", 0);
			m_mainMenuWindow->AddMenuItem(U"Tips", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(U"Feedbacks", 0);

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->RebuildMenu();

		m_mainMenuWindow->SetMenuTextOffset(0, 3);
	}

	mgf::GSD3D11* gsd3d11 = new mgf::GSD3D11();
	m_gs = gsd3d11;

	//mgf::BackendD3D11Params bp;
	if (!gsd3d11->Init(m_windowMain, 0))
	{
		return false;
	}

	m_archiveSystem = new mgf::ArchiveSystem;
	mgf::ArchiveZipFile* zip = m_archiveSystem->ZipAdd("../data/appicons.zip");
	if (zip)
	{
		uint32_t dataSize = 0;
		uint8_t* data = m_archiveSystem->ZipUnzip("app_icons.png", &dataSize, zip);
		if (data)
		{
			m_guiIcons = m_framework->CreateIcons(data, dataSize, mgf::ImageLoader::png, m_backend);
			free(data);
		}
	}

	if(!m_guiIcons)
		m_guiIcons = m_framework->CreateIcons(L"../data/app_icons.png", m_backend);

	m_guiIcons_ids.m_mainMenu_logo = m_guiIcons->Add(0, 0, 29, 15);
	m_guiIcons_ids.m_mainMenu_minimize = m_guiIcons->Add(33, 0, 17, 17);
	m_guiIcons_ids.m_mainMenu_maximize = m_guiIcons->Add(50, 0, 17, 17);
	m_guiIcons_ids.m_mainMenu_close = m_guiIcons->Add(67, 0, 28, 17);
	m_guiIcons_ids.m_mainMenu_FileSave = m_guiIcons->Add(95, 0, 14, 14);

	UpdateWindowTitle();
	m_windowMain->SetVisible(true);
	m_windowMain->Rebuild();
	return true;
}

void Application::Run()
{
	m_gs->SetClearColor(&m_themeCurr->m_clearColor.r);

	while (m_framework->Run())
	{
		//m_gs->BeginDraw();
		//m_gs->ClearAll();

		//m_backend->BeginDraw();
		m_framework->DrawAll();
		//m_backend->EndDraw();

		//m_gs->EndDraw();
		//m_gs->SwapBuffers();
	}
}

mgf::GS* Application::GetGS()
{
	return m_gs;
}

void Application::UpdateWindowTitle()
{
	m_titleText = U"Kkrooo ";
	m_titleText += m_versionMaj;
	m_titleText += ".";
	m_titleText += m_versionMin;
	m_titleText += ".";
	m_titleText += m_versionPatch;

	m_titleText.Get(m_titleTextW);
	
	m_windowMain->SetTitle(m_titleTextW.data());
}

void Application::_InitThemes()
{
	m_themeDark.m_titlebarText.setAsIntegerRGB(0xEFEFEF);
	m_themeLight.m_titlebarText.setAsIntegerRGB(0xEFEFEF);

	m_themeDark.m_titlebarSystemButtonCloseHover.setAsIntegerRGB(0xFF0000);
	m_themeLight.m_titlebarSystemButtonCloseHover.setAsIntegerRGB(0xFF0000);
	
	m_themeDark.m_titlebarSystemButtonClosePush.setAsIntegerRGB(0xAA0000);
	m_themeLight.m_titlebarSystemButtonClosePush.setAsIntegerRGB(0xAA0000);

	m_themeDark.m_titlebarSystemButtonMaxHover.setAsIntegerRGB(0x007ACC);
	m_themeLight.m_titlebarSystemButtonMaxHover.setAsIntegerRGB(0x007ACC);
	m_themeDark.m_titlebarSystemButtonMaxPush.setAsIntegerRGB(0x005B99);
	m_themeLight.m_titlebarSystemButtonMaxPush.setAsIntegerRGB(0x005B99);

	m_themeDark.m_titlebarSystemButtonMinHover.setAsIntegerRGB(0x007ACC);
	m_themeLight.m_titlebarSystemButtonMinHover.setAsIntegerRGB(0x007ACC);
	m_themeDark.m_titlebarSystemButtonMinPush.setAsIntegerRGB(0x005B99);
	m_themeLight.m_titlebarSystemButtonMinPush.setAsIntegerRGB(0x005B99);

	m_themeDark.m_titlebarBG.setAsIntegerRGB(0x454545);
	m_themeLight.m_titlebarBG.setAsIntegerRGB(0x454545);

	m_themeDark.m_titlebarMenuHoverBG.setAsIntegerRGB(0x5B5B5B);
	m_themeLight.m_titlebarMenuHoverBG.setAsIntegerRGB(0x5B5B5B);

	m_themeDark.m_popupBG.setAsIntegerRGB(0x2D2D2D);
	m_themeLight.m_popupBG.setAsIntegerRGB(0x2D2D2D);

	m_themeDark.m_clearColor.setAsIntegerRGB(0x454545);
	m_themeLight.m_clearColor.setAsIntegerRGB(0x454545);

	m_themeDark.m_titlebarMenuActiveBG.setAsIntegerRGB(0x3E6382);
	m_themeLight.m_titlebarMenuActiveBG.setAsIntegerRGB(0x3E6382);

	m_themeDark.m_popupText.setAsIntegerRGB(0xEFEFEF);
	m_themeLight.m_popupText.setAsIntegerRGB(0xEFEFEF);
	m_themeDark.m_popupShortcutText.setAsIntegerRGB(0xEFEFEF);
	m_themeLight.m_popupShortcutText.setAsIntegerRGB(0xEFEFEF);
	m_themeDark.m_popupDisabledText.setAsIntegerRGB(0x636363);
	m_themeLight.m_popupDisabledText.setAsIntegerRGB(0x636363);

}

void Application::SetTheme(Theme* t)
{
	m_themeCurr = t;
	m_mainMenuWindowStyle.windowMenuText = t->m_titlebarText;
	m_mainMenuWindowStyle.windowMenuBG = t->m_titlebarBG;
	m_mainMenuWindowStyle.windowMenuHoverItemBG = t->m_titlebarMenuHoverBG;
	m_mainMenuWindowStyle.windowMenuActiveItemBG = t->m_titlebarMenuActiveBG;
	m_mainMenuWindowStyle.popupBG = t->m_popupBG;
	m_mainMenuWindowStyle.popupText = t->m_popupText;
	m_mainMenuWindowStyle.popupTextShortcut = t->m_popupShortcutText;
	m_mainMenuWindowStyle.popupTextDisabled = t->m_popupDisabledText;
	m_mainMenuWindowStyle.popupHoverElementBG = t->m_titlebarMenuActiveBG;
	m_mainMenuWindowStyle.popupIconNext = t->m_popupShortcutText;
	m_mainMenuWindowStyle.popupIconCheck = t->m_popupText;
	m_windowBGStyle.windowBG = t->m_clearColor;
	
	if(m_gs)
		m_gs->SetClearColor(&m_themeCurr->m_clearColor.r);
}

void Application::Quit()
{
	m_framework->m_run = false;
}