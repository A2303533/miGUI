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
#ifndef _MG_STYLE_H_
#define _MG_STYLE_H_

typedef struct mgStyle_s {
	mgColor windowBG;
	mgColor windowTitlebar;
	mgColor windowBGTopWindow;
	mgColor windowTitlebarTopWindow;
	mgColor windowTitlebarText;
	mgColor windowScrollbarBG;
	mgColor windowScrollbarElement;
	mgColor windowMenuBG;
	mgColor windowMenuHoverItemBG;
	mgColor windowMenuActiveItemBG;
	mgColor windowMenuText;
	mgColor windowMenuTextDisabled;

	mgColor tooltipBG;
	mgColor tooltipText;

	mgColor dockpanelBG;
	mgColor dockpanelSplitterBG;
	mgColor dockpanelPanelSplitterBG;
	mgColor dockpanelWindowToDock;
	mgColor dockpanelTabBG;
	mgColor dockpanelTabWindowTitleBG;
	mgColor dockpanelTabActiveWindowTitleBG;
	
	mgColor popupBG;
	mgColor popupText;
	mgColor popupTextDisabled;
	mgColor popupTextShortcut;
	mgColor popupTextShortcutDisabled;
	mgColor popupHoverElementBG;
	mgColor popupSeparator;

	mgColor button1;
	mgColor button2;
	mgColor buttonHover1;
	mgColor buttonHover2;
	mgColor buttonPress1;
	mgColor buttonPress2;
	mgColor buttonDisabled1;
	mgColor buttonDisabled2;

	mgColor buttonText;
	mgColor buttonTextHover;
	mgColor buttonTextPress;
	mgColor buttonTextDisabled;
	
	mgColor textInputBGActive;
	mgColor textInputBGNotActive;
	mgColor textInputDefaultText;
	mgColor textInputText;
	mgColor textInputTextSelected;
	mgColor textInputCursor;
	mgColor textInputSelectionBGActive;
	mgColor textInputSelectionBGNotActive;

	mgColor listBG;
	mgColor listItemText;
	mgColor listItemBG1;
	mgColor listItemBG2;
	mgColor listItemHoverBG;
	mgColor listItemSelectedBG;

	mgColor tableBG;
	mgColor tableRowBG1;
	mgColor tableRowBG2;
	mgColor tableRowHoverBG;
	mgColor tableRowSelectedBG;
	mgColor tableCellText;
	mgColor tableCellBG;
	mgColor tableColTitleBG;
	mgColor tableColTitleColBG;
	mgColor tableColTitleColHover;
	mgColor tableColTitleColActive;

} mgStyle;

#endif