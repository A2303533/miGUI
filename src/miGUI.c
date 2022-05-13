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
#include "mgFunctions.h"

#include <time.h>
#include <wchar.h>

void mgInitDefaultCursors(mgContext* c);
void mgDestroyDefaultCursors(mgContext* c);
void mgUpdateWindow(struct mgWindow_s* w);
void mgDrawDockPanel(struct mgContext_s* c);
void mgDockPanelOnSize(struct mgContext_s* c);
void mgDockPanelUpdate(struct mgContext_s* c);
void mgDrawPopup_f(struct mgContext_s* c, mgPopup* p);
void mgUpdatePopup(struct mgContext_s* c, mgPopup* p);
void mgDockPanelClear(struct mgContext_s* c);

/*
* sometimes need to stop update elements, like when double click in list box 
* for activating text input.
* Reset in StartFrame
*/
int g_skipFrame = 0;

void 
mgDestroyElement_internal(mgElement* e)
{
	assert(e);
	for (uint32_t i = 0; i < e->childrenCount; ++i)
	{
		mgDestroyElement_internal(e->children[i].pointer);
	}

	if (e->implementation)
		free(e->implementation);
	free(e);
}

MG_API
void MG_C_DECL
mgDestroyElement_f(mgElement* e)
{
	assert(e);

	mgElement* parent = e->parent;
	mgDestroyElement_internal(e);
	if (!parent)
		return;
	
	if (parent->childrenCount == 1)
	{
		free(parent->children);
		parent->children = 0;
		parent->childrenCount = 0;
	}
	else
	{
		struct mgElementNode_s* newChildren = malloc(sizeof(struct mgElementNode_s) * (parent->childrenCount - 1));
		for (uint32_t i = 0, i2 = 0; i < parent->childrenCount; ++i)
		{
			if (parent->children[i].pointer == e)
				continue;

			newChildren[i2] = parent->children[i];
			++i2;
		}
		--parent->childrenCount;

		free(parent->children);
		parent->children = newChildren;
	}
}

MG_API
void MG_C_DECL
mgInitStyleDark_f(mgStyle* s)
{
	assert(s);
}

