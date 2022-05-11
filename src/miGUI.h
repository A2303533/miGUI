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
typedef struct mgVideoDriverAPI_s {
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

	void(*drawText)( 
		int reason, 
		void* object,  /*depends on reason*/
		mgPoint* position, const wchar_t* text, int textLen, mgColor*, mgFont*);

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


#ifdef MG_NO_DLL
MG_API struct mgContext_s* MG_C_DECL mgCreateContext_f(mgVideoDriverAPI*, mgInputContext*);
#define mgCreateContext mgCreateContext_f
#else
typedef struct mgContext_s* (*PFNMGCREATECONTEXTPROC)(mgVideoDriverAPI*, mgInputContext*);
extern PFNMGCREATECONTEXTPROC mgCreateContext;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyContext_f(struct mgContext_s*);
#define mgDestroyContext mgDestroyContext_f
#else
typedef void (*PFNMGDESTROYCONTEXTPROC)(struct mgContext_s*);
extern PFNMGDESTROYCONTEXTPROC mgDestroyContext;
#endif

#ifdef MG_NO_DLL
MG_API struct mgWindow_s* MG_C_DECL mgCreateWindow_f(struct mgContext_s*, int px, int py, int sx, int sy);
#define mgCreateWindow mgCreateWindow_f
#else
typedef struct mgWindow_s* (*PFNMGCREATEWINDOWPROC)(struct mgContext_s*, int px, int py, int sx, int sy);
extern PFNMGCREATEWINDOWPROC mgCreateWindow;
#endif

#if defined(MG_NO_DLL)
MG_API void MG_C_DECL mgDestroyWindow_f(struct mgWindow_s*);
#define mgDestroyWindow mgDestroyWindow_f
#else
typedef void (*PFNMGDESTROYWINDOWPROC)(struct mgWindow_s*);
extern PFNMGDESTROYWINDOWPROC mgDestroyWindow;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgSetWindowTitle_f(struct mgWindow_s*, const wchar_t*);
#define mgSetWindowTitle mgSetWindowTitle_f
#else
typedef void (*PFNMGSETWINDOWTITLEPROC)(struct mgWindow_s*, const wchar_t*);
extern PFNMGSETWINDOWTITLEPROC mgSetWindowTitle;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgBringWindowToTop_f(struct mgWindow_s*);
#define mgBringWindowToTop mgBringWindowToTop_f
#else
typedef void (*PFNMGBRINGWINDOWTOTOPPROC)(struct mgWindow_s*);
extern PFNMGBRINGWINDOWTOTOPPROC mgBringWindowToTop;
#endif

/*
* 1 show
* 0 hide
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgShowWindow_f(struct mgWindow_s*, int);
#define mgShowWindow mgShowWindow_f
#else
typedef void (*PFNMGSHOWWINDOWPROC)(struct mgWindow_s*, int);
extern PFNMGSHOWWINDOWPROC mgShowWindow;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgOnWindowSize_f(struct mgContext_s*, int x, int y);
#define mgOnWindowSize mgOnWindowSize_f
#else
typedef void (*PFNMGONWINDOWSIZEPROC)(struct mgContext_s*, int x, int y);
extern PFNMGONWINDOWSIZEPROC mgOnWindowSize;
#endif

/* Create bitmap font or load from file.
* If filename exist load from file. If not then try to generate from installed system font.
* Destroy it by yourself (call mgDestroyFont).
* saveIt (optional) - save .txt and images dds rgba in ../data/fonts/$saveIt/
*/
#ifdef MG_NO_DLL
MG_API mgFont* MG_C_DECL mgCreateFont_f(struct mgContext_s*, const char* filename, unsigned int flags, int size, const char* saveIt);
#define mgCreateFont mgCreateFont_f
#else
typedef mgFont* (*PFNMGCREATEFONTPROC)(struct mgContext_s*, const char* filename, unsigned int flags, int size, const char* saveIt);
extern PFNMGCREATEFONTPROC mgCreateFont;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyFont_f(struct mgContext_s*, mgFont*);
#define mgDestroyFont mgDestroyFont_f
#else
typedef void (*PFNMGDESTROYFONTPROC)(struct mgContext_s*, mgFont*);
extern PFNMGDESTROYFONTPROC mgDestroyFont;
#endif

/*call before event loop*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgStartFrame_f(struct mgContext_s*);
#define mgStartFrame mgStartFrame_f
#else
typedef void (*PFNMGSTARTFRAMEPROC)(struct mgContext_s*);
extern PFNMGSTARTFRAMEPROC mgStartFrame;
#endif

/*do work*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgUpdate_f(struct mgContext_s*);
#define mgUpdate mgUpdate_f
#else
typedef void (*PFNMGUPDATEPROC)(struct mgContext_s*);
extern PFNMGUPDATEPROC mgUpdate;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgSetParent_f(mgElement* object, mgElement* parent);
#define mgSetParent mgSetParent_f
#else
typedef void (*PFNMGSETPARENTPROC)(mgElement* object, mgElement* parent);
extern PFNMGSETPARENTPROC mgSetParent;
#endif

#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateRectangle_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2);
#define mgCreateRectangle mgCreateRectangle_f
#else
typedef mgElement* (*PFNMGCREATERECTANGLEPROC)(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2);
extern PFNMGCREATERECTANGLEPROC mgCreateRectangle;
#endif

#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateText_f(struct mgWindow_s* w, mgPoint* position, const wchar_t* text, mgFont* font);
#define mgCreateText mgCreateText_f
#else
typedef mgElement* (*PFNMGCREATETEXTPROC)(struct mgWindow_s* w, mgPoint* position, const wchar_t* text, mgFont* font);
extern PFNMGCREATETEXTPROC mgCreateText;
#endif

#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateButton_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font);
#define mgCreateButton mgCreateButton_f
#else
typedef mgElement* (*PFNMGCREATEBUTTONPROC)(struct mgWindow_s* w, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font);
extern PFNMGCREATEBUTTONPROC mgCreateButton;
#endif

/*
* arr and arrSz - can be 0, set this later
*/
#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateListBox_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** items, uint32_t itemsSize, mgFont* f);
#define mgCreateListBox mgCreateListBox_f
#else
typedef mgElement* (*PFNMGCREATELISTBOXPROC)(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** items, uint32_t itemsSize, mgFont* f);
extern PFNMGCREATELISTBOXPROC mgCreateListBox;
#endif

#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateTable_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** rows, uint32_t rowsSz, uint32_t colNum, mgFont* f);
#define mgCreateTable mgCreateTable_f
#else
typedef mgElement* (*PFNMGCREATETABLEPROC)(struct mgWindow_s* w, mgPoint* position, mgPoint* size, void** rows, uint32_t rowsSz, uint32_t colNum, mgFont* f);
extern PFNMGCREATETABLEPROC mgCreateTable;
#endif

/*
* Will destroy element and children.
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyElement_f(mgElement*);
#define mgDestroyElement mgDestroyElement_f
#else
typedef void (*PFNMGDESTROYELEMENTPROC)(mgElement*);
extern PFNMGDESTROYELEMENTPROC mgDestroyElement;
#endif

/*set visible include all children*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgSetVisible_f(mgElement*, int);
#define mgSetVisible mgSetVisible_f
#else
typedef void (*PFNMGSETVISIBLEPROC)(mgElement*, int);
extern PFNMGSETVISIBLEPROC mgSetVisible;
#endif

/*draw all*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDraw_f(struct mgContext_s*);
#define mgDraw mgDraw_f
#else
typedef void (*PFNMGDRAWPROC)(struct mgContext_s*);
extern PFNMGDRAWPROC mgDraw;
#endif

/*don't forget to destroy cursor with mgDestroyCursor*/
#ifdef MG_NO_DLL
MG_API mgCursor* MG_C_DECL mgCreateCursor_f(const wchar_t* fileName);
#define mgCreateCursor mgCreateCursor_f
#else
typedef mgCursor*(*PFNMGCREATECURSORPROC)(const wchar_t* fileName);
extern PFNMGCREATECURSORPROC mgCreateCursor;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyCursor_f(mgCursor*);
#define mgDestroyCursor mgDestroyCursor_f
#else
typedef void (*PFNMGDESTROYCURSORPROC)(mgCursor*);
extern PFNMGDESTROYCURSORPROC mgDestroyCursor;
#endif

/*if cursor == 0 then set default*/
typedef void (*PFNMGSETCURSORPROC)(struct mgContext_s*, mgCursor* cursor, unsigned int type);
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgSetCursor_f(struct mgContext_s*, mgCursor* cursor, unsigned int type);
#define mgSetCursor mgSetCursor_f
#else
extern PFNMGSETCURSORPROC mgSetCursor;
#endif

#ifdef MG_NO_DLL
MG_API mgIcons* MG_C_DECL mgCreateIcons_f(mgTexture*, int textureSizeX, int textureSizeY, int iconNum);
#define mgCreateIcons mgCreateIcons_f
#else
typedef mgIcons* (*PFNMGCREATEICONSPROC)(mgTexture*, int textureSizeX, int textureSizeY, int iconNum);
extern PFNMGCREATEICONSPROC mgCreateIcons;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyIcons_f(mgIcons*);
#define mgDestroyIcons mgDestroyIcons_f
#else
typedef void (*PFNMGDESTROYICONSPROC)(mgIcons*);
extern PFNMGDESTROYICONSPROC mgDestroyIcons;
#endif

/*
* px, py - left top corner of rect
* sx, sy - size
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgSetIcon_f(mgIcons*, int id, int px, int py, int sx, int sy);
#define mgSetIcon mgSetIcon_f
#else
typedef void (*PFNMGSETICONPROC)(mgIcons*, int id, int px, int py, int sx, int sy);
extern PFNMGSETICONPROC mgSetIcon;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgInitDockPanel_f(struct mgContext_s*, int indentLeft, int indentTop, int indentRight, int indentBottom, mgDockPanelElementCreationInfo* elements, int elementsSize);
#define mgInitDockPanel mgInitDockPanel_f
#else
typedef void (*PFNMGINITDOCKPANELPROC)(struct mgContext_s*, int indentLeft, int indentTop, int indentRight, int indentBottom, mgDockPanelElementCreationInfo* elements, int elementsSize);
extern PFNMGINITDOCKPANELPROC mgInitDockPanel;
#endif

/*
* if dw 0 then id is id of mgDockPanelElement_s
* if dw not 0 then id is where (0 - left, 1 - top, 2 - right, 3 - bottom)
*/
#ifdef MG_NO_DLL
MG_API struct mgDockPanelWindow_s* MG_C_DECL mgDockAddWindow_f(struct mgWindow_s*, struct mgDockPanelWindow_s* dw, int id);
#define mgDockAddWindow mgDockAddWindow_f
#else
typedef struct mgDockPanelWindow_s* (*PFNMGDOCKADDWINDOWPROC)(struct mgWindow_s*, struct mgDockPanelWindow_s* dw, int id);
extern PFNMGDOCKADDWINDOWPROC mgDockAddWindow;
#endif

#ifdef MG_NO_DLL
MG_API struct mgPopup_s* MG_C_DECL mgCreatePopup_f(struct mgContext_s* c, struct mgPopupItemInfo_s* arr, int arrSize, mgFont*, int flags);
#define mgCreatePopup mgCreatePopup_f
#else
typedef struct mgPopup_s* (*PFNMGCREATEPOPUPPROC)(struct mgContext_s* c, struct mgPopupItemInfo_s* arr, int arrSize, mgFont*, int flags);
extern PFNMGCREATEPOPUPPROC mgCreatePopup;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyPopup_f(struct mgPopup_s*);
#define mgDestroyPopup mgDestroyPopup_f
#else
typedef void (*PFNMGDESTROYPOPUPPROC)(struct mgPopup_s*);
extern PFNMGDESTROYPOPUPPROC mgDestroyPopup;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgShowPopup_f(struct mgContext_s*, struct mgPopup_s*, mgPoint* position);
#define mgShowPopup mgShowPopup_f
#else
typedef void (*PFNMGSHOWPOPUPPROC)(struct mgContext_s*, struct mgPopup_s*, mgPoint* position);
extern PFNMGSHOWPOPUPPROC mgShowPopup;
#endif

/*return 0 if bad*/
#ifdef MG_NO_DLL
MG_API int MG_C_DECL mgInitDefaultIcons_f(struct mgContext_s*, mgTexture*);
#define mgInitDefaultIcons mgInitDefaultIcons_f
#else
typedef int (*PFNMGINITDEFAULTICONSPROC)(struct mgContext_s*, mgTexture*);
extern PFNMGINITDEFAULTICONSPROC mgInitDefaultIcons;
#endif

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
#ifdef MG_NO_DLL
MG_API int* MG_C_DECL mgDockGetSaveData_f(struct mgContext_s*, int* dataSize_out);
#define mgDockGetSaveData mgDockGetSaveData_f
#else
typedef int* (*PFNMGDOCKGETSAVEDATAPROC)(struct mgContext_s*, int* dataSize_out);
extern PFNMGDOCKGETSAVEDATAPROC mgDockGetSaveData;
#endif

/*
* callback must return window with same id
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDockLoadData_f(struct mgContext_s*, int* data, int dataSizeInInt, mgWindow* (*callback)(int windowID));
#define mgDockLoadData mgDockLoadData_f
#else
typedef void (*PFNMGDOCKLOADDATAPROC)(struct mgContext_s*, int* data, int dataSizeInInt, mgWindow*(*callback)(int windowID));
extern PFNMGDOCKLOADDATAPROC mgDockLoadData;
#endif

/*
* mgMenu works with mgWindow
* You must create popups by yourself, and destroy them too.
*/
#ifdef MG_NO_DLL
MG_API mgMenu* MG_C_DECL mgCreateMenu_f(struct mgContext_s*, mgMenuItemInfo* items, int itemsSize, mgFont* f);
#define mgCreateMenu mgCreateMenu_f
#else
typedef mgMenu*(*PFNMGCREATEMENUPROC)(struct mgContext_s*, mgMenuItemInfo* items, int itemsSize, mgFont* f);
extern PFNMGCREATEMENUPROC mgCreateMenu;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDestroyMenu_f(mgMenu*);
#define mgDestroyMenu mgDestroyMenu_f
#else
typedef void (*PFNMGDESTROYMENUPROC)(mgMenu*);
extern PFNMGDESTROYMENUPROC mgDestroyMenu;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgInitStyleLight_f(mgStyle*);
#define mgInitStyleLight mgInitStyleLight_f
#else
typedef void (*PFNMGINITSTYLELIGHTPROC)(mgStyle*);
extern PFNMGINITSTYLELIGHTPROC mgInitStyleLight;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgInitStyleDark_f(mgStyle*);
#define mgInitStyleDark mgInitStyleDark_f
#else
typedef void (*PFNMGINITSTYLEDARKPROC)(mgStyle*);
extern PFNMGINITSTYLEDARKPROC mgInitStyleDark;
#endif

#ifdef MG_NO_DLL
MG_API mgElement* MG_C_DECL mgCreateTextInput_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgFont* font);
#define mgCreateTextInput mgCreateTextInput_f
#else
typedef mgElement* (*PFNMGCREATETEXTINPUTPROC)(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgFont* font);
extern PFNMGCREATETEXTINPUTPROC mgCreateTextInput;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgTextInputSetText_f(struct mgElementTextInput_s* e, const wchar_t* text);
#define mgTextInputSetText mgTextInputSetText_f
#else
typedef void (*PFNMGTEXTINPUTSETTEXTPROC)(struct mgElementTextInput_s* e, const wchar_t* text);
extern PFNMGTEXTINPUTSETTEXTPROC mgTextInputSetText;
#endif

/*
* freeMemory: 1 for free
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgTextInputClear_f(struct mgElementTextInput_s* e, int freeMemory);
#define mgTextInputClear mgTextInputClear_f
#else
typedef void (*PFNMGTEXTINPUTCLEARPROC)(struct mgElementTextInput_s* e, int freeMemory);
extern PFNMGTEXTINPUTCLEARPROC mgTextInputClear;
#endif

/*return width in pixels (max width)*/
#ifdef MG_NO_DLL
MG_API int MG_C_DECL mgTextInputPutText_f(struct mgElementTextInput_s* e, const wchar_t* text, uint32_t len);
#define mgTextInputPutText mgTextInputPutText_f
#else
typedef int (*PFNMGTEXTINPUTPUTTEXTPROC)(struct mgElementTextInput_s* e, const wchar_t* text, uint32_t len);
extern PFNMGTEXTINPUTPUTTEXTPROC mgTextInputPutText;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgTextInputDeleteSelected_f(struct mgElementTextInput_s* e);
#define mgTextInputDeleteSelected mgTextInputDeleteSelected_f
#else
typedef void (*PFNMGTEXTINPUTDELETESELECTEDPROC)(struct mgElementTextInput_s* e);
extern PFNMGTEXTINPUTDELETESELECTEDPROC mgTextInputDeleteSelected;
#endif

/*
* deactivateCode is optional
*/
#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgTextInputActivate_f(struct mgContext_s*, struct mgElementTextInput_s* e, int isActive, int deactivateCode);
#define mgTextInputActivate mgTextInputActivate_f
#else
typedef void (*PFNMGTEXTINPUTACTIVATEPROC)(struct mgContext_s*, struct mgElementTextInput_s* e, int isActive, int deactivateCode);
extern PFNMGTEXTINPUTACTIVATEPROC mgTextInputActivate;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDrawWindow_f(struct mgWindow_s* w);
#define mgDrawWindow mgDrawWindow_f
#else
typedef void (*PFNMGDRAWWINDOWPROC)(struct mgWindow_s*);
extern PFNMGDRAWWINDOWPROC mgDrawWindow;
#endif

#ifdef MG_NO_DLL
MG_API void MG_C_DECL mgDrawPopup_f(struct mgContext_s* c, mgPopup* p);
#define mgDrawPopup mgDrawPopup_f
#else
typedef void (*PFNMGDRAWPOPUPPROC)(struct mgContext_s* c, mgPopup* p);
extern PFNMGDRAWPOPUPPROC mgDrawPopup;
#endif

#if defined(__cplusplus)
}
#endif

/*
* Idea is: you load migui.dll in your exe,
*   and then you can use functions in your dlls.
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
	void(*getTextSize)(const wchar_t* text, mgFont*, mgPoint*);

	// mgPopup with systemWindow flag will create new window.
	// you can/backends will use this for some initialization
	//void(*onCreateWindow)(struct mgSystemWindowOSData*);

	struct mgFunctions_s functions;

	struct mgDockPanel_s* dockPanel;

	mgColor whiteColor;
	//mgRect currentIcon;
	mgIconsNode* currentIcon;

	mgStyle styleLight;
	mgStyle styleDark;
	mgStyle* activeStyle;

	mgPopup* activePopup;
	mgPopup* popupUnderCursor;
	mgPopup* defaultPopupForTextInput;
	mgMenu* activeMenu;
	mgElementTextInput* activeTextInput;

	mgFont* defaultPopupFont;
	int cursorInPopup;
	
	int cursorInElement; // 1 if cursor above element like button, menu item, but not in popup or window

	mgIconGroup* defaultIconGroup;
	
	mgFont* tooltipFont;
	const wchar_t* tooltipText;


} mgContext;

#endif