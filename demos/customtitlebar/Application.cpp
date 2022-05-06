#include "Application.h"
#include "framework/GS.h"
#include "framework/GSD3D11.h"

#include "miGUI.h"

#include "framework/BackendD3D11.h"
#include "framework/BackendGDI.h"
#include "framework/Window.h"
#include "framework/Font.h"
#include "framework/Icons.h"

#include "WindowMain.h"
#include "WindowMainMenu.h"

class MyBackend : public mgf::BackendGDI
{
public:
	MyBackend() {};
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
		if (m_menuFont)
			m_menuFont->Release();
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

	_InitThemes();
	SetTheme(&m_themeDark);

	//m_backend = new mgf::BackendGDI();
	m_backend = new MyBackend();
	m_GUIContext = m_framework->CreateContext(m_windowMain, m_backend);

	m_menuFont = m_backend->CreateFontPrivate(L"..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf", 11, false, false, L"LT Internet");

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
		m_mainMenuWindow->UseMenu(true, m_menuFont);

		m_mainMenuWindow->BeginMenu(L"File");
		{
			m_mainMenuWindow->BeginSubMenu(L"New", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Project", 0, L"Ctrl+Shift+N");
				m_mainMenuWindow->AddMenuItem(L"Repository", 0);
				m_mainMenuWindow->AddMenuItem(L"File", 0, L"Ctrl+N");
				m_mainMenuWindow->AddMenuItem(L"Project from", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Open", WindowMainMenu::MenuItemID_Open, false);
			{
				m_mainMenuWindow->AddMenuItem(L"Project/Solution", 0, L"Ctrl+Shift+O");
				m_mainMenuWindow->AddMenuItem(L"Folder", 0, L"Ctrl+Shift+Alt+O");
				m_mainMenuWindow->AddMenuItem(L"Web site", 0, L"Shift+Alt+O");
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"File", 0, L"Ctrl+O");
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(L"Clone rep", 0);
			m_mainMenuWindow->AddMenuItem(L"Start window", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(L"Add", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"New Project", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Existing project", 0);
				m_mainMenuWindow->AddMenuItem(L"Existing web site", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Close", 0);
			m_mainMenuWindow->AddMenuItem(L"Close solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Start live session", 0);
			m_mainMenuWindow->AddMenuItem(L"Join live session", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Save", WindowMainMenu::MenuItemID_Save);
			m_mainMenuWindow->AddMenuItem(L"Save as", WindowMainMenu::MenuItemID_SaveAs);
			m_mainMenuWindow->AddMenuItem(L"Save all", WindowMainMenu::MenuItemID_SaveAll);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Page setup", 0);
			m_mainMenuWindow->AddMenuItem(L"Print", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Account settings", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(L"Recent files", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App1.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App2.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App3.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App4.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App5.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App6.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App7.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App8.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App9.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App10.cpp", 0);
				m_mainMenuWindow->AddMenuItem(L"1 E:\\Code\\mixer\\src\\kkrooo\\App11.cpp", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"recent projects", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"1 Kkrooo", 0);
				m_mainMenuWindow->AddMenuItem(L"2 WindowsProject", 0);
				m_mainMenuWindow->AddMenuItem(L"3 Wx", 0);
				m_mainMenuWindow->AddMenuItem(L"4 test", 0);
				m_mainMenuWindow->AddMenuItem(L"5 dnd", 0);
				m_mainMenuWindow->AddMenuItem(L"6 popup", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Exit", WindowMainMenu::MenuItemID_Exit);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Edit");
		{
			m_mainMenuWindow->BeginSubMenu(L"Go to", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Go To Line", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To All", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To File", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Recent", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Type", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Member", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Symbol", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Go To Next", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Prev", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Go To Last", 0, L"Ctrl+G");
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Find and replace", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Quik find", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Quik replace", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Find in files", 0, L"Ctrl+G");
				m_mainMenuWindow->AddMenuItem(L"Replace in files", 0, L"Ctrl+G");
				m_mainMenuWindow->EndSubMenu();
			}
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Undo", WindowMainMenu::MenuItemID_Undo, L"Ctrl+Z");
			m_mainMenuWindow->AddMenuItem(L"Redo", WindowMainMenu::MenuItemID_Redo, L"Ctrl+Y");
			m_mainMenuWindow->AddMenuItem(L"Undo last", 0);
			m_mainMenuWindow->AddMenuItem(L"Redo last", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Cut", 0);
			m_mainMenuWindow->AddMenuItem(L"Copy", 0);
			m_mainMenuWindow->AddMenuItem(L"Paste", 0);
			m_mainMenuWindow->AddMenuItem(L"Show clipboard", 0);
			m_mainMenuWindow->AddMenuItem(L"Duplicate", 0);
			m_mainMenuWindow->AddMenuItem(L"Delete", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Select all", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Insert file", 0);

			m_mainMenuWindow->BeginSubMenu(L"Advanced", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Format Document", 0);
				m_mainMenuWindow->AddMenuItem(L"Format Selection", 0);
				m_mainMenuWindow->AddMenuItem(L"Tabify", 0);
				m_mainMenuWindow->AddMenuItem(L"UnTabify", 0);
				m_mainMenuWindow->AddMenuItem(L"Make upper", 0);
				m_mainMenuWindow->AddMenuItem(L"Make lower", 0);
				m_mainMenuWindow->AddMenuItem(L"Move selected up", 0);
				m_mainMenuWindow->AddMenuItem(L"Move selected down", 0);
				m_mainMenuWindow->AddMenuItem(L"delete horizontal", 0);
				m_mainMenuWindow->AddMenuItem(L"expand selection to", 0);
				m_mainMenuWindow->AddMenuItem(L"expand selection", 0);
				m_mainMenuWindow->AddMenuItem(L"Contract selection", 0);
				m_mainMenuWindow->AddMenuItem(L"view white space", 0);
				m_mainMenuWindow->AddMenuItem(L"word wrap", 0);
				m_mainMenuWindow->AddMenuItem(L"incremental search", 0);
				m_mainMenuWindow->AddMenuItem(L"toggle line comment", 0);
				m_mainMenuWindow->AddMenuItem(L"toggle block comment", 0);
				m_mainMenuWindow->AddMenuItem(L"Sort lines", 0);
				m_mainMenuWindow->AddMenuItem(L"join lines", 0);
				m_mainMenuWindow->AddMenuItem(L"Comment selection", 0);
				m_mainMenuWindow->AddMenuItem(L"Uncomment selection", 0);
				m_mainMenuWindow->AddMenuItem(L"Increase line indent", 0);
				m_mainMenuWindow->AddMenuItem(L"decrease line indent", 0);

				m_mainMenuWindow->BeginSubMenu(L"set end of line", 0);
				{
					m_mainMenuWindow->AddMenuItem(L"CRLF", 0, 0);
					m_mainMenuWindow->AddMenuItem(L"LF", 0, 0);
					m_mainMenuWindow->AddMenuItem(L"CR", 0, 0);
					m_mainMenuWindow->EndSubMenu();
				}

				m_mainMenuWindow->BeginSubMenu(L"set indentation", 0);
				{
					m_mainMenuWindow->AddMenuItem(L"Spaces", WindowMainMenu::MenuItemID_Spaces, 0);
					m_mainMenuWindow->AddMenuItem(L"Tabs", WindowMainMenu::MenuItemID_Tabs, 0);
					m_mainMenuWindow->EndSubMenu();
				}

				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Bookmarks", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Toggle bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"Enable all", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"Enable bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"prev bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"next bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"clear bookmark", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"prev bookmark in", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"next bookmark in", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"prev bookmark in doc", 0, 0);
				m_mainMenuWindow->AddMenuItem(L"next bookmark in doc", 0, 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Add task list shortcut", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Outlining", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Collapse block: ;", 0);
				m_mainMenuWindow->AddMenuItem(L"Collapse all in: ;", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Toggle outlining exp", 0);
				m_mainMenuWindow->AddMenuItem(L"Toggle all outlining", 0);
				m_mainMenuWindow->AddMenuItem(L"Stop outlining", 0);
				m_mainMenuWindow->AddMenuItem(L"Stop hiding current", 0);
				m_mainMenuWindow->AddMenuItem(L"Collapse to definition", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Intellisense", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"List members", 0);
				m_mainMenuWindow->AddMenuItem(L"Parameter info", 0);
				m_mainMenuWindow->AddMenuItem(L"quick info", 0);
				m_mainMenuWindow->AddMenuItem(L"Complete word", 0);
				m_mainMenuWindow->AddMenuItem(L"Switch between", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Surround with", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Multiple carets", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Insert next", 0);
				m_mainMenuWindow->AddMenuItem(L"Insert carets", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->BeginSubMenu(L"Refactor", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Convert to", 0);
				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"View");
		{
			m_mainMenuWindow->AddMenuItem(L"Code", 0);
			m_mainMenuWindow->AddMenuItem(L"Open", 0);
			m_mainMenuWindow->AddMenuItem(L"Open with", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Solution explorer", 0);
			m_mainMenuWindow->AddMenuItem(L"Git changes", 0);
			m_mainMenuWindow->AddMenuItem(L"Git repository", 0);
			m_mainMenuWindow->AddMenuItem(L"Team expl", 0);
			m_mainMenuWindow->AddMenuItem(L"Server expl", 0);
			m_mainMenuWindow->AddMenuItem(L"Test expl", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Bookmark window", 0);
			m_mainMenuWindow->AddMenuItem(L"Call hier", 0);
			m_mainMenuWindow->AddMenuItem(L"Class view", 0);
			m_mainMenuWindow->AddMenuItem(L"Code def wind", 0);
			m_mainMenuWindow->AddMenuItem(L"Obj brow", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->AddMenuItem(L"Error list", 0);
			m_mainMenuWindow->AddMenuItem(L"Output", 0);
			m_mainMenuWindow->AddMenuItem(L"Task list", 0);
			m_mainMenuWindow->AddMenuItem(L"Toolbox", 0);
			m_mainMenuWindow->AddMenuItem(L"Notifications", 0);
			m_mainMenuWindow->AddMenuItem(L"Terminal", 0);

			m_mainMenuWindow->BeginSubMenu(L"Other windows", 0);
			{
				m_mainMenuWindow->AddMenuItem(L"Command Window", 0);
				m_mainMenuWindow->AddMenuItem(L"Web browser", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Load test", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Application insight", 0);
				m_mainMenuWindow->AddMenuItem(L"Live share", 0);
				m_mainMenuWindow->AddMenuItem(L"Web publish", 0);
				m_mainMenuWindow->AddMenuItem(L"task runner", 0);
				m_mainMenuWindow->AddMenuItem(L"Package manager", 0);
				m_mainMenuWindow->AddMenuItem(L"Browser link", 0);
				m_mainMenuWindow->AddMenuItem(L"Document outline", 0);
				m_mainMenuWindow->AddMenuItem(L"Property manager", 0);
				m_mainMenuWindow->AddMenuItem(L"Resource view", 0);
				m_mainMenuWindow->AddMenuItem(L"F# interactive", 0);
				m_mainMenuWindow->AddMenuItem(L"C# interactive", 0);
				m_mainMenuWindow->AddMenuItem(0, 0);
				m_mainMenuWindow->AddMenuItem(L"Code metrics", 0);

				m_mainMenuWindow->EndSubMenu();
			}
			m_mainMenuWindow->AddMenuItem(0, 0);

			m_mainMenuWindow->BeginSubMenu(L"Toolbars", 0);
			{

				m_mainMenuWindow->EndSubMenu();
			}

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Git", WindowMainMenu::MenuID_Git);
		{
			m_mainMenuWindow->AddMenuItem(L"Clone rep", 0);
			m_mainMenuWindow->AddMenuItem(L"local reps", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Commit", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Fetch", 0);
			m_mainMenuWindow->AddMenuItem(L"Pull", 0);
			m_mainMenuWindow->AddMenuItem(L"Push", 0);
			m_mainMenuWindow->AddMenuItem(L"Sync", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"New branch", 0);
			m_mainMenuWindow->AddMenuItem(L"view branch history", 0);
			m_mainMenuWindow->AddMenuItem(L"Manage branches", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Github", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"open in file explorer", 0);
			m_mainMenuWindow->AddMenuItem(L"open in cmd", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Manage remotes", 0);
			m_mainMenuWindow->AddMenuItem(L"Settings", 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Project");
		{
			m_mainMenuWindow->AddMenuItem(L"Retarget solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Add module", 0);
			m_mainMenuWindow->AddMenuItem(L"Add class", 0);
			m_mainMenuWindow->AddMenuItem(L"class wizard", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Add new item", 0);
			m_mainMenuWindow->AddMenuItem(L"Add existing item", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Exclude from project", 0);
			m_mainMenuWindow->AddMenuItem(L"Show all files", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Rescan solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Display", 0);

			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Build");
		{
			m_mainMenuWindow->AddMenuItem(L"Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Debug");
		{
			m_mainMenuWindow->AddMenuItem(L"Debug: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Debug: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Test");
		{
			m_mainMenuWindow->AddMenuItem(L"Test: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Test: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Analyze");
		{
			m_mainMenuWindow->AddMenuItem(L"Analyze: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Analyze: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Tools");
		{
			m_mainMenuWindow->AddMenuItem(L"Tools: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Tools: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Extensions");
		{
			m_mainMenuWindow->AddMenuItem(L"Extensions: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions:  solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Extensions: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Window");
		{
			m_mainMenuWindow->AddMenuItem(L"Window: Build solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: ReBuild solution", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: clean solution", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Build Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: ReBuild Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Clean Kkrooo", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Run code analysis", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Project only", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Batch build", 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Config manager", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Window: Compile", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->EndMenu();
		}

		m_mainMenuWindow->BeginMenu(L"Help");
		{
			m_mainMenuWindow->AddMenuItem(L"View help", 0);
			m_mainMenuWindow->AddMenuItem(L"Get started", 0);
			m_mainMenuWindow->AddMenuItem(L"Tips", 0);
			m_mainMenuWindow->AddMenuItem(0, 0);
			m_mainMenuWindow->AddMenuItem(L"Feedbacks", 0);

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
	m_titleText = "Kkrooo ";
	m_titleText += m_versionMaj;
	m_titleText += ".";
	m_titleText += m_versionMin;
	m_titleText += ".";
	m_titleText += m_versionPatch;
	m_windowMain->SetTitle(m_titleText.data());
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