MG_API
void MG_C_DECL
mgInitStyleLight_f(mgStyle* s)
{
	assert(s);
	mgColorSetAsIntegerRGB(&s->button1, 0x999999);
	mgColorSetAsIntegerRGB(&s->button2, 0x666666);
	mgColorSetAsIntegerRGB(&s->buttonDisabled1, 0x999999);
	mgColorSetAsIntegerRGB(&s->buttonDisabled2, 0x666666);
	mgColorSetAsIntegerRGB(&s->buttonHover1, 0xAAAAAA);
	mgColorSetAsIntegerRGB(&s->buttonHover2, 0x777777);
	mgColorSetAsIntegerRGB(&s->buttonPress1, 0x777777);
	mgColorSetAsIntegerRGB(&s->buttonPress2, 0x444444);
	mgColorSetAsIntegerRGB(&s->buttonText, 0x0);
	mgColorSetAsIntegerRGB(&s->buttonTextHover, 0x222222);
	mgColorSetAsIntegerRGB(&s->buttonTextPress, 0x0);
	mgColorSetAsIntegerRGB(&s->buttonTextDisabled, 0x555555);
	mgColorSetAsIntegerRGB(&s->windowBG, 0xE1E6F7);
	mgColorSetAsIntegerRGB(&s->windowTitlebar, 0xC9D6F2);
	mgColorSetAsIntegerRGB(&s->windowBGTopWindow, 0xE8EDFF);
	mgColorSetAsIntegerRGB(&s->windowTitlebarTopWindow, 0xB5CCFF);
	mgColorSetAsIntegerRGB(&s->windowTitlebarText, 0x0);
	mgColorSetAsIntegerRGB(&s->dockpanelBG, 0xFFFFFF);
	mgColorSetAsIntegerRGB(&s->dockpanelWindowToDock, 0x009BFF);
	mgColorSetAsIntegerRGB(&s->dockpanelTabBG, 0x009BFF);
	mgColorSetAsIntegerRGB(&s->dockpanelSplitterBG, 0xC4C4C4);
	mgColorSetAsIntegerRGB(&s->dockpanelPanelSplitterBG, 0xC8C8C8);
	mgColorSetAsIntegerRGB(&s->dockpanelTabWindowTitleBG, 0xE5F3FF);
	mgColorSetAsIntegerRGB(&s->dockpanelTabActiveWindowTitleBG, 0xB2D2FF);
	mgColorSetAsIntegerRGB(&s->popupBG, 0xE8EDFF);
	mgColorSetAsIntegerRGB(&s->popupText, 0x0);
	mgColorSetAsIntegerRGB(&s->popupTextShortcut, 0x555555);
	mgColorSetAsIntegerRGB(&s->popupTextDisabled, 0x888888);
	mgColorSetAsIntegerRGB(&s->popupTextShortcutDisabled, 0x888888);
	mgColorSetAsIntegerRGB(&s->popupHoverElementBG, 0xC1C1C1);
	mgColorSetAsIntegerRGB(&s->windowScrollbarBG, 0xD4D2EF);
	mgColorSetAsIntegerRGB(&s->windowScrollbarElement, 0xB3C4DB);
	mgColorSetAsIntegerRGB(&s->popupSeparator, 0xC1C1C1);
	mgColorSetAsIntegerRGB(&s->windowMenuBG, 0xD6E1FF);
	mgColorSetAsIntegerRGB(&s->windowMenuHoverItemBG, 0x9EC0FF);
	mgColorSetAsIntegerRGB(&s->windowMenuActiveItemBG, 0x00E1FF);
	mgColorSetAsIntegerRGB(&s->tooltipBG, 0xD6E5FF);
	mgColorSetAsIntegerRGB(&s->tooltipText, 0x0);
	mgColorSetAsIntegerRGB(&s->textInputBGActive, 0x333333);
	mgColorSetAsIntegerRGB(&s->textInputBGNotActive, 0x999999);
	mgColorSetAsIntegerRGB(&s->textInputText, 0xAAAAAA);
	mgColorSetAsIntegerRGB(&s->textInputTextSelected, 0xFFFFFF);
	mgColorSetAsIntegerRGB(&s->textInputDefaultText, 0x666666);
	mgColorSetAsIntegerRGB(&s->textInputCursor, 0xffffff);
	mgColorSetAsIntegerRGB(&s->textInputSelectionBGActive, 0x009BFF);
	mgColorSetAsIntegerRGB(&s->textInputSelectionBGNotActive, 0x888888);
	mgColorSetAsIntegerRGB(&s->listBG, 0x888888);
	mgColorSetAsIntegerRGB(&s->listItemText, 0x0);
	mgColorSetAsIntegerRGB(&s->listItemHoverBG, 0xC1C1C1);
	mgColorSetAsIntegerRGB(&s->listItemSelectedBG, 0xE1EEEF);
	mgColorSetAsIntegerRGB(&s->listItemBG1, 0x888888);
	mgColorSetAsIntegerRGB(&s->listItemBG2, 0x999999);
	mgColorSetAsIntegerRGB(&s->tableBG, 0xFFFFFF);
	mgColorSetAsIntegerRGB(&s->tableRowBG1, 0xE1EEEF);
	mgColorSetAsIntegerRGB(&s->tableRowBG2, 0xE1EEEF);
	mgColorSetAsIntegerRGB(&s->tableRowHoverBG, 0xF9F7FF);
	mgColorSetAsIntegerRGB(&s->tableRowSelectedBG, 0xFAFFA3);
	mgColorSetAsIntegerRGB(&s->tableCellText, 0x0);
	mgColorSetAsIntegerRGB(&s->tableCellBG, 0xE1EEEF);
	mgColorSetAsIntegerRGB(&s->tableColTitleBG, 0xD1E4FF);
	mgColorSetAsIntegerRGB(&s->tableColTitleColBG, 0xB2D2FF);
	mgColorSetAsIntegerRGB(&s->tableColTitleColHover, 0xEFF6FF);
	mgColorSetAsIntegerRGB(&s->tableColTitleColActive, 0xB2D2FF);
	mgColorSetAsIntegerRGB(&s->windowMenuText, 0x0);
	mgColorSetAsIntegerRGB(&s->windowMenuTextDisabled, 0x343434);
	mgColorSetAsIntegerRGB(&s->popupIconCheck, 0xFFFFFF);
	mgColorSetAsIntegerRGB(&s->popupIconNext, 0xFF0000);
	mgColorSetAsIntegerRGB(&s->windowIconClose, 0x0);
	mgColorSetAsIntegerRGB(&s->windowIconExpand, 0x0);
	
}

