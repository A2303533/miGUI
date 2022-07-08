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

#include "framework/mgf.h"
#include "framework/Window.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"
#include "framework/Popup.h"
#include "framework/TextProcessor.h"

using namespace mgf;
extern Backend* g_backend;

Popup::Popup(Context* c)
	:
	m_implementation(0),
	m_type(type_migui),
	m_context(c)
{
}

Popup::~Popup()
{
	if (m_type == type_migui)
	{
		mgPopup* popup = (mgPopup*)m_implementation;
		mgDestroyPopup(popup);
	}
}

void Popup::SetTextProcessor(TextProcessor* tp)
{
	m_textProcessor = tp;
	mgPopup* popup = (mgPopup*)m_implementation;
	if(popup)
		popup->textProcessor = tp->GetTextProcessor();
}

//void Popup::SetFont(Font* f)
//{
//	if (m_type == type_migui)
//	{
//		mgPopup* popup = (mgPopup*)m_implementation;
//#pragma message("!!!!! !!!! !!!! Maybe need to remove font and add SetTextProcessor: " __FILE__ __FUNCTION__ " LINE : ")
//				//popup->font = ((FontImpl*)f)->m_font;
//	}
//}

void Popup::Show(int x, int y)
{
	if (m_type == type_migui)
	{
		mgPopup* popup = (mgPopup*)m_implementation;
		mgPoint pos;
		pos.x = x;
		pos.y = y;
		mgShowPopup(m_context->m_gui_context, popup, &pos);
	}
}
