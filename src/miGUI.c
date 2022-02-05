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

void mgInitDefaultCursors(mgContext* c);
void mgDestroyDefaultCursors(mgContext* c);
void mgDrawWindow(struct mgWindow_s* w);
void mgUpdateWindow(struct mgWindow_s* w);
void mgDrawDockPanel(struct mgContext_s* c);
void mgDockPanelOnSize(struct mgContext_s* c);
void mgDockPanelUpdate(struct mgContext_s* c);

void 
mgDestroyElement_f(mgElement* e)
{
	assert(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgDestroyElement_f(e->children[i].pointer);
	}

	if (e->implementation)
		free(e->implementation);
	free(e);
}

MG_API 
mgContext* MG_C_DECL
mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input)
{
	assert(gpu);
	assert(input);

	mgContext* c = calloc(1, sizeof(mgContext));
	c->gpu = gpu;
	c->input = input;
	c->needUpdateTransform = 1;
	c->needRebuild = 1;
	c->activeStyle = &c->styleLight;
	c->deltaTime = 0.f;

	mgColorSetAsIntegerRGB(&c->styleLight.windowBGColor, 0xE1E6F7);
	mgColorSetAsIntegerRGB(&c->styleLight.windowTitlebarColor, 0xC9D6F2);
	mgColorSetAsIntegerRGB(&c->styleLight.windowBGColorTopWindow, 0xE8EDFF);
	mgColorSetAsIntegerRGB(&c->styleLight.windowTitlebarColorTopWindow, 0xB5CCFF);
	mgColorSetAsIntegerRGB(&c->styleLight.windowTitlebarTextColor, 0x0);
	mgColorSetAsIntegerRGB(&c->styleLight.dockpanelBGColor, 0xF0F0F0);
	
	c->functions.SetCursor_p = mgSetCursor_f;

	mgInitDefaultCursors(c);

	return c;
}

MG_API 
void MG_C_DECL
mgDestroyContext_f(mgContext* c)
{
	assert(c);

	mgDestroyDefaultCursors(c);

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
		if (c->dockPanel->elements)
			free(c->dockPanel->elements);
		free(c->dockPanel);
	}

	free(c);
}

void
mgUpdateElement(mgElement* e) 
{
	e->onUpdate(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgUpdateElement(e->children[i].pointer);
	}
}

void
mgUpdateTransformElement(mgElement* e)
{
	if (!e->visible)
		return;

	e->onUpdateTransform(e);
	for (int i = 0; i < e->childrenCount; ++i)
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
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgRebuildElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgUpdate_f(mgContext* c)
{
	assert(c);

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

	c->windowUnderCursor = 0;

	mgWindow* cw = c->firstWindow;
	if (cw && !(c->dockPanel->flags & mgDockPanelFlag_onSplitter))
	{
		cw = cw->left;
		
		mgWindow* lw = cw->right;
		while (1)
		{
			if (cw->flags & mgWindowFlag_internal_visible)
			{
				if (!c->windowUnderCursor)
				{
					mgUpdateWindow(cw);

					if (cw->flags & mgWindowFlag_internal_isExpand)
					{
						mgUpdateElement(cw->rootElement);

						if (c->needUpdateTransform)
						{
							mgUpdateTransformElement(cw->rootElement);
						}

						if (c->needRebuild)
						{
							mgRebuildElement(cw->rootElement);
							c->needRebuild = 0;
						}
					}
				}
			}

			if (cw == lw)
				break;
			cw = cw->left;
		}
		c->needUpdateTransform = 0;
	}

	if (c->dockPanel && !c->windowUnderCursor)
		mgDockPanelUpdate(c);
}

MG_API
void MG_C_DECL
mgStartFrame_f(mgContext* c)
{
	assert(c);
	
	c->input->mouseMoveDeltaOld = c->input->mouseMoveDelta;
	c->input->mouseWheelDeltaOld = c->input->mouseWheelDelta;
	c->input->mousePositionOld = c->input->mousePosition;
	c->input->mouseButtonFlags1 = 0;

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
		int chCount = 0;
		for (int i = 0; i < object->parent->childrenCount; ++i)
		{
			if (object->parent->children[i].pointer != object)
				++chCount;
		}
		
		struct mgElementNode_s* newChildren = 0;
		
		if (chCount)
		{
			newChildren = malloc(chCount * sizeof(struct mgElementNode_s));

			int i2 = 0;
			for (int i = 0; i < object->parent->childrenCount; ++i)
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

		for (int i = 0; i < object->parent->childrenCount; ++i)
		{
			newChildren[i].pointer = object->parent->children[i].pointer;
		}

		newChildren[object->parent->childrenCount].pointer = object;

		object->parent->childrenCount++;

		if (object->parent->children)
			free(object->parent->children);

		object->parent->children = newChildren;
	}
}

MG_API
void MG_C_DECL
mgSetVisible_f(mgElement* e, int v)
{
	assert(e);
	e->visible = v;
	for (int i = 0; i < e->childrenCount; ++i)
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

	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgDrawElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgDraw_f(mgContext* c)
{
	assert(c);

	if (c->dockPanel)
		mgDrawDockPanel(c);

	mgWindow* cw = c->firstWindow;
	if (cw)
	{
		mgWindow* lw = cw->left;
		while (1)
		{
			if (cw->flags & mgWindowFlag_internal_visible)
				mgDrawWindow(cw);

			if (cw == lw)
				break;
			cw = cw->right;
		}
	}
}

MG_API
mgIcons* MG_C_DECL
mgCreateIcons_f(mgTexture t, int textureSizeX, int textureSizeY, int iconNum)
{
	assert(t);
	assert(textureSizeX > 0);
	assert(textureSizeY > 0);
	assert(iconNum > 0);
	mgIcons* newIc = calloc(1, sizeof(mgIcons));
	newIc->texture = t;
	newIc->textureSize.x = textureSizeX;
	newIc->textureSize.y = textureSizeY;
	newIc->iconsSize = iconNum;
	newIc->icons = malloc(iconNum * sizeof(mgIconsNode));
	return newIc;
}

MG_API
void MG_C_DECL
mgDestroyIcons_f(mgIcons* ic)
{
	assert(ic);
	if (ic->icons)
		free(ic->icons);
	free(ic);
}

MG_API
void MG_C_DECL
mgSetIcon_f(mgIcons* ic, int id, int px, int py, int sx, int sy)
{
	assert(ic);
	assert(ic->iconsSize);
	assert(id >= 0);
	assert(id < ic->iconsSize);
	ic->icons[id].lt.x = px;
	ic->icons[id].lt.y = py;
	ic->icons[id].rb.x = px + sx;
	ic->icons[id].rb.y = py + sy;

	ic->icons[id].sz.x = sx;
	ic->icons[id].sz.y = sy;
	
	float mx = 1.f / ic->textureSize.x;
	float my = 1.f / ic->textureSize.y;
	
	ic->icons[id].uv.x = px * mx;
	ic->icons[id].uv.y = py * my;
	ic->icons[id].uv.z = (px + sx) * mx;
	ic->icons[id].uv.w = (py + sy) * my;
}

MG_API
void MG_C_DECL
mgOnWindowSize_f(struct mgContext_s* c, int x, int y)
{
	assert(c);
	c->needRebuild = 1;
	c->windowSize.x = x;
	c->windowSize.y = y;

	if (c->dockPanel)
		mgDockPanelOnSize(c);
}
