#pragma once
#ifndef __APP_H_
#define __APP_H_

#include "framework/mgf.h"
#include "framework/GS.h"

class Application;

struct Theme
{
	mgColor m_clearColor;
	mgColor m_titlebarText;
	mgColor m_titlebarBG;
	mgColor m_titlebarSystemButtonCloseHover;
	mgColor m_titlebarSystemButtonClosePush;
	mgColor m_titlebarSystemButtonMaxHover;
	mgColor m_titlebarSystemButtonMaxPush;
	mgColor m_titlebarSystemButtonMinHover;
	mgColor m_titlebarSystemButtonMinPush;
	mgColor m_titlebarMenuHoverBG;
	mgColor m_titlebarMenuActiveBG;
	mgColor m_popupBG;
	mgColor m_popupText;
	mgColor m_popupShortcutText;
	mgColor m_popupDisabledText;
};

class WindowMain;
class WindowMainMenu;

class Application
{
	friend class WindowMain;
	friend class WindowMainMenu;

	mgf::Framework* m_framework = 0;
	WindowMain* m_windowMain = 0;
	
	mgf::Window* m_windowBG = 0;
	mgStyle m_windowBGStyle;

	//mgf::SystemWindow* m_windowMainRight = 0;
	
	Theme m_themeLight;
	Theme m_themeDark;
	Theme* m_themeCurr = 0;
	void _InitThemes();
	void SetTheme(Theme*);

	WindowMainMenu* m_mainMenuWindow = 0;
	mgStyle m_mainMenuWindowStyle;

	mgf::GS* m_gs = 0;
	mgf::Backend* m_backend = 0;
	mgf::Context* m_GUIContext = 0;

	mgf::Font* m_menuFont = 0;
	
	struct GUIIconsIDs
	{
		int m_mainMenu_logo = -1;
		int m_mainMenu_minimize = -1;
		int m_mainMenu_maximize = -1;
		int m_mainMenu_close = -1;
		int m_mainMenu_FileSave = -1;
	}m_guiIcons_ids;
	mgf::Icons* m_guiIcons = 0;

	uint32_t m_versionMaj = 0;
	uint32_t m_versionMin = 2;
	uint32_t m_versionPatch = 0;
	mgf::StringW m_titleText;

	mgColor m_colorWhite;
	mgColor m_colorRed;
	mgColor m_colorBlue;
	mgColor m_colorLime;
public:
	Application();
	~Application();

	bool Init();
	void Run();

	mgf::GS* GetGS();

	void UpdateWindowTitle();

	void Quit();
};

#endif