MG_API 
mgContext* MG_C_DECL
mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input)
{
	assert(gpu);
	assert(input);

	mgContext* c = calloc(1, sizeof(mgContext));
	mgColorSet(&c->whiteColor, 1.f, 1.f, 1.f, 1.f);
	c->gpu = gpu;
	c->input = input;
	c->input->mousePosition.x = -1000;
	c->input->mousePosition.y = -1000;
	c->needUpdateTransform = 1;
	c->needRebuild = 1;
	c->activeStyle = &c->styleLight;
	c->deltaTime = 0.f;

	mgInitStyleLight_f(&c->styleLight);
	c->functions.SetCursor_p = mgSetCursor_f;

	mgInitDefaultCursors(c);


	return c;
}

struct mgPopup_s*
mgGetDefaultPopupTextInput(mgContext* c)
{
	assert(c);
	assert(c->defaultPopupFont);

	if (c->defaultPopupForTextInput)
		return c->defaultPopupForTextInput;
	struct mgPopupItemInfo_s items[] = {
		{0, L"Cut", 0, 0, mgPopupItemType_default, 0, L"Ctl+X", 1},
		{0, L"Copy", 0, 0, mgPopupItemType_default, 0, L"Ctl+C", 1},
		{0, L"Paste", 0, 0, mgPopupItemType_default, 0, L"Ctl+V", 1},
		{0, L"Delete", 0, 0, mgPopupItemType_default, 0, 0, 1},
		{0, L"Select All", 0, 0, mgPopupItemType_default, 0, L"Ctrl+A", 1},
	};
	c->defaultPopupForTextInput = mgCreatePopup_f(c, items, 5, c->defaultPopupFont, 0);
	return c->defaultPopupForTextInput;
}

MG_API 
void MG_C_DECL
mgDestroyContext_f(mgContext* c)
{
	assert(c);

	mgDestroyDefaultCursors(c);

	if (c->defaultIconGroup)
	{
		if (c->defaultIconGroup->icons)
			mgDestroyIcons_f(c->defaultIconGroup->icons);
		free(c->defaultIconGroup);
	}

	if (c->defaultPopupForTextInput)
		mgDestroyPopup_f(c->defaultPopupForTextInput);

	mgWindow* cw = c->firstWindow;
	if (cw)
	{
		mgWindow* lw = cw->left;
		while (1)
		{
			mgWindow* nw = cw->right;
			mgDestroyWindow_f(cw);
			if (cw == lw)
				break;
			cw = nw;
		}
	}

	if (c->dockPanel)
	{
		if (c->dockPanel->windowTabPopup)
		{
			c->activePopup = 0;
			mgDestroyPopup_f(c->dockPanel->windowTabPopup);
		}

		mgDockPanelClear(c);

		if (c->dockPanel->elements)
			free(c->dockPanel->elements);
		free(c->dockPanel);
	}

	free(c);
}

