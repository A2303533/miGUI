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

#ifndef _MI_GUI_H_
#define _MI_GUI_H_

struct mgContext_s;

#include "mgDefs.h"
#include "mgForward.h"
#include "mgPoint.h"
#include "mgRect.h"
#include "mgVec4.h"
#include "mgColor.h"
#include "mgUnicode.h"
#include "mgFont.h"
#include "mgImage.h"
#include "mgTexture.h"

#include "mgIcons.h"
#include "mgStyle.h"
#include "mgPopup.h"
#include "mgMenu.h"
#include "mgDockPanel.h"
#include "mgWindow.h"
#include "mgElement.h"

#include "mgSystemWindowData.h"


// Before creating GUI context you must create this objects.
// VideoDriverAPI - callbacks for drawing.
// InputContext - information about pressed buttons, cursor position and other
typedef struct mgVideoDriverAPI_s 
{
	// Create GPU texture using RGBA data.
	// Library can generate fonts, fonts is a textures.
	mgTexture*(*createTexture)(mgImage*);

	// Destroy texture.For fonts.
	void(*destroyTexture)(mgTexture*);

	struct mgSystemWindowOSData* (*setCurrentWindow)(struct mgSystemWindowOSData*);
	// update buffer for current window
	void (*updateBackBuffer)();

	void(*beginDraw)(int reason); // prepare for drawing, set shaders and other things
	void(*endDraw)();

	void(*drawRectangle)(
		int reason,
		void* object,  // depends on reason
		mgRect* rect,
		mgColor* color1, 
		mgColor* color2, 
		mgTexture* texture, // optional
		mgVec4* UVRegion); // optional

	int(*drawText)( 
		int reason, 
		mgPoint* position, 
		const mgUnicodeChar* text, 
		size_t textLen,
		mgColor*,
		mgFont*);

	void(*drawLine)(
		int reason,
		void* object,  /*depends on reason*/
		mgPoint* position, 
		mgPoint* where,
		mgColor*,
		int size);

	/*set new and return old clip rect*/
	mgRect(*setClipRect)(mgRect*);

} mgVideoDriverAPI;

#include "mgInputContex.h"
#include "mgCursor.h"

struct mgContext_s;

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>
/// Force update elements in this window.
/// </summary>
void MG_C_DECL mgUpdateTransform(struct mgWindow_s* w);

/// <summary>
/// Allocate memory and set some data.
/// </summary>
struct mgContext_s* MG_C_DECL mgCreateContext(mgVideoDriverAPI*, mgInputContext*);
void MG_C_DECL mgDestroyContext(struct mgContext_s*);

/// <summary>
/// Create GUI window
/// </summary>
struct mgWindow_s* MG_C_DECL mgCreateWindow(struct mgContext_s*, int px, int py, int sx, int sy, struct mgTextProcessor_s*);
void MG_C_DECL mgDestroyWindow(struct mgWindow_s*);
void MG_C_DECL mgSetWindowTitle(struct mgWindow_s*, const mgUnicodeChar*);
void MG_C_DECL mgBringWindowToTop(struct mgWindow_s*);
/*
* 1 show
* 0 hide
*/
void MG_C_DECL mgShowWindow(struct mgWindow_s*, int);

/// <summary>
/// System will update all elements in all windows.
/// </summary>
void MG_C_DECL mgOnWindowSize(struct mgContext_s*, int x, int y);

/// <summary>
/// Create bitmap font or load from file.
/// If filename exist load from file.If not then try to generate from installed system font.
/// Destroy it by yourself(call mgDestroyFont).
/// saveIt(optional) - save.txt and images dds rgba in .. / data / fonts / $saveIt /
/// 
/// If backend is GDI then probably create system font.
/// </summary>
mgFont* MG_C_DECL mgCreateFont(struct mgContext_s*, const char* filename, unsigned int flags, int size, const char* saveIt);
void MG_C_DECL mgDestroyFont(struct mgContext_s*, mgFont*);

