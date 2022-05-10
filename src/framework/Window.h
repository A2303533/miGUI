/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#ifndef _MGF_WINDOW_H_
#define _MGF_WINDOW_H_

#include "forward.h"

#include <string>
#include <vector>
#include <stack>

namespace mgf
{
	class Window : public BaseClass
	{
		friend class Text;
		friend class Context;
		friend class Rectangle;
		friend class Button;
		friend class TextInput;
		friend class ListBox;
		friend class Table;

	protected:
		mgWindow_s* m_window = 0;
		std::basic_string<wchar_t> m_title;
		std::vector<Element*> m_elements;

		mgMenu_s* m_menu = 0;
		/*struct _menuItem
		{
			StringW m_title;
		};
		std::vector<_menuItem> m_menuItems;
		_menuItem m_menu_currItem;*/
		Font* m_menuFont = 0;

		struct _menuPopupItemInfo
		{
			_menuPopupItemInfo()
				:
				id(0),
				enabled(true),
				icon(0),
				icon_index(0),
				popup(0)
			{}
			StringW title;
			uint32_t id;
			StringW shortcut_text;
			bool enabled;
			Icons* icon;
			uint32_t icon_index;

			mgPopup_s* popup;
		};
		/*std::vector<_menuPopupItemInfo> m_menuPopupItems[5];
		int m_menuPopupItemsIndices[5];
		int m_menuPopupItemsCurr = 0;*/
		struct _menuTreeNode
		{
			_menuTreeNode()
				:
				children(0),
				siblings(0),
				parent(0)
			{}
			_menuTreeNode* children;
			_menuTreeNode* siblings;
			_menuTreeNode* parent;

			_menuPopupItemInfo itemInfo;
		};
		struct _menuTree
		{
			_menuTree()
				:
				m_root(0)
			{}
			_menuTreeNode* m_root;
		}
		m_menuTree;
		_menuTreeNode* m_menuNodeCurr = 0;
		std::stack<_menuTreeNode*> m_menuNodeCurrPrev;

		_menuTreeNode* _menu_getLastSibling(_menuTreeNode*, int* num);
		std::vector<_menuTreeNode*> m_menuNodes; // for easy delete
		mgPopup_s* _menu_rebuild_createPopup(_menuTreeNode*);
		void _menu_addMenuItem(bool isSub, const wchar_t* title, uint32_t id, const wchar_t* shortcut_text, bool enabled);

		bool m_isVisible = true;
		bool m_useSystemWindowForPopup = false;
	public:
		Window(Context*);
		virtual ~Window();

		void SetTitle(const wchar_t* t, Font* optionalFont);
		
		void SetVisible(bool);
		bool IsVisible();

		void SetWithCloseButton(bool);
		void SetWithCollapseButton(bool);
		void SetWithTitlebar(bool);
		void SetCanMove(bool);
		void SetSize(int x, int y);
		void SetPosition(int x, int y);
		void SetDrawBG(bool);
		void SetCanDock(bool);
		void SetCanResize(bool);
		void SetCanToTop(bool);
		void SetNoMenuBG(bool);

		void SetID(int);
		int GetID();
		void SetUserData(void*);
		void* GetUserData();

		mgRect* GetMenuRect();
		const mgRect& GetRect();

		void UseMenu(bool useornot, bool useSystemWindowForPopup, Font*);
		void RebuildMenu();
		void DeleteMenu();
		void BeginMenu(const wchar_t* title, uint32_t id = 0);

		// for separator use 0 for title
		// for Enabled/Disabled use OnIsMenuItemEnabled
		void AddMenuItem(const wchar_t* title, uint32_t id, const wchar_t* shortcut_text = 0);
		void BeginSubMenu(const wchar_t* title, uint32_t id, bool enabled = true);
		void EndSubMenu();
		void EndMenu();

		virtual void OnMenuCommand(int id);
		// return true for enable, false for disable
		virtual bool OnIsMenuItemEnabled(int id, bool prev);  // like New Open Save
		virtual bool OnIsMenuEnabled(int id, bool prev);     // like File Edit View
		virtual bool OnIsMenuChecked(int id, bool prev);

		// just to know what icon need to draw. 
		// You need to use OnIsMenuChecked.
		virtual bool OnIsMenuAsRadio(int id); 

		/*return 1 if need to draw icon*/
		/*you must set icons and iconID*/
		virtual bool OnIcon(int id, Icons** icons, int* iconID, mgColor* color);

		// Size of menu item, like  [   File   ][   Edit   ]
		void SetMenuTextIndent(int val); 
		// this like  [     File ][     Edit ]
		void SetMenuTextOffset(int x, int y);

		/*void DeleteElement(Element*);
		Rectangle* AddRectangle();
		Text* AddText(int x, int y, const wchar_t* text, Font*);
		Button* AddButton();
		TextInput* AddTextInput(Font*);*/

		/*
		* arr - user data type.
		* first field must be a const wchar_t* for text
		* if multiselect then second field must be a uint32_t for flags:
		*  flags:
		*		0x1 - selected or not
		*/
		/*ListBox* AddListBox(Font*);
		Table* AddTable(uint32_t colNum, Font*);*/

		void UpdateRect();
		void Draw();

		void SetUserStyle(mgStyle_s*);
	};
}

#endif