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

#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"
#include "framework/Text.h"
#include "framework/Button.h"
#include "framework/TextInput.h"
#include "framework/ListBox.h"
#include "framework/Table.h"
#include "framework/FontImpl.h"
#include "framework/Icons.h"

using namespace mgf;

Window::Window(Context* ctx)
	:m_gui_context(ctx->m_gui_context)
{
	m_window = mgCreateWindow(ctx->m_gui_context, 0, 0, 300, 200);
	m_window->titlebarFont = ((mgf::FontImpl*)ctx->GetBackend()->GetDefaultFont())->m_font;
}

Window::~Window()
{
	mgf::Element** elements = m_elements.data();
	for (size_t i = 0, sz = m_elements.size(); i < sz; ++i)
	{
		delete(elements[i]);
	}

	if(m_window)
		mgDestroyWindow(m_window);
}

void Window::SetTitle(const wchar_t* t, Font* optionalFont)
{
	m_title.assign(t);
	if (optionalFont)
	{
		FontImpl* f = (FontImpl*)optionalFont;
		m_window->titlebarFont = f->m_font;
	}
	mgSetWindowTitle(m_window, m_title.data());
}

void Window::SetVisible(bool v)
{
	m_isVisible = v;
	mgShowWindow(m_window, v ? 1 : 0);
}

bool Window::IsVisible()
{
	return m_isVisible;
}

void Window::SetWithCloseButton(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_closeButton;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_closeButton)
			m_window->flags ^= mgWindowFlag_closeButton;
	}
}

void Window::SetWithCollapseButton(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_collapseButton;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_collapseButton)
			m_window->flags ^= mgWindowFlag_collapseButton;
	}
}

void Window::SetWithTitlebar(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_withTitlebar;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_withTitlebar)
			m_window->flags ^= mgWindowFlag_withTitlebar;
	}
}

void Window::SetCanMove(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_canMove;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_canMove)
			m_window->flags ^= mgWindowFlag_canMove;
	}
}

void Window::UpdateRect()
{
	m_window->rect.left = m_window->position.x;
	m_window->rect.top = m_window->position.y;
	m_window->rect.right = m_window->rect.left + m_window->size.x;
	m_window->rect.bottom = m_window->rect.top + m_window->size.y;
}

void Window::SetSize(int x, int y)
{
	m_window->size.x = x;
	m_window->size.y = y;
	//m_window->clientHeight = y;
	//m_window->contentHeight = 0;
	m_window->flagsInternal |= mgWindowFlag_internal_updateContentHeight;

	UpdateRect();
}

void Window::SetPosition(int x, int y)
{
	m_window->position.x = x;
	m_window->position.y = y;
	UpdateRect();
}

void Window::SetDrawBG(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_drawBG;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_drawBG)
			m_window->flags ^= mgWindowFlag_drawBG;
	}
}

void Window::SetCanDock(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_canDock;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_canDock)
			m_window->flags ^= mgWindowFlag_canDock;
	}
}

void Window::SetCanResize(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_canResize;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_canResize)
			m_window->flags ^= mgWindowFlag_canResize;
	}
}

void Window::SetCanToTop(bool v)
{
	if (v)
	{
		if (m_window->flags & mgWindowFlag_noToTop)
			m_window->flags ^= mgWindowFlag_noToTop;
	}
	else
	{
		m_window->flags |= mgWindowFlag_noToTop;
	}
}

void Window::SetID(int i)
{
	m_window->id = i;
}

int Window::GetID()
{
	return m_window->id;
}

void Window::SetUserData(void* d)
{
	m_window->userData = d;
}

void* Window::GetUserData()
{
	return m_window->userData;
}

//void Window::DeleteElement(Element* e)
//{
//	assert(e);
//	auto elements = m_elements.data();
//	for (size_t i = 0, sz = m_elements.size(); i < sz; ++i)
//	{
//		if (elements[i] != e)
//			continue;
//
//		m_elements.erase(m_elements.begin() + i);
//		break;
//	}
//	delete e;
//}
//
//mgf::Rectangle* Window::AddRectangle()
//{
//	Rectangle* e = new Rectangle(this);
//	m_elements.emplace_back(e);
//	return e;
//}
//
//Text* Window::AddText(int x, int y, const wchar_t* text, Font* f)
//{
//	Text* e = new Text(this, text, f);
//	m_elements.emplace_back(e);
//	e->SetPosition(x, y);
//	return e;
//}
//
//Button* Window::AddButton()
//{
//	Button* e = new Button(this);
//	m_elements.emplace_back(e);
//	return e;
//}
//
//TextInput* Window::AddTextInput(Font* f)
//{
//	TextInput* e = new TextInput(this, f);
//	m_elements.emplace_back(e);
//	return e;
//}
//
//ListBox* Window::AddListBox(Font* f)
//{
//	ListBox* e = new ListBox(this, f);
//	m_elements.emplace_back(e);
//	return e;
//}
//
//Table* Window::AddTable(uint32_t colNum, Font* f)
//{
//	Table* e = new Table(this, colNum, f);
//	m_elements.emplace_back(e);
//	return e;
//}