MG_API
int MG_C_DECL
mgInitDefaultIcons_f(struct mgContext_s* c, mgTexture* t)
{
	if (c->defaultIconGroup)
		return 0;

	c->defaultIconGroup = calloc(1, sizeof(mgIconGroup));
	c->defaultIconGroup->icons = mgCreateIcons_f(t, 512, 512, 8);
	
	c->defaultIconGroup->windowCloseButton = 0;
	mgSetIcon_f(c->defaultIconGroup->icons, 0, 15, 2, 11, 11); //close wnd

	c->defaultIconGroup->windowCloseButtonMouseHover = 1;
	mgSetIcon_f(c->defaultIconGroup->icons, 1, 2, 2, 11, 11); // close wnd mouse hover

	c->defaultIconGroup->windowCloseButtonPress = 2;
	mgSetIcon_f(c->defaultIconGroup->icons, 2, 29, 2, 11, 11); // close wnd push

	c->defaultIconGroup->windowCollapseButton = 3;
	mgSetIcon_f(c->defaultIconGroup->icons, 3, 42, 2, 11, 11); // collapse wnd

	c->defaultIconGroup->windowExpandButton = 4;
	mgSetIcon_f(c->defaultIconGroup->icons, 4, 53, 2, 11, 11); // expand wnd

	c->defaultIconGroup->popupNext = 5;
	mgSetIcon_f(c->defaultIconGroup->icons, 5, 64, 2, 11, 11); // popup next

	c->defaultIconGroup->popupCheck = 6;
	mgSetIcon_f(c->defaultIconGroup->icons, 6, 78, 0, 12, 13);

	c->defaultIconGroup->popupCheckRadio = 7;
	mgSetIcon_f(c->defaultIconGroup->icons, 7, 90, 0, 12, 13);
	
	return 1;
}

void
mgUpdateElementLMBClick(mgElement* e)
{
	if (e->childrenCount)
	{
		for (uint32_t i = e->childrenCount - 1; i >= 0; --i)
		{
			mgUpdateElementLMBClick(e->children[i].pointer);

			if (!i)
				break;
		}
	}

	e->lmb2ClickTimer += e->window->context->deltaTime;
	if (e->cursorInRect)
	{
		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			++e->lmbClickCount;
			e->lmb2ClickTimer = 0.f;
		}
	}
	if (e->lmb2ClickTimer > e->lmb2ClickTimerLimit)
		e->lmbClickCount = 0;
}

void
mgUpdateElement(mgElement* e) 
{
	if (!e->visible)
		return;

	if (e->childrenCount)
	{
		for (uint32_t i = e->childrenCount - 1; i >= 0; --i)
		{
			mgUpdateElement(e->children[i].pointer);

			if (!i)
				break;
		}
	}

	if(!g_skipFrame)
		e->onUpdate(e);
}

void
mgUpdateTransformElement(mgElement* e)
{
	if (!e->visible)
		return;

	e->onUpdateTransform(e);
	for (uint32_t i = 0; i < e->childrenCount; ++i)
	{
		mgUpdateTransformElement(e->children[i].pointer);
	}
}

void
mgRebuildElement(mgElement* e)
{
	/*if (!e->visible)
		return;*/

	e->onRebuild(e);
	for (uint32_t i = 0; i < e->childrenCount; ++i)
	{
		mgRebuildElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgUpdate_f(mgContext* c)
{
	assert(c);

	c->tooltipText = 0;
	c->cursorInElement = 0;

	unsigned int ctrl_shift_alt = 0;
	if (mgIsKeyHold(c->input, MG_KEY_LALT) || mgIsKeyHold(c->input, MG_KEY_RALT))
		ctrl_shift_alt |= 1;

	if (mgIsKeyHold(c->input, MG_KEY_LSHIFT) || mgIsKeyHold(c->input, MG_KEY_RSHIFT))
		ctrl_shift_alt |= 2;

	if (mgIsKeyHold(c->input, MG_KEY_LCTRL) || mgIsKeyHold(c->input, MG_KEY_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  c->input->keyboardModifier = MG_KBMOD_clear;         break;
	case 1:  c->input->keyboardModifier = MG_KBMOD_ALT;           break;
	case 2:  c->input->keyboardModifier = MG_KBMOD_SHIFT;         break;
	case 3:  c->input->keyboardModifier = MG_KBMOD_SHIFTALT;      break;
	case 4:  c->input->keyboardModifier = MG_KBMOD_CTRL;          break;
	case 5:  c->input->keyboardModifier = MG_KBMOD_CTRLALT;       break;
	case 6:  c->input->keyboardModifier = MG_KBMOD_CTRLSHIFT;     break;
	case 7:  c->input->keyboardModifier = MG_KBMOD_CTRLSHIFTALT;  break;
	}

	c->input->mouseMoveDelta.x = c->input->mousePosition.x - c->input->mousePositionOld.x;
	c->input->mouseMoveDelta.y = c->input->mousePosition.y - c->input->mousePositionOld.y;

	static clock_t then = 0;
	clock_t now = clock();

	c->deltaTime = (float)(now - then) / CLOCKS_PER_SEC;
	then = now;

	static float lmb_dbl_timer = 0.f;
	lmb_dbl_timer += c->deltaTime;
	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
	{
		++c->input->LMBClickCount;
		lmb_dbl_timer = 0.f;
	}
	if (lmb_dbl_timer > 0.3f)
		c->input->LMBClickCount = 0;

	c->windowUnderCursor = 0;
	c->popupUnderCursor = 0;
	if (c->activePopup)
	{
		c->cursorInPopup = 0;
		mgUpdatePopup(c, c->activePopup);

		if (!c->cursorInPopup)
		{
			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				mgShowPopup_f(c, 0, 0);
			if (c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
				mgShowPopup_f(c, 0, 0);
			if (c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN)
				mgShowPopup_f(c, 0, 0);
		}

		if (!c->activeMenu)
			return;
	}

	mgWindow* cw = c->firstWindow;

	int ok = 1;
	if (c->dockPanel)
	{
		if (c->dockPanel->flags & mgDockPanelFlag_onSplitter)
			ok = 0;
	}
	
	if (c->activeTextInput)
	{
		mgUpdateElementLMBClick(c->activeTextInput->element->window->rootElement);

		mgUpdateElement(c->activeTextInput->element);
		return;
	}

	if (cw && ok)
	{
		cw = cw->left;
		
		mgWindow* lw = cw->right;
		while (1)
		{
			if ((cw->flags & mgWindowFlag_canDock) && cw->dockPanelWindow)
				goto skip;

			if (cw->flagsInternal & mgWindowFlag_internal_visible)
			{
				if (cw->flagsInternal & mgWindowFlag_internal_isExpand)
				{
					if (c->needUpdateTransform)
						mgUpdateTransformElement(cw->rootElement);

					if (c->needRebuild)
						mgRebuildElement(cw->rootElement);
				}

				if (!c->windowUnderCursor)
				{
					mgUpdateWindow(cw);

					if (!c->activePopup)
					{
						if (cw->flagsInternal & mgWindowFlag_internal_isExpand)
						{
							mgUpdateElementLMBClick(cw->rootElement);
							mgUpdateElement(cw->rootElement);
						}
					}
				}

				
			}

		skip:;
			if (cw == lw)
				break;
			cw = cw->left;
		}
		c->needUpdateTransform = 0;
		c->needRebuild = 0;
	}

	if (c->dockPanel)
	{
		if (!c->windowUnderCursor && !c->popupUnderCursor)
			mgDockPanelUpdate(c);

		if (c->activeMenu && !c->activePopup)
		{
			if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
				|| (c->input->mouseButtonFlags1 & MG_MBFL_RMBUP))
			{
				c->activeMenu->activeItem = 0;
				c->activeMenu = 0;
			}
		}
	}
}

MG_API
void MG_C_DECL
mgStartFrame_f(mgContext* c)
{
	assert(c);
	g_skipFrame = 0;
	
	c->input->mouseMoveDeltaOld = c->input->mouseMoveDelta;
	c->input->mouseWheelDeltaOld = c->input->mouseWheelDelta;
	c->input->mousePositionOld = c->input->mousePosition;
	c->input->mouseButtonFlags1 = 0;
	c->input->mouseWheelDelta = 0;
	c->input->character = 0;

	if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN) == MG_MBFL_LMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_LMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN) == MG_MBFL_RMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_RMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN) == MG_MBFL_MMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_MMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X1MBDOWN) == MG_MBFL_X1MBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X1MBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X2MBDOWN) == MG_MBFL_X2MBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X2MBDOWN;

	if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBUP) == MG_MBFL_LMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_LMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_RMBUP) == MG_MBFL_RMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_RMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_MMBUP) == MG_MBFL_MMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_MMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X1MBUP) == MG_MBFL_X1MBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X1MBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X2MBUP) == MG_MBFL_X2MBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X2MBUP;

	for (int i = 0; i < 256; ++i)
	{
		if ((c->input->keyFlags[i] & MG_KEYFL_HIT) == MG_KEYFL_HIT)
			c->input->keyFlags[i] ^= MG_KEYFL_HIT;

		if ((c->input->keyFlags[i] & MG_KEYFL_RELEASE) == MG_KEYFL_RELEASE)
			c->input->keyFlags[i] ^= MG_KEYFL_RELEASE;
	}
}


