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
#ifndef _MG_ELEMENT_H_
#define _MG_ELEMENT_H_

enum mgAlignment
{
	mgAlignment_leftTop,
	mgAlignment_top,
	mgAlignment_rightTop,
	mgAlignment_right,
	mgAlignment_rightBottom,
	mgAlignment_bottom,
	mgAlignment_leftBottom,
	mgAlignment_left,
	mgAlignment_center
};

enum {
	MG_TYPE_RECTANGLE = 1,
	MG_TYPE_TEXT,
	MG_TYPE_BUTTON,
	MG_TYPE_TEXTINPUT,
	MG_TYPE_LIST,
	MG_TYPE_TABLE,
	MG_TYPE_CHECKBOX,
};

struct mgElement_s;

/*I don't want to use manystars** magic*/
struct mgElementNode_s {
	struct mgElement_s* pointer;
};

struct mgElementTransform_s {
	mgRect buildArea;
	mgRect clipArea;
	mgPoint sz; /*can change*/
};

/* base data for all GUI widgets*/
typedef struct mgElement_s {
	uint32_t type; /*MG_TYPE...*/
	void* implementation;

	struct mgWindow_s* window;

	uint32_t align; /*mgAlignment*/

	int id;
	void* userData;
	void* userStyle;

	uint32_t visible;/* != 0 - visible*/
	uint32_t enabled;/*1*/
	uint32_t drawBG;/*1*/
	
	struct mgElement_s* parent;
	struct mgElementNode_s* children;
	uint32_t childrenCount;

	mgRect creationRect;
	struct mgElementTransform_s transformLocal;
	struct mgElementTransform_s transformWorld; /*transformLocal + parent->transformLocal*/

	/*internal*/
	void(*onDraw)(struct mgElement_s* e);
	void(*onUpdate)(struct mgElement_s* e);
	void(*onUpdateTransform)(struct mgElement_s* e);
	void(*onRebuild)(struct mgElement_s* e);/*must call when change window size*/

	/*----*/
	void(*onMouseEnter)(struct mgElement_s* e);
	void(*onMouseLeave)(struct mgElement_s* e);	
	void(*onClickLMB)(struct mgElement_s* e);
	void(*onClickRMB)(struct mgElement_s* e);
	void(*onClickMMB)(struct mgElement_s* e);
	void(*onClickX1MB)(struct mgElement_s* e);
	void(*onClickX2MB)(struct mgElement_s* e);
	void(*onReleaseLMB)(struct mgElement_s* e);
	void(*onReleaseRMB)(struct mgElement_s* e);
	void(*onReleaseMMB)(struct mgElement_s* e);
	void(*onReleaseX1MB)(struct mgElement_s* e);
	void(*onReleaseX2MB)(struct mgElement_s* e);
	
	/* 
	*  0x1 when cursor in buildAreaFinal
	*  0x2 LMB click
	*  0x4 RMB click
	*  0x8 MMB click
	*  0x10 X1MB click
	*  0x20 X2MB click
	*/
	uint32_t elementState;

	float scrollValue;
	float scrollValueTarget;
	float scrollValueWorld;

	uint32_t contentHeight; /*everything inside element. */
	uint32_t clientHeight;  /*rect.bottom - rect.top*/

	uint32_t cursorInRect; /*1\0*/

	uint32_t lmbClickCount;
	float lmb2ClickTimer;
	float lmb2ClickTimerLimit;/*0.3f*/
} mgElement;

inline
void mgElementDefaultInit(mgElement* e)
{
	e->id = -1;
	e->visible = 1;
	e->enabled = 1;
	e->drawBG = 1;
	e->lmb2ClickTimerLimit = 0.3f;
}

#include "mgElementRectangle.h"
#include "mgElementText.h"
#include "mgElementButton.h"
#include "mgElementTextInput.h"
#include "mgElementList.h"
#include "mgElementTable.h"
#include "mgElementCheckbox.h"

#endif