void Window::SetMenuTextOffset(int x, int y)
{
	if (m_menu)
	{
		m_menu->textOffset.x = x;
		m_menu->textOffset.y = y;
	}
}

void Window::SetMenuTextIndent(int val)
{
	if (m_menu)
		m_menu->textIndent = val;
}

void Window::UseMenu(bool v, bool useSystemWindowForPopup, Font* f)
{
	m_menuFont = f;
	m_useSystemWindowForPopup = useSystemWindowForPopup;

	if (m_menu)
		m_menu->font = ((FontImpl*)f)->m_font;

	if (v)
		m_window->menu = m_menu;
	else
		m_window->menu = 0;
}

void Window_menuCallback(int id, struct mgPopupItem_s* pi)
{
	Window* window = (Window*)pi->userData;
	window->OnMenuCommand(id);
}
void Window::OnMenuCommand(int id){}

int Window_onIsItemEnabled(struct mgContext_s*, struct mgPopup_s*, struct mgPopupItem_s* pi)
{
	Window* window = (Window*)pi->userData;
	return window->OnIsMenuItemEnabled(pi->info.id, pi->info.isEnabled);
}
bool Window::OnIsMenuItemEnabled(int id, bool prev) { return prev; }

int Window_onIsItemChecked(struct mgContext_s*, struct mgPopup_s*, struct mgPopupItem_s* pi)
{
	Window* window = (Window*)pi->userData;
	return window->OnIsMenuChecked(pi->info.id, pi->info.isChecked);
}
bool Window::OnIsMenuChecked(int id, bool prev) { return prev; }

int Window_onIsItemRadio(struct mgContext_s*, struct mgPopup_s*, struct mgPopupItem_s* pi)
{
	Window* window = (Window*)pi->userData;
	return window->OnIsMenuAsRadio(pi->info.id);
}
bool Window::OnIsMenuAsRadio(int id) { return false; }

int Window_onIcon(struct mgContext_s*, struct mgPopup_s*, struct mgPopupItem_s* pi,
	struct mgIcons_s** icons, int* iconID, mgColor* color)
{
	Window* window = (Window*)pi->userData;

	mgf::Icons* mgf_icons = 0;

	int result = (int)window->OnIcon(pi->info.id, &mgf_icons, iconID, color);

	if (mgf_icons)
	{
		*icons = mgf_icons->GetMGIcons();
	}

	return result;
}
bool Window::OnIcon(int id, Icons** icons, int* iconID, mgColor* color) { return false; }


mgPopup* Window::_menu_rebuild_createPopup(_menuTreeNode* firstNode)
{
	std::vector<mgPopupItemInfo_s> popupItems;

	_menuTreeNode* curr = firstNode;
	while (true)
	{
		mgPopupItemInfo_s info;
		memset(&info, 0, sizeof(mgPopupItemInfo_s));


		if (curr->children)
		{
			// another popup
			info.subMenu = _menu_rebuild_createPopup(curr->children);
		}

		info.callback = Window_menuCallback;
		info.id = curr->itemInfo.id;
		info.isChecked = 0;
		info.isEnabled = curr->itemInfo.enabled ? 1 : 0;
		info.shortcutText = curr->itemInfo.shortcut_text.data();
		info.text = curr->itemInfo.title.size() ? curr->itemInfo.title.data() : 0;
		info.type = info.text ? mgPopupItemType_default : mgPopupItemType_separator;

		popupItems.push_back(info);

		curr = curr->siblings;
		if (!curr)
			break;
	}

	mgPopup* newPopup = 0;
	if (popupItems.size())
	{
		int flags = 0;
		if (m_useSystemWindowForPopup)
		{
			flags |= mgPopupFlags_systemWindow;
		}

		newPopup = mgCreatePopup(m_gui_context, popupItems.data(), (int)popupItems.size(), ((FontImpl*)m_menuFont)->m_font, flags);
		newPopup->onIsItemEnabled = Window_onIsItemEnabled;
		newPopup->onIsItemChecked = Window_onIsItemChecked;
		newPopup->onIsItemRadio = Window_onIsItemRadio;
		newPopup->onIcon = Window_onIcon;
		for (int i = 0; i < newPopup->itemsSize; ++i)
		{
			newPopup->items[i].userData = this;
		}
	}

	return newPopup;
}


int Window_onIsMenuItemEnabled(struct mgMenuItem_s* mi)
{
	Window* window = (Window*)mi->userData;
	return window->OnIsMenuEnabled(mi->info.id, (bool)mi->isEnabled);
}
bool Window::OnIsMenuEnabled(int id, bool prev) { return prev; }