MG_API
void MG_C_DECL
mgSetParent_f(mgElement* object, mgElement* parent)
{
	assert(object);

	if (object->parent)
	{
		uint32_t chCount = 0;
		for (uint32_t i = 0; i < object->parent->childrenCount; ++i)
		{
			if (object->parent->children[i].pointer != object)
				++chCount;
		}
		
		struct mgElementNode_s* newChildren = 0;
		
		if (chCount)
		{
			newChildren = malloc(chCount * sizeof(struct mgElementNode_s));

			uint32_t i2 = 0;
			for (uint32_t i = 0; i < object->parent->childrenCount; ++i)
			{
				if (object->parent->children[i].pointer != object)
				{
					newChildren[i2].pointer = object->parent->children[i].pointer;
					++i2;
				}
			}
		}

		object->parent->childrenCount = chCount;
		if (object->parent->children)
			free(object->parent->children);

		object->parent->children = newChildren;
	}

	object->parent = parent;
	if (!parent)
		object->parent = object->window->rootElement;

	{
		int chCount = object->parent->childrenCount + 1;
		struct mgElementNode_s* newChildren = malloc(chCount * sizeof(struct mgElementNode_s));

		for (uint32_t i = 0; i < object->parent->childrenCount; ++i)
		{
			newChildren[i].pointer = object->parent->children[i].pointer;
		}

		newChildren[object->parent->childrenCount].pointer = object;

		object->parent->childrenCount++;

		if (object->parent->children)
			free(object->parent->children);

		object->parent->children = newChildren;
	}

	object->window->flagsInternal |= mgWindowFlag_internal_updateContentHeight;
}

MG_API
void MG_C_DECL
mgSetVisible_f(mgElement* e, int v)
{
	assert(e);
	e->visible = v;
	for (uint32_t i = 0; i < e->childrenCount; ++i)
	{
		mgSetVisible_f(e->children[i].pointer, v);
	}
}

