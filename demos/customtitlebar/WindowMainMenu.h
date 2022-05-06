#pragma once
#ifndef __WINDOWMAINMENU_H_
#define __WINDOWMAINMENU_H_

class Application;
class WindowMainMenu : public mgf::Window
{
	Application* m_app;
public:
	WindowMainMenu(Application*);
	virtual ~WindowMainMenu();

	virtual void OnMenuCommand(int id) override;
	virtual bool OnIsMenuItemEnabled(int id, bool prev) override;
	virtual bool OnIsMenuEnabled(int id, bool prev) override;
	virtual bool OnIsMenuChecked(int id, bool prev) override;
	virtual bool OnIsMenuAsRadio(int id) override;
	virtual bool OnIcon(int id, mgf::Icons** icons, int* iconID, mgColor* color) override;

	enum
	{
		MenuItemID_Exit = 1,
		MenuItemID_Undo,
		MenuItemID_Redo,
		MenuItemID_Open,
		MenuItemID_Spaces,
		MenuItemID_Tabs,
		MenuItemID_Save,
		MenuItemID_SaveAs,
		MenuItemID_SaveAll,
	};

	enum
	{
		MenuID_Git = 1
	};
};

#endif