void Window::RebuildMenu()
{
	if (m_menu)
		mgDestroyMenu(m_menu);
	
	if (!m_menuTree.m_root)
		return;

	int num = 0;
	_menuTreeNode* lastSib = _menu_getLastSibling(m_menuTree.m_root, &num);

	mgMenuItemInfo* mii = new mgMenuItemInfo[num];
	int index = 0;

	_menuTreeNode* currSib = m_menuTree.m_root;
	while(true)
	{
		mii[index].popup = 0;
		mii[index].id = currSib->itemInfo.id;

		if (currSib->children) // if node has children then crete new popup
		{
			mii[index].popup = _menu_rebuild_createPopup(currSib->children);
		}

		mii[index].text = currSib->itemInfo.title.c_str();
		++index;

		if (currSib == lastSib)
			break;
		
		currSib = currSib->siblings;
	}

	m_menu = mgCreateMenu(m_window->context, mii, num, ((FontImpl*)m_menuFont)->m_font);
	for (int i = 0; i < num; ++i)
	{
		m_menu->items[i].userData = this;
	}
	m_menu->onIsItemEnabled = Window_onIsMenuItemEnabled;

	delete[] mii;

	UseMenu(true, m_useSystemWindowForPopup, m_menuFont);
}


void Window::DeleteMenu()
{
	UseMenu(false, m_useSystemWindowForPopup, m_menuFont);
	if (m_menu)
		mgDestroyMenu(m_menu);
	m_menu = 0;
	for (size_t i = 0; i < m_menuNodes.size(); ++i)
	{
		delete m_menuNodes[i];
	}
	m_menuNodes.clear();
}

Window::_menuTreeNode* Window::_menu_getLastSibling(_menuTreeNode* node, int* num)
{
	*num = 1;

	if (node->siblings)
	{
		_menuTreeNode* sib = node->siblings;
	bgn:;
		*num += 1;
		if (sib == node)
			return node;

		if (!sib->siblings)
			return sib;

		sib = sib->siblings;
		goto bgn;
	}
	return node;
}

void Window::BeginMenu(const wchar_t* title, uint32_t id)
{
	_menuTreeNode* newNode = new _menuTreeNode;
	newNode->itemInfo.id = id;
	newNode->itemInfo.title = title;
	if (!m_menuTree.m_root)
	{
		m_menuTree.m_root = newNode;
	}
	else
	{
		int num = 0;
		_menuTreeNode* lastSib = _menu_getLastSibling(m_menuTree.m_root, &num);
		lastSib->siblings = newNode;
	}
	m_menuNodeCurr = newNode;
	m_menuNodes.push_back(m_menuNodeCurr);
}

void Window::_menu_addMenuItem(
	bool isSub, 
	const wchar_t* title, 
	uint32_t id, 
	const wchar_t* shortcut_text, 
	bool enabled)
{
	_menuPopupItemInfo info;
	info.enabled = enabled;
	info.icon = 0;
	info.icon_index = 0;
	info.id = id;
	info.shortcut_text = shortcut_text;
	info.title = title;

	_menuTreeNode* newNode = new _menuTreeNode;
	newNode->itemInfo = info;

	if (!m_menuNodeCurr->children)
	{
		m_menuNodeCurr->children = newNode;
	}
	else
	{
		int num = 0;
		_menuTreeNode* lastSib = _menu_getLastSibling(m_menuNodeCurr->children, &num);
		lastSib->siblings = newNode;
	}

	if (isSub)
	{
		m_menuNodeCurrPrev.push(m_menuNodeCurr);
		m_menuNodeCurr = newNode;
	}

	m_menuNodes.push_back(newNode);
}

void Window::BeginSubMenu(
	const wchar_t* title, 
	uint32_t id,
	bool enabled)
{	
	_menu_addMenuItem(true, title, id, 0, enabled);
}

void Window::EndSubMenu()
{
	m_menuNodeCurr = m_menuNodeCurrPrev.top();
	m_menuNodeCurrPrev.pop();
}

void Window::AddMenuItem(
	const wchar_t* title, 
	uint32_t id,
	const wchar_t* shortcut_text)
{
	_menu_addMenuItem(false, title, id, shortcut_text, true);
}

void Window::EndMenu()
{
}

void Window::Draw()
{
	mgDrawWindow(m_window);
}

void Window::SetUserStyle(mgStyle_s* s)
{
	m_window->userStyle = s;
}

void Window::SetNoMenuBG(bool v)
{
	if (v)
	{
		m_window->flags |= mgWindowFlag_noMenuBG;
	}
	else
	{
		if (m_window->flags & mgWindowFlag_noMenuBG)
			m_window->flags ^= mgWindowFlag_noMenuBG;
	}
}

mgRect* Window::GetMenuRect()
{
	return &m_window->menuRect;
}

const mgRect& Window::GetRect()
{
	return m_window->rect;
}