/*call before event loop*/
void MG_C_DECL mgStartFrame(struct mgContext_s*);

/*do work*/
void MG_C_DECL mgUpdate(struct mgContext_s*);

void MG_C_DECL mgSetParent(mgElement* object, mgElement* parent);

mgElement* MG_C_DECL mgCreateRectangle(struct mgWindow_s* w, mgPoint* position, mgPoint* size);
mgElement* MG_C_DECL mgCreateText(struct mgWindow_s* w, mgPoint* position, struct mgTextProcessor_s*);
mgElement* MG_C_DECL mgCreateButton(struct mgWindow_s* w, mgPoint* position, mgPoint* size, struct mgTextProcessor_s*);
mgElement* MG_C_DECL mgCreateCheckbox(struct mgWindow_s* w, mgPoint* position, mgPoint* size, struct mgTextProcessor_s*);

// arr and arrSz - can be 0, set this later
mgElement* MG_C_DECL mgCreateListBox(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** items, uint32_t itemsSize, struct mgTextProcessor_s*);

mgElement* MG_C_DECL mgCreateTable(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** rows, uint32_t rowsSz, uint32_t colNum, struct mgTextProcessor_s*);


// Will destroy element and children.
void MG_C_DECL mgDestroyElement(mgElement*);

//*set visible include all children*/
void MG_C_DECL mgSetVisible(mgElement*, int);

//*draw all*/
void MG_C_DECL mgDraw(struct mgContext_s*);

//*don't forget to destroy cursor with mgDestroyCursor*/
mgCursor* MG_C_DECL mgCreateCursor(const wchar_t* fileName);
void MG_C_DECL mgDestroyCursor(mgCursor*);

/*if cursor == 0 then set default*/
typedef void (*PFNMGSETCURSORPROC)(struct mgContext_s*, mgCursor* cursor, unsigned int type);
void MG_C_DECL mgSetCursor(struct mgContext_s*, mgCursor* cursor, unsigned int type);

mgIcons* MG_C_DECL mgCreateIcons(mgTexture*, int textureSizeX, int textureSizeY, int iconNum);
void MG_C_DECL mgDestroyIcons(mgIcons*);

// px, py - left top corner of rect
// sx, sy - size
void MG_C_DECL mgSetIcon(mgIcons*, int id, int px, int py, int sx, int sy);
void MG_C_DECL mgInitDockPanel(struct mgContext_s*, int indentLeft, int indentTop, int indentRight, int indentBottom, mgDockPanelElementCreationInfo* elements, int elementsSize, struct mgTextProcessor_s*);

//* if dw 0 then id is id of mgDockPanelElement_s
//* if dw not 0 then id is where (0 - left, 1 - top, 2 - right, 3 - bottom)
struct mgDockPanelWindow_s* MG_C_DECL mgDockAddWindow(struct mgWindow_s*, struct mgDockPanelWindow_s* dw, int id);

struct mgPopup_s* MG_C_DECL mgCreatePopup(struct mgContext_s* c, struct mgPopupItemInfo_s* arr, int arrSize, int flags, struct mgTextProcessor_s*);
void MG_C_DECL mgDestroyPopup(struct mgPopup_s*);
void MG_C_DECL mgShowPopup(struct mgContext_s*, struct mgPopup_s*, mgPoint* position);

//*return 0 if bad*/
int MG_C_DECL mgInitDefaultIcons(struct mgContext_s*, mgTexture*);

/*return int* , use free().
* Everything is in int
*	element - 0x1
*       size
*       panelWindow - 0x2
*			parentIndex
*			where
*			size
*			window - 0x3
*				windowID
* *			window - 0x3
*				windowID
* 
* Every mgWindow in dockpanel must have unique ID.
* dataSize_out in int, use `* sizeof(int)`
*/
int* MG_C_DECL mgDockGetSaveData(struct mgContext_s*, int* dataSize_out);

//* callback must return window with same id
void MG_C_DECL mgDockLoadData(struct mgContext_s*, int* data, int dataSizeInInt, mgWindow* (*callback)(int windowID));

//* mgMenu works with mgWindow
//* You must create popups by yourself, and destroy them too.
mgMenu* MG_C_DECL mgCreateMenu(struct mgContext_s*, mgMenuItemInfo* items, int itemsSize, struct mgTextProcessor_s*);

void MG_C_DECL mgDestroyMenu(mgMenu*);
void MG_C_DECL mgInitStyleLight(mgStyle*);
void MG_C_DECL mgInitStyleDark(mgStyle*);

mgElement* MG_C_DECL mgCreateTextInput(struct mgWindow_s* w, mgPoint* position, mgPoint* size, struct mgTextProcessor_s*);
void MG_C_DECL mgTextInputSetText(struct mgElementTextInput_s* e, const mgUnicodeChar* text);

//* freeMemory: 1 for free
void MG_C_DECL mgTextInputClear(struct mgElementTextInput_s* e, int freeMemory);

//*return width in pixels (max width)*/
int MG_C_DECL mgTextInputPutText(struct mgElementTextInput_s* e, const mgUnicodeChar* text, size_t len);
void MG_C_DECL mgTextInputDeleteSelected(struct mgElementTextInput_s* e);

//* deactivateCode is optional
void MG_C_DECL mgTextInputActivate(struct mgContext_s*, struct mgElementTextInput_s* e, int isActive, int deactivateCode);

void MG_C_DECL mgDrawWindow(struct mgWindow_s* w);
void MG_C_DECL mgDrawPopup(struct mgContext_s* c, mgPopup* p);

#if defined(__cplusplus)
}
#endif

/*
* Idea is: you can use functions in your dlls.
*/
struct mgFunctions_s {
	PFNMGSETCURSORPROC SetCursor_p;
};

#define mgClickedElementsSize 2

typedef struct mgContext_s {
	mgVideoDriverAPI* gpu;
	mgInputContext* input;
	int needUpdateTransform;
	int needRebuild;/*WHY I NEED THIS? needUpdateTransform is enough*/

	float deltaTime;
	mgPoint windowSize;/*system window size*/

	mgWindow* firstWindow;
	mgWindow* windowUnderCursor;

	struct mgSystemWindowOSData systemWindowData;

	mgCursor* defaultCursors[mgCursorType__count];
	mgCursor* currentCursors[mgCursorType__count];

	// Get text size in pixels
	// need to know text size.
	//void(*getTextSize)(const mgUnicodeChar* text, mgTextCallbackFont, mgPoint*);

	// mgPopup with systemWindow flag will create new window.
	// you can/backends will use this for some initialization
	//void(*onCreateWindow)(struct mgSystemWindowOSData*);

	struct mgFunctions_s functions;

	struct mgDockPanel_s* dockPanel;

	mgColor whiteColor;
	//mgRect currentIcon;
	mgIconsNode* currentIcon;

	struct mgStyle_s styleLight;
	struct mgStyle_s styleDark;
	struct mgStyle_s* activeStyle;

	mgPopup* activePopup;
	mgPopup* popupUnderCursor;
	mgPopup* defaultPopupForTextInput;
	mgMenu* activeMenu;
	mgElementTextInput* textInputActive;
	mgElementTextInput* textInputUnderCursor;

//	mgFont* defaultPopupFont;
	int cursorInPopup;
	
	int cursorInElement; // 1 if cursor above element like button, menu item, but not in popup or window

	mgIconGroup* defaultIconGroup;
	
	//mgFont* tooltipFont;
	const mgUnicodeChar* tooltipText;
	struct mgTextProcessor_s* tooltipTextProcessor;

} mgContext;

#endif