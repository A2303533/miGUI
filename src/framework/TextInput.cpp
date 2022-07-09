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
#include "framework/TextInput.h"
#include "framework/TextProcessor.h"

using namespace mgf;
extern Backend* g_backend;


int TextInput_onEndEdit(struct mgElement_s* e, int type)
{
	TextInput* impl = (TextInput*)e->userData;
	TextInput::EndEdit ee = TextInput::EndEdit::_unknown;
	switch (type)
	{
	case 1: ee = TextInput::EndEdit::_enterKey; break;
	case 2: ee = TextInput::EndEdit::_click; break;
	case 3: ee = TextInput::EndEdit::_escapeKey; break;
	}
	return impl->OnEndEdit(ee);
}

mgUnicodeChar TextInput_onCharEnter(struct mgElement_s* e, mgUnicodeChar c)
{
	TextInput* impl = (TextInput*)e->userData;
	return impl->OnCharEnter(c);
}

void TextInput_onActivate(struct mgElement_s* e)
{
	TextInput* impl = (TextInput*)e->userData;
	impl->OnActivate();
}

int TextInput_onPaste(struct mgElement_s* e, mgUnicodeChar* str, size_t sz)
{
	TextInput* impl = (TextInput*)e->userData;
	return impl->OnPaste(str, sz);
}

TextInput::TextInput(Window* w)
	:
	Element(w),
	m_elementText(0)
{
	assert(w);

	mgPoint p;
	mgPointSet(&p, 0, 0);
	this->SetTextProcessor(g_backend->GetTextProcessor());

	m_element = mgCreateTextInput(w->m_window, &p, &p, m_textProcessor->GetTextProcessor());
	m_elementText = (mgElementTextInput_s*)m_element->implementation;
	m_elementText->onActivate = TextInput_onActivate;
	m_elementText->onCharEnter = TextInput_onCharEnter;
	m_elementText->onEndEdit = TextInput_onEndEdit;
	m_elementText->onPaste = TextInput_onPaste;
	
	m_element->userData = this;

	Element::PostInit();
}

TextInput::~TextInput()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void TextInput::SetTextProcessor(TextProcessor* tp)
{
	Element::SetTextProcessor(tp);
	if(m_elementText)
		m_elementText->textProcessor = tp->GetTextProcessor();
}

void TextInput::SetText(const mgUnicodeChar* text)
{
	mgTextInputSetText(m_elementText, text);
}

void TextInput::SetDefaultText(const mgUnicodeChar* text)
{
	m_elementText->defaultTextLen = 0;
	m_elementText->defaultText = text;
	if (text)
		m_elementText->defaultTextLen = mgUnicodeStrlen(text);
}

void TextInput::SetCharLimit(uint32_t i)
{
	m_elementText->charLimit = i;
}

const mgUnicodeChar* TextInput::GetText()
{
	return m_elementText->text;
}

size_t TextInput::GetTextSize()
{
	return m_elementText->textLen;
}

int TextInput::OnEndEdit(EndEdit ee)
{
	return 1;
}

mgUnicodeChar TextInput::OnCharEnter(mgUnicodeChar c)
{
	return c;
}

void TextInput::OnActivate()
{
}

int TextInput::OnPaste(mgUnicodeChar* s, size_t sz)
{
	return 1;
}
