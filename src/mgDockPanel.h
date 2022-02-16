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

#ifndef _MG_DOCKPANEL_H_
#define _MG_DOCKPANEL_H_

//typedef struct mgDockPanelWindowNode_s {
//	struct mgWindow_s* window;
//	struct mgDockPanelWindowNode_s* left;
//	struct mgDockPanelWindowNode_s* right;
//} mgDockPanelWindowNode;

typedef struct mgDockPanelWindow_s {	
	mgRect rect; /*all area, with tabbar and other*/
	mgRect windowRect;/*only window*/
	mgRect tabRect;
	mgRect splitterRect;

	/*
	* 0 - left, 1 - top, 2 - right, 3 - bottom
	*/
	int where;

	/*int id;
	int parentId;*/
	struct mgDockPanelWindow_s* parent;

	struct mgWindow_s** windows;
	int windowsSize;
	struct mgWindow_s* activeWindow;

	struct mgDockPanelElement_s* dockElement;

	int sz;
	int sizeOnClick;
} mgDockPanelWindow;

typedef struct mgDockPanelElementCreationInfo_s {
	/*
	* 0 - left, 1 - top, 2 - right, 3 - bottom
	*/
	int where;

	int size;
	int sizeMinimum;
	int sizeMaximum;
} mgDockPanelElementCreationInfo;

enum mgDockPanelElementFlag
{
	mgDockPanelElementFlag_drawBG = 0x1
};

typedef struct mgDockPanelElement_s {
	/*mgRect indent;*/
	mgRect rect;
	int flags;

	mgRect splitterRect;
	mgRect addWindowRect;
	
	mgDockPanelElementCreationInfo info;
	int sizeOnClick;

	mgColor colorBG;

	mgDockPanelWindow** panelWindows;
	int panelWindowsSize;

} mgDockPanelElement;

enum mgDockPanelFlag
{
	mgDockPanelFlag_drawBG = 0x1,
	mgDockPanelFlag_drawSplitterBG = 0x2,

	/*internal*/
	mgDockPanelFlag_cursorChanged = 0x40000000,
	mgDockPanelFlag_onSplitter = 0x80000000,
};

typedef struct mgDockPanel_s {
	mgRect indent; /*set this on creation*/
	
	mgRect rect; /*update it when resize window*/
	mgPoint size; /*current rect size*/

	int flags;

	mgDockPanelElement* elements;
	int elementsNum;
	
	int splitterWidth; /*2*/

	mgPoint mainElementSize;
	mgPoint mainElementSizeMinimum;/*300 300*/

	/*update and draw only windows in this array*/
	mgDockPanelWindow** arrayWindows;
	int arrayWindowsSize;

	int tabHeight;

} mgDockPanel;

#endif