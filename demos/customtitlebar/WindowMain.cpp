#include "framework/mgf.h"
#include "framework/SystemWindow.h"
#include "framework/Window.h"
#include "framework/Context.h"
#include "framework/Icons.h"
#include "framework/Font.h"

#include "miGUI.h"

#include "Application.h"
#include "WindowMain.h"
#include "WindowMainMenu.h"

WindowMain::WindowMain(Application* app, int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize)
	:
	m_app(app),
	mgf::SystemWindow(windowFlags, windowPosition, windowSize)
{
	GetSizeMinimum()->x = 700;
	GetSizeMinimum()->y = 500;

	
}

WindowMain::~WindowMain()
{
}

void WindowMain::OnSize()
{
	mgf::SystemWindow::OnSize();
	auto& wsz = GetSize();
	if (m_app)
	{
		if(m_app->m_mainMenuWindow)
			m_app->m_mainMenuWindow->SetSize(wsz.x - TitlebarRightPartSize, 0);

		//m_app->m_windowBG->SetPosition(m_app->m_mainMenuWindow->GetRect().left, m_app->m_mainMenuWindow->GetRect().bottom);
		if(m_app->m_windowBG)
			m_app->m_windowBG->SetSize(wsz.x, wsz.y);
	}

	m_pushedSystemButton = 0;
}

bool WindowMain::OnHTCaption()
{
	auto hitRect = *m_app->m_windowMain->GetCustomTitleBarHitRect();
	// cursor already in this rect

	if (m_app->m_GUIContext->m_gui_context->cursorInElement)
		return false;
	if (m_app->m_GUIContext->m_gui_context->cursorInPopup)
	{
		return false;
	}
	if (m_cursorInSystemButtons)
		return false;

	return true;
}

void WindowMain::OnDrawCustomTitleBar()
{
	mgRect* menuRect = m_app->m_mainMenuWindow->GetMenuRect();
	if (!m_app->m_framework->m_run)
		return;


	auto& wsz = GetSize();

	mgRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = wsz.x;
	rect.bottom = menuRect->bottom;

	m_app->m_backend->SetClipRect(&rect);
	m_app->m_backend->DrawRectangle(0, 0, &rect,
		&m_app->m_mainMenuWindowStyle.windowMenuBG,
		&m_app->m_mainMenuWindowStyle.windowMenuBG,
		0, 0);

	//mgColor c;
	//c.setAsIntegerRGB(0xff0000);
	//m_app->m_backend->SetClipRect(menuRect);
	//m_app->m_backend->DrawRectangle(0, 0, menuRect, &c, &c, 0, 0);

	*m_app->m_windowMain->GetCustomTitleBarHitRect() = rect;

	rect.left = menuRect->left - 29 + 1;
	rect.top = menuRect->top + 2;
	rect.right = rect.left + 29;
	rect.bottom = rect.top + 15;


	auto iconsTexture = m_app->m_guiIcons->GetMGTexture();
	m_app->m_backend->SetIcon(m_app->m_guiIcons, m_app->m_guiIcons_ids.m_mainMenu_logo);

	m_app->m_backend->DrawRectangle(mgDrawRectangleReason_buttonIcon, 0, &rect,
		&m_app->m_colorWhite,
		&m_app->m_colorWhite,
		iconsTexture, 0);

	m_app->m_mainMenuWindow->Draw();


	rect.right = m_app->m_windowMain->GetSize().x;
	rect.bottom = m_app->m_windowMain->GetSize().y;

	int rightPartSize = rect.right - menuRect->right;
	int rightPartSizeHalf = (int)((float)rightPartSize * 0.5f);

	m_app->m_backend->SetClipRect(&rect);
	mgPoint textPos;
	{
		textPos.x = menuRect->right + (int)((float)rightPartSize * 0.125f);
		textPos.y = menuRect->top+3;
	}
	m_app->m_backend->DrawText(0, 0, &textPos, m_app->m_titleText.data(), m_app->m_titleText.size(),
		&m_app->m_mainMenuWindowStyle.windowMenuText, m_app->m_menuFont->GetMGFont());

	// buttons
	{
		rect.top = 0;

		int xOffset = 0;
		if (m_isMaximized)
		{
			rect.top = 9;
			xOffset = 7;
		}


		rect.left = wsz.x - 32 - xOffset;
		rect.right = wsz.x - 5 - xOffset;

		rect.bottom = menuRect->top + 17;
		m_app->m_backend->SetClipRect(&rect);

		mgRect rcClose, rcMax, rcMin;

		rcClose = rect;

		m_cursorInSystemButtons = 0;

		if (m_app->m_GUIContext->PointInRect(&rect))
			m_cursorInSystemButtons = 1;

		if (m_pushedSystemButton)
		{
			if (m_pushedSystemButton == 1)
			{
				m_app->m_backend->DrawRectangle(0, 0, &rect,
					&m_app->m_themeCurr->m_titlebarSystemButtonClosePush,
					&m_app->m_themeCurr->m_titlebarSystemButtonClosePush,
					0, 0);
			}
		}
		else if (m_cursorInSystemButtons == 1)
		{
			m_app->m_backend->DrawRectangle(0, 0, &rect,
				&m_app->m_themeCurr->m_titlebarSystemButtonCloseHover,
				&m_app->m_themeCurr->m_titlebarSystemButtonCloseHover,
				0, 0);
		}

		m_app->m_backend->SetIcon(m_app->m_guiIcons, m_app->m_guiIcons_ids.m_mainMenu_close);
		m_app->m_backend->DrawRectangle(mgDrawRectangleReason_buttonIcon, 0, &rect,
			&m_app->m_colorWhite,
			&m_app->m_colorWhite,
			iconsTexture, 0);

		rect.left -= 17;
		rect.right -= 28;
		m_app->m_backend->SetClipRect(&rect);
		rcMax = rect;

		if (m_app->m_GUIContext->PointInRect(&rect))
			m_cursorInSystemButtons = 2;

		if (m_pushedSystemButton)
		{
			if (m_pushedSystemButton == 2)
			{
				m_app->m_backend->DrawRectangle(0, 0, &rect,
					&m_app->m_themeCurr->m_titlebarSystemButtonMaxPush,
					&m_app->m_themeCurr->m_titlebarSystemButtonMaxPush,
					0, 0);
			}
		}
		else if (m_cursorInSystemButtons == 2)
		{
			m_app->m_backend->DrawRectangle(0, 0, &rect,
				&m_app->m_themeCurr->m_titlebarSystemButtonMaxHover,
				&m_app->m_themeCurr->m_titlebarSystemButtonMaxHover,
				0, 0);
		}

		m_app->m_backend->SetIcon(m_app->m_guiIcons, m_app->m_guiIcons_ids.m_mainMenu_maximize);
		m_app->m_backend->DrawRectangle(mgDrawRectangleReason_buttonIcon, 0, &rect,
			&m_app->m_colorLime,
			&m_app->m_colorLime,
			iconsTexture, 0);

		rect.left -= 17;
		rect.right -= 17;
		m_app->m_backend->SetClipRect(&rect);
		rcMin = rect;

		if (m_app->m_GUIContext->PointInRect(&rect))
			m_cursorInSystemButtons = 3;

		if (m_pushedSystemButton)
		{
			if (m_pushedSystemButton == 3)
			{
				m_app->m_backend->DrawRectangle(0, 0, &rect,
					&m_app->m_themeCurr->m_titlebarSystemButtonMinPush,
					&m_app->m_themeCurr->m_titlebarSystemButtonMinPush,
					0, 0);
			}
		}
		else if (m_cursorInSystemButtons == 3)
		{
			m_app->m_backend->DrawRectangle(0, 0, &rect,
				&m_app->m_themeCurr->m_titlebarSystemButtonMinHover,
				&m_app->m_themeCurr->m_titlebarSystemButtonMinHover,
				0, 0);
		}

		m_app->m_backend->SetIcon(m_app->m_guiIcons, m_app->m_guiIcons_ids.m_mainMenu_minimize);
		m_app->m_backend->DrawRectangle(mgDrawRectangleReason_buttonIcon, 0, &rect,
			&m_app->m_colorBlue,
			&m_app->m_colorBlue,
			iconsTexture, 0);

		// on window size lmb down will be many times. need to block it
		static bool pressOnlyOnce = false;

		if (m_cursorInSystemButtons)
		{
			if (m_app->m_GUIContext->IsLMBDown() && !pressOnlyOnce)
			{
				pressOnlyOnce = true;
				m_pushedSystemButton = m_cursorInSystemButtons;
			}
		}

		if (m_app->m_GUIContext->IsLMBUp())
		{
			pressOnlyOnce = false;
			switch (m_pushedSystemButton)
			{
			case 1:
			{
				if (m_app->m_GUIContext->PointInRect(&rcClose))
					m_app->Quit();
			}break;
			case 2:
			{
				if (m_app->m_GUIContext->PointInRect(&rcMax))
				{
					if (this->m_isMaximized)
						this->Restore();
					else
						this->Maximize();
				}
			}break;
			case 3:
			{
				if (m_app->m_GUIContext->PointInRect(&rcMin))
					this->Minimize();
			}break;
			}

			m_pushedSystemButton = 0;
		}
	}
}

void WindowMain::OnMaximize()
{
	mgf::SystemWindow::OnMaximize();
	if (m_app->m_mainMenuWindow)
		m_app->m_mainMenuWindow->SetPosition(MainMenuPositionX, MainMenuPositionY);
	m_isMaximized = true;
}

void WindowMain::OnRestore()
{
	mgf::SystemWindow::OnRestore();
	if(m_app->m_mainMenuWindow)
		m_app->m_mainMenuWindow->SetPosition(MainMenuPositionXWindowed, 0);
	m_isMaximized = false;
}

void WindowMain::OnMove()
{
	if (m_app->m_mainMenuWindow)
		m_app->m_mainMenuWindow->SetPosition(MainMenuPositionXWindowed, 0);

	m_isMaximized = false;
	OnDraw();
}
