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
#include "framework/Element.h"

using namespace mgf;

Element::Element()
:
m_element(0),
m_userData(0),
m_alignment(LeftTop)
{
}

Element::~Element()
{
}

void Element_onMouseEnter(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnMouseEnter();
}
void Element_onMouseLeave(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnMouseLeave();
}
void Element_onClickLMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnClickLMB();
}
void Element_onClickRMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnClickRMB();
}
void Element_onClickMMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnClickMMB();
}
void Element_onClickX1MB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnClickX1MB();
}
void Element_onClickX2MB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnClickX2MB();
}
void Element_onReleaseLMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnReleaseLMB();
}
void Element_onReleaseRMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnReleaseRMB();
}
void Element_onReleaseMMB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnReleaseMMB();
}
void Element_onReleaseX1MB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnReleaseX1MB();
}
void Element_onReleaseX2MB(struct mgElement_s* e) {
	Element* el = (Element*)e->userData;
	el->OnReleaseX2MB();
}

void Element::PostInit()
{
	m_element->onMouseEnter = Element_onMouseEnter;
	m_element->onMouseLeave = Element_onMouseLeave;
	m_element->onClickLMB = Element_onClickLMB;
	m_element->onClickRMB = Element_onClickRMB;
	m_element->onClickMMB = Element_onClickMMB;
	m_element->onClickX1MB = Element_onClickX1MB;
	m_element->onClickX2MB = Element_onClickX2MB;
	m_element->onReleaseLMB = Element_onReleaseLMB;
	m_element->onReleaseRMB = Element_onReleaseRMB;
	m_element->onReleaseMMB = Element_onReleaseMMB;
	m_element->onReleaseX1MB = Element_onReleaseX1MB;
	m_element->onReleaseX2MB = Element_onReleaseX2MB;
	m_element->userData = this;
}

void Element::SetUserData(void* d)
{
	m_userData = d;
}

void* Element::GetUserData()
{
	return m_userData;
}

void Element::SetVisible(bool v)
{
	m_element->visible = v ? 1 : 0;
}

bool Element::IsVisible()
{
	return (bool)m_element->visible;
}

void Element::SetID(int id)
{
	m_element->id = id;
}

int Element::GetID()
{
	return m_element->id;
}

void Element::SetRect(mgRect* r)
{
	assert(m_element);
	m_element->transformLocal.buildArea = *r;
	m_element->transformLocal.clipArea = *r;
	m_element->creationRect = *r;
	m_element->transformWorld = m_element->transformLocal;
}

void Element::SetRect(int left, int top, int right, int bottom)
{
	assert(m_element);
	m_element->transformLocal.buildArea.left = left;
	m_element->transformLocal.buildArea.top = top;
	m_element->transformLocal.buildArea.right = right;
	m_element->transformLocal.buildArea.bottom = bottom;
	m_element->transformLocal.clipArea.left = left;
	m_element->transformLocal.clipArea.top = top;
	m_element->transformLocal.clipArea.right = right;
	m_element->transformLocal.clipArea.bottom = bottom;
	m_element->creationRect.left = left;
	m_element->creationRect.top = top;
	m_element->creationRect.right = right;
	m_element->creationRect.bottom = bottom;
	m_element->transformWorld = m_element->transformLocal;
}

void Element::SetPositionAndSize(int posX, int posY, int sizeX, int sizeY)
{
	SetRect(posX, posY, posX + sizeX, posY + sizeY);
}

void Element::SetRectLeft(int v)
{
	m_element->transformLocal.buildArea.left = v;
	m_element->transformLocal.clipArea.left = v;
	m_element->creationRect.left = v;
	m_element->transformWorld = m_element->transformLocal;
}

void Element::SetRectTop(int v)
{
	m_element->transformLocal.buildArea.top = v;
	m_element->transformLocal.clipArea.top = v;
	m_element->creationRect.top = v;
	m_element->transformWorld = m_element->transformLocal;
}

void Element::SetRectRight(int v)
{
	m_element->transformLocal.buildArea.right = v;
	m_element->transformLocal.clipArea.right = v;
	m_element->creationRect.right = v;
	m_element->transformWorld = m_element->transformLocal;
}

void Element::SetRectBottom(int v)
{
	m_element->transformLocal.buildArea.bottom = v;
	m_element->transformLocal.clipArea.bottom = v;
	m_element->creationRect.bottom = v;
	m_element->transformWorld = m_element->transformLocal;
}

Element::Alignment Element::GetAlignment()
{
	return m_alignment;
}

void Element::SetAlignment(Element::Alignment a)
{
	m_alignment = a;
	switch (a)
	{
	case LeftTop:
		m_element->align = mgAlignment_leftTop;
		break;
	case Top:
		m_element->align = mgAlignment_top;
		break;
	case RightTop:
		m_element->align = mgAlignment_rightTop;
		break;
	case Right:
		m_element->align = mgAlignment_right;
		break;
	case RightBottom:
		m_element->align = mgAlignment_rightBottom;
		break;
	case Bottom:
		m_element->align = mgAlignment_bottom;
		break;
	case LeftBottom:
		m_element->align = mgAlignment_leftBottom;
		break;
	case Left:
		m_element->align = mgAlignment_left;
		break;
	case Center:
		m_element->align = mgAlignment_center;
		break;
	}
}

void Element::SetUserStyle(mgStyle_s* s)
{
	m_element->userStyle = s;
}

void Element::SetDrawBG(bool v)
{
	m_element->drawBG = v ? 1 : 0;
}

bool Element::IsDrawBG()
{
	return m_element->drawBG;
}

void Element::SetEnabled(bool v)
{
	m_element->enabled = v ? 1 : 0;
}

bool Element::IsEnabled()
{
	return m_element->enabled;
}

mgElement_s* Element::GetElement()
{
	return m_element;
}

void Element::OnMouseEnter()
{

}

void Element::OnMouseLeave()
{

}

void Element::OnClickLMB()
{

}

void Element::OnClickRMB()
{

}

void Element::OnClickMMB()
{

}

void Element::OnClickX1MB()
{

}

void Element::OnClickX2MB()
{

}

void Element::OnReleaseLMB()
{

}

void Element::OnReleaseRMB()
{

}

void Element::OnReleaseMMB()
{

}

void Element::OnReleaseX1MB()
{

}

void Element::OnReleaseX2MB()
{

}