void 
mgDrawElement(mgElement* e)
{
	if (!e->visible)
		return;

	e->onDraw(e);

	for (uint32_t i = 0; i < e->childrenCount; ++i)
	{
		mgDrawElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgDraw_f(mgContext* c)
{
	assert(c);

	static int drawTooltip = 0;

	if (c->dockPanel)
		mgDrawDockPanel(c);

	mgWindow* cw = c->firstWindow;
	if (cw)
	{
		mgWindow* lw = cw->left;
		while (1)
		{
			if ((cw->flags & mgWindowFlag_canDock) && cw->dockPanelWindow)
				goto skip;

			if (cw->flagsInternal & mgWindowFlag_internal_visible)
				mgDrawWindow_f(cw);

		skip:;
			if (cw == lw)
				break;
			cw = cw->right;
		}
	}

	if (c->activePopup)
	{
		mgDrawPopup_f(c, c->activePopup);
	}

	if(!c->tooltipText)
		drawTooltip = 0;

	if (c->tooltipText && c->tooltipFont)
	{
		static float tttime = 0.f;
		static mgRect cursorRect;
		static mgPoint saveCursorPos;

		tttime += c->deltaTime;
		if (tttime > 0.5f)
		{
			tttime = 0.f;
			if (!drawTooltip)
			{
				drawTooltip = 1;
				cursorRect.left = c->input->mousePosition.x - 10;
				cursorRect.top = c->input->mousePosition.y - 10;
				cursorRect.right = c->input->mousePosition.x + 20;
				cursorRect.bottom = c->input->mousePosition.y + 20;
				saveCursorPos = c->input->mousePosition;
			}
		}
		
		/*c->gpu->drawRectangle(mgDrawRectangleReason_tooltip,
			&cursorRect,
			&c->activeStyle->popupSeparator,
			&c->activeStyle->popupSeparator,
			0, 0, 0);*/

		if (drawTooltip)
		{
			size_t textLen = wcslen(c->tooltipText);
			if (textLen)
			{
				int tooltipIndent = 3;

				mgRect r;
				r.left = saveCursorPos.x;
				r.top = saveCursorPos.y;
				r.bottom = r.top;
				r.right = r.left;

				mgPoint pt;
				c->getTextSize(c->tooltipText, c->tooltipFont, &pt);

				r.left -= tooltipIndent;
				r.top -= tooltipIndent;
				r.right += tooltipIndent + pt.x;
				r.bottom += tooltipIndent + pt.y;


				r.top += 10;
				r.bottom += 10;

				if (r.top < cursorRect.bottom)
				{
					int v = cursorRect.bottom - r.top;
					r.top += v;
					r.bottom += v;
				}

				if (r.bottom > c->windowSize.y)
				{
					int v = (r.bottom - c->windowSize.y) + 20;
					r.top -= v;
					r.bottom -= v;
				}

				c->gpu->setClipRect(&r);
				c->gpu->drawRectangle(mgDrawRectangleReason_tooltip,
					0,
					&r,
					&c->activeStyle->tooltipBG,
					&c->activeStyle->tooltipBG,
					0, 0);

				pt.x = r.left + tooltipIndent;
				pt.y = r.top + tooltipIndent;

				c->gpu->drawText(mgDrawTextReason_tooltip,
					0,
					&pt,
					c->tooltipText,
					(int)textLen,
					&c->activeStyle->tooltipText,
					c->tooltipFont);
			}
		}

		if (c->input->mouseMoveDelta.x || c->input->mouseMoveDelta.y)
		{
			if (!mgPointInRect(&cursorRect, &c->input->mousePosition))
			{
				drawTooltip = 0;
				tttime = 0.f;
				mgRectSet(&cursorRect, 0, 0, 0, 0);
				mgPointSet(&saveCursorPos, 0, 0);
			}
		}
	}
}

MG_API
mgIcons* MG_C_DECL
mgCreateIcons_f(mgTexture* t, int textureSizeX, int textureSizeY, int iconNum)
{
	assert(t);
	assert(textureSizeX > 0);
	assert(textureSizeY > 0);
	assert(iconNum > 0);
	mgIcons* newIc = calloc(1, sizeof(mgIcons));
	newIc->texture = t;
	newIc->textureSize.x = textureSizeX;
	newIc->textureSize.y = textureSizeY;
	newIc->iconNodesSize = iconNum;
	newIc->iconNodes = malloc(iconNum * sizeof(mgIconsNode));
	return newIc;
}

MG_API
void MG_C_DECL
mgDestroyIcons_f(mgIcons* ic)
{
	assert(ic);
	if (ic->iconNodes)
		free(ic->iconNodes);
	free(ic);
}

MG_API
void MG_C_DECL
mgSetIcon_f(mgIcons* ic, int id, int px, int py, int sx, int sy)
{
	sy++;
	assert(ic);
	assert(ic->iconNodesSize);
	assert(id >= 0);
	assert(id < ic->iconNodesSize);
	ic->iconNodes[id].lt.x = px;
	ic->iconNodes[id].lt.y = py;
	ic->iconNodes[id].rb.x = px + sx;
	ic->iconNodes[id].rb.y = py + sy;

	ic->iconNodes[id].sz.x = sx;
	ic->iconNodes[id].sz.y = sy;
	
	float mx = 1.f / ic->textureSize.x;
	float my = 1.f / ic->textureSize.y;
	
	ic->iconNodes[id].uv.x = px * mx;
	ic->iconNodes[id].uv.y = py * my;
	ic->iconNodes[id].uv.z = (px + sx) * mx;
	ic->iconNodes[id].uv.w = (py + sy) * my;
}

MG_API
void MG_C_DECL
mgOnWindowSize_f(struct mgContext_s* c, int x, int y)
{
	assert(c);
	c->needRebuild = 1;
	c->needUpdateTransform = 1;
	c->windowSize.x = x;
	c->windowSize.y = y;

	g_skipFrame = 1;

	mgUpdate_f(c);

	if (c->dockPanel)
		mgDockPanelOnSize(c);
}
