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
#ifndef _MG_FORWARD_H_
#define _MG_FORWARD_H_

struct mgSystemWindowOSData;
struct mgPoint_s;
struct mgRect_s;
struct mgVec4_s;
struct mgColor_s;
struct mgFontBitmap_s;
struct mgFontGlyph_s;
struct mgFont_s;
struct mgImage_s;
struct mgIconsNode_s;
struct mgIcons_s;
struct mgIconGroup_s;
struct mgStyle_s;
struct mgPopupItemInfo_s;
struct mgPopupItem_s;
struct mgPopup_s;
struct mgMenuItemInfo_s;
struct mgMenuItem_s;
struct mgMenu_s;
struct mgDockPanelWindow_s;
struct mgDockPanelElementCreationInfo_s;
struct mgDockPanelElement_s;
struct mgDockPanel_s;
struct mgWindow_s;
struct mgElementNode_s;
struct mgElementTransform_s;
struct mgElement_s;
struct mgElementRectangle_s;
struct mgElementText_s;
struct mgElementButton_s;
struct mgVideoDriverAPI_s;
struct mgInputContext_s;
struct mgCursor_s;
struct mgFunctions_s;
struct mgContext_s;
struct mgTexture_s;
struct mgElementTextInput_s;
struct mgElementList_s;
struct mgElementTable_s;

enum mgBeginDrawReason
{
	mgBeginDrawReason_systemWindow,
	mgBeginDrawReason_popupWindow
};

enum mgDrawRectangleReason
{
	mgDrawRectangleReason_user,
	mgDrawRectangleReason_windowBG,
	mgDrawRectangleReason_windowTitlebar,
	mgDrawRectangleReason_windowCloseButton,
	mgDrawRectangleReason_windowCollapseButton,
	mgDrawRectangleReason_windowScrollbarBG,
	mgDrawRectangleReason_windowScrollbarElement,
	mgDrawRectangleReason_windowMenuBG,
	mgDrawRectangleReason_windowMenuHoverItemBG,
	mgDrawRectangleReason_windowMenuActiveItemBG,
	mgDrawRectangleReason_tooltip,
	mgDrawRectangleReason_rectangle,
	mgDrawRectangleReason_rectangleWithTexture,
	mgDrawRectangleReason_buttonBG,
	mgDrawRectangleReason_buttonIcon,
	mgDrawRectangleReason_dockBG,
	mgDrawRectangleReason_dockElementBG,
	mgDrawRectangleReason_dockSplitterBGHor,
	mgDrawRectangleReason_dockSplitterBGVert,
	mgDrawRectangleReason_dockPanelSplitterBGHor,
	mgDrawRectangleReason_dockPanelSplitterBGVert,
	mgDrawRectangleReason_dockWindowToDock,
	mgDrawRectangleReason_dockTabBG,
	mgDrawRectangleReason_dockTabWindowTitle,
	mgDrawRectangleReason_popupBG,
	mgDrawRectangleReason_popupHoverElement,
	mgDrawRectangleReason_popupSeparator,
	mgDrawRectangleReason_popupNextIcon,
	mgDrawRectangleReason_popupCheckIcon,
	mgDrawRectangleReason_textInputBG,
	mgDrawRectangleReason_textInputCursor,
	mgDrawRectangleReason_textInputSelectionBG,
	mgDrawRectangleReason_listBG,
	mgDrawRectangleReason_listItemBG1,
	mgDrawRectangleReason_listItemBG2,
	mgDrawRectangleReason_listHoverItemBG,
	mgDrawRectangleReason_listSelectedItemBG,
	mgDrawRectangleReason_tableBG,
	mgDrawRectangleReason_tableRowBG1,
	mgDrawRectangleReason_tableRowBG2,
	mgDrawRectangleReason_tableHoverItemBG,
	mgDrawRectangleReason_tableCellBG,
	mgDrawRectangleReason_tableRowSelectedBG,
	mgDrawRectangleReason_tableColTitleBG,
	mgDrawRectangleReason_tableColTitleColBG,
	mgDrawRectangleReason_tableColTitleColHover,
	mgDrawRectangleReason_tableColTitleColActive,
};

enum mgDrawTextReason
{
	mgDrawTextReason_user,
	mgDrawTextReason_windowTitlebar,
	mgDrawTextReason_windowMenu,
	mgDrawTextReason_buttonText,
	mgDrawTextReason_text,
	mgDrawTextReason_table,
	mgDrawTextReason_tableColTitle,
	mgDrawTextReason_listbox,
	mgDrawTextReason_popup,
	mgDrawTextReason_popupShortcut,
	mgDrawTextReason_tooltip,
	mgDrawTextReason_textInputDefaultText,
};

#endif