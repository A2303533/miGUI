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

#ifndef _MG_FUNCTIONS_H_
#define _MG_FUNCTIONS_H_

//mgElement* MG_C_DECL mgCreateButton_f(struct mgWindow_s* c, mgPoint* position, mgPoint* size);
//mgContext* MG_C_DECL mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input);
//mgCursor* MG_C_DECL mgCreateCursor_f(const wchar_t* fileName);
//struct mgWindow_s* MG_C_DECL mgCreateWindow_f(struct mgContext_s* ctx, int px, int py, int sx, int sy);
//void MG_C_DECL mgDestroyWindow_f(struct mgWindow_s* w);
//mgFont* MG_C_DECL mgCreateFont_f(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt);
//mgElement* MG_C_DECL mgCreateRectangle_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size);
//mgElement* MG_C_DECL mgCreateText_f(struct mgWindow_s* c, mgPoint* position);
//void MG_C_DECL mgDestroyContext_f(mgContext* c);
//void MG_C_DECL mgDestroyCursor_f(mgCursor* c);
//void MG_C_DECL mgDestroyFont_f(struct mgContext_s* c, mgFont* f);
//void MG_C_DECL mgDraw_f(mgContext* c);
//void MG_C_DECL mgSetCursor_f(mgContext* ctx, mgCursor* c, unsigned int type);
//void MG_C_DECL mgSetParent_f(mgElement* object, mgElement* parent);
//void MG_C_DECL mgSetVisible_f(mgElement* e, int v);
//void MG_C_DECL mgStartFrame_f(mgContext* c);
//void MG_C_DECL mgUpdate_f(mgContext* c);
//struct mgDockPanelWindow_s* MG_C_DECL mgDockAddWindow_f(struct mgWindow_s* w, struct mgDockPanelWindow_s* dw, int id);
//struct mgPopup_s* MG_C_DECL mgCreatePopup_f(struct mgContext_s* c, struct mgPopupItemInfo_s* arr, int arrSize, mgFont* fnt, int flags);
//void MG_C_DECL mgDestroyPopup_f(struct mgPopup_s* p);
//void MG_C_DECL mgShowPopup_f(struct mgContext_s* c, struct mgPopup_s* p, mgPoint* position);
//void MG_C_DECL mgShowWindow_f(struct mgWindow_s* w, int i);
//void MG_C_DECL mgDestroyIcons_f(mgIcons* ic);
//mgIcons* MG_C_DECL mgCreateIcons_f(mgTexture* t, int textureSizeX, int textureSizeY, int iconNum);
//void MG_C_DECL mgSetIcon_f(mgIcons* ic, int id, int px, int py, int sx, int sy);
//mgMenu* MG_C_DECL mgCreateMenu_f(struct mgContext_s*, mgMenuItemInfo* items, int itemsSize, mgFont* f);
//void MG_C_DECL mgDestroyMenu_f(mgMenu* m);
//mgElement* MG_C_DECL mgCreateTextInput_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgFont* font);
//void MG_C_DECL mgTextInputClear_f(struct mgElementTextInput_s* e, int freeMemory);
//void MG_C_DECL mgTextInputDeleteSelected_f(struct mgElementTextInput_s* e);
//int MG_C_DECL mgTextInputPutText_f(struct mgElementTextInput_s* e, const wchar_t* text, uint32_t len);
//mgElement* MG_C_DECL mgCreateListBox_f(struct mgWindow_s* w,mgPoint* position,mgPoint* size,void** items,uint32_t itemsSize,mgFont* f);
//void MG_C_DECL mgTextInputActivate_f(struct mgContext_s*, struct mgElementTextInput_s* e, int isActive, int deactivateCode);
//void MG_C_DECL mgTextInputSetText_f(struct mgElementTextInput_s* e, const wchar_t* text);
//void MG_C_DECL mgDrawWindow_f(struct mgWindow_s* w);
//void MG_C_DECL mgDrawPopup_f(struct mgContext_s* c, mgPopup* p);
//struct mgPopup_s* mgGetDefaultPopupTextInput(mgContext* c);

#endif