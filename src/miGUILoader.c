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
#include "miGUILoader.h"
#include <assert.h>

PFNMGCREATECONTEXTPROC mgCreateContext;
PFNMGDESTROYCONTEXTPROC mgDestroyContext;
PFNMGCREATEFONTPROC mgCreateFont;
PFNMGDESTROYFONTPROC mgDestroyFont;
PFNMGSTARTFRAMEPROC mgStartFrame;
PFNMGUPDATEPROC mgUpdate;
PFNMGSETPARENTPROC mgSetParent;
PFNMGSETVISIBLEPROC mgSetVisible;
PFNMGDRAWPROC mgDraw;
PFNMGCREATERECTANGLEPROC mgCreateRectangle;
PFNMGCREATETEXTPROC mgCreateText;
PFNMGCREATEBUTTONPROC mgCreateButton;
PFNMGCREATEWINDOWPROC mgCreateWindow;
PFNMGDESTROYWINDOWPROC mgDestroyWindow;
PFNMGSETWINDOWTITLEPROC mgSetWindowTitle;
PFNMGBRINGWINDOWTOTOPPROC mgBringWindowToTop;
PFNMGCREATECURSORPROC mgCreateCursor;
PFNMGDESTROYCURSORPROC mgDestroyCursor;
PFNMGSETCURSORPROC mgSetCursor;
PFNMGCREATEICONSPROC mgCreateIcons;
PFNMGDESTROYICONSPROC mgDestroyIcons;
PFNMGSETICONPROC mgSetIcon;
PFNMGONWINDOWSIZEPROC mgOnWindowSize;
PFNMGINITDOCKPANELPROC mgInitDockPanel;
PFNMGDOCKADDWINDOWPROC mgDockAddWindow;
PFNMGCREATEPOPUPPROC mgCreatePopup;
PFNMGDESTROYPOPUPPROC mgDestroyPopup;
PFNMGSHOWPOPUPPROC mgShowPopup;
PFNMGSHOWWINDOWPROC mgShowWindow;
PFNMGINITDEFAULTICONSPROC mgInitDefaultIcons;
PFNMGDOCKGETSAVEDATAPROC mgDockGetSaveData;
PFNMGDOCKLOADDATAPROC mgDockLoadData;

void* 
mgGetProc(MG_LIB_HANDLE lib, const char* proc)
{
	void* res = 0;

#ifdef MG_PLATFORM_WINDOWS
	res = GetProcAddress(lib, proc);
#else
#error Need implementation
#endif
	return res;
}

MG_LIB_HANDLE
mgLoad()
{
#ifdef MG_PLATFORM_WINDOWS
	const char* lib_name = "migui.dll";
#else
#error Need implementation
#endif

	MG_LIB_HANDLE lib = MG_LOAD_LIB(lib_name);
	if(!lib)
		return lib;

	mgCreateContext = mgGetProc(lib, "mgCreateContext_f");
	mgDestroyContext = mgGetProc(lib, "mgDestroyContext_f");
	mgCreateFont = mgGetProc(lib, "mgCreateFont_f");
	mgDestroyFont = mgGetProc(lib, "mgDestroyFont_f");
	mgStartFrame = mgGetProc(lib, "mgStartFrame_f");
	mgUpdate = mgGetProc(lib, "mgUpdate_f");
	mgSetParent = mgGetProc(lib, "mgSetParent_f");
	mgSetVisible = mgGetProc(lib, "mgSetVisible_f");
	mgDraw = mgGetProc(lib, "mgDraw_f");
	mgCreateRectangle = mgGetProc(lib, "mgCreateRectangle_f");
	mgCreateText = mgGetProc(lib, "mgCreateText_f");
	mgCreateButton = mgGetProc(lib, "mgCreateButton_f");
	mgCreateWindow = mgGetProc(lib, "mgCreateWindow_f");
	mgDestroyWindow = mgGetProc(lib, "mgDestroyWindow_f");
	mgSetWindowTitle = mgGetProc(lib, "mgSetWindowTitle_f");
	mgBringWindowToTop = mgGetProc(lib, "mgBringWindowToTop_f");
	mgCreateCursor = mgGetProc(lib, "mgCreateCursor_f");
	mgDestroyCursor = mgGetProc(lib, "mgDestroyCursor_f");
	mgSetCursor = mgGetProc(lib, "mgSetCursor_f");
	mgCreateIcons = mgGetProc(lib, "mgCreateIcons_f");
	mgDestroyIcons = mgGetProc(lib, "mgDestroyIcons_f");
	mgSetIcon = mgGetProc(lib, "mgSetIcon_f");
	mgOnWindowSize = mgGetProc(lib, "mgOnWindowSize_f");
	mgInitDockPanel = mgGetProc(lib, "mgInitDockPanel_f");
	mgDockAddWindow = mgGetProc(lib, "mgDockAddWindow_f");
	mgCreatePopup = mgGetProc(lib, "mgCreatePopup_f");
	mgDestroyPopup = mgGetProc(lib, "mgDestroyPopup_f");
	mgShowPopup = mgGetProc(lib, "mgShowPopup_f");
	mgShowWindow = mgGetProc(lib, "mgShowWindow_f");
	mgInitDefaultIcons = mgGetProc(lib, "mgInitDefaultIcons_f");
	mgDockGetSaveData = mgGetProc(lib, "mgDockGetSaveData_f");
	mgDockLoadData = mgGetProc(lib, "mgDockLoadData_f");


	return lib;
}

void
mgUnload(MG_LIB_HANDLE lib)
{
	assert(lib);
	MG_FREE_LIB(lib);
}