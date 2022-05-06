#include "framework/mgf.h"
#include "framework/SystemWindow.h"
#include "framework/Window.h"
#include "framework/Context.h"
#include "framework/Icons.h"
#include "framework/FontImpl.h"

#include "miGUI.h"

#include "Application.h"
#include "WindowMain.h"
#include "WindowMainMenu.h"

WindowMainMenu::WindowMainMenu(Application* app)
	:
	mgf::Window(app->m_GUIContext),
	m_app(app)
{
	//m_window = mgCreateWindow(app->m_GUIContext->m_gui_context, 0, 0, 300, 200);
	//m_window->titlebarFont = ((mgf::FontImpl*)app->m_backend->GetDefaultFont())->m_font;
}

WindowMainMenu::~WindowMainMenu()
{


}

// test
bool m_isCanUndo = true;
bool m_isCanRedo = false;
bool m_isSpaces = false;
int m_radioTest = 1;

void WindowMainMenu::OnMenuCommand(int id)
{
	switch (id)
	{
	case WindowMainMenu::MenuItemID_Exit:
		m_app->Quit();
	break;
	case WindowMainMenu::MenuItemID_Undo:
		m_isCanUndo = false;
		m_isCanRedo = true;
		break;
	case WindowMainMenu::MenuItemID_Redo:
		m_isCanUndo = true;
		m_isCanRedo = false;
		break;
	case WindowMainMenu::MenuItemID_Spaces:
		m_isSpaces = true;
		break;
	case WindowMainMenu::MenuItemID_Tabs:
		m_isSpaces = false;
		break;

	case WindowMainMenu::MenuItemID_Save:
		m_radioTest = 1;
		break;
	case WindowMainMenu::MenuItemID_SaveAs:
		m_radioTest = 2;
		break;
	case WindowMainMenu::MenuItemID_SaveAll:
		m_radioTest = 3;
		break;
	}
}

bool WindowMainMenu::OnIsMenuItemEnabled(int id, bool prev)
{
	switch (id)
	{
	case WindowMainMenu::MenuItemID_Undo:
		return m_isCanUndo;
	case WindowMainMenu::MenuItemID_Redo:
		return m_isCanRedo;
	case WindowMainMenu::MenuItemID_Open:
		return m_isCanUndo;
	}

	return prev;
}

bool WindowMainMenu::OnIsMenuEnabled(int id, bool prev)
{
	switch (id)
	{
	case WindowMainMenu::MenuID_Git:
		return m_isCanRedo;
	}

	return prev;
}

bool WindowMainMenu::OnIsMenuChecked(int id, bool prev)
{
	switch (id)
	{
	case WindowMainMenu::MenuItemID_Spaces:
		return m_isSpaces;
	case WindowMainMenu::MenuItemID_Tabs:
		return m_isSpaces ? false : true;

	case WindowMainMenu::MenuItemID_Save:
		return m_radioTest == 1;
	case WindowMainMenu::MenuItemID_SaveAs:
		return m_radioTest == 2;
	case WindowMainMenu::MenuItemID_SaveAll:
		return m_radioTest == 3;
	}

	return prev;
}

bool WindowMainMenu::OnIsMenuAsRadio(int id)
{
	switch (id)
	{
	case WindowMainMenu::MenuItemID_Save:
		return m_radioTest == 1;
	case WindowMainMenu::MenuItemID_SaveAs:
		return m_radioTest == 2;
	case WindowMainMenu::MenuItemID_SaveAll:
		return m_radioTest == 3;
	}
	return false;
}

bool WindowMainMenu::OnIcon(int id, mgf::Icons** icons, int* iconID, mgColor* color)
{
	bool ret = false;
	switch (id)
	{
	case WindowMainMenu::MenuItemID_Save:
	{
		if (icons)
		{
			*icons = m_app->m_guiIcons;
			*iconID = m_app->m_guiIcons_ids.m_mainMenu_FileSave;
			color->setAsIntegerRGB(0xFFFFFF);
			ret = true;
		}
	}break;
	}

	return ret;
}
