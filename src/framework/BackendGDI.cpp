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
#include "framework/SystemWindowImpl.h"
#include "framework/BackendGDI.h"

#ifdef MG_PLATFORM_WINDOWS

using namespace mgf;

extern mgf::Backend* g_backend;

void BackendGDI_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	g_backend->GetTextSize(text, font, sz);
}

mgTexture BackendGDI_createTexture(mgImage* img)
{
	return g_backend;
}

void BackendGDI_destroyTexture(mgTexture t)
{
}

void BackendGDI_beginDraw()
{
	g_backend->BeginDraw();
}

void BackendGDI_endDraw()
{
	g_backend->EndDraw();
}

void BackendGDI_drawRectangle(
	int reason,
	void* object,
	mgRect* rct,
	mgColor* color1,
	mgColor* color2,
	mgTexture texture, /*optional*/
	mgVec4* UVRegion)
{
	g_backend->DrawRectangle(reason, object, rct, color1, color2, texture, UVRegion);
}

void BackendGDI_drawText(
	int reason,
	void* object,
	mgPoint* position,
	const wchar_t* text,
	int textLen,
	mgColor* color,
	mgFont* font)
{
	g_backend->DrawText(reason, object, position, text, textLen, color, font);
}

mgRect BackendGDI_setClipRect(mgRect* r)
{
	return g_backend->SetClipRect(r);
}

BackendGDI::BackendGDI()
{
	m_getTextSize = BackendGDI_getTextSize;
	g_backend = this;
	m_gpu = new mgVideoDriverAPI;
	((mgVideoDriverAPI*)m_gpu)->createTexture = BackendGDI_createTexture;
	((mgVideoDriverAPI*)m_gpu)->destroyTexture = BackendGDI_destroyTexture;
	((mgVideoDriverAPI*)m_gpu)->beginDraw = BackendGDI_beginDraw;
	((mgVideoDriverAPI*)m_gpu)->endDraw = BackendGDI_endDraw;
	((mgVideoDriverAPI*)m_gpu)->drawRectangle = BackendGDI_drawRectangle;
	((mgVideoDriverAPI*)m_gpu)->drawText = BackendGDI_drawText;
	((mgVideoDriverAPI*)m_gpu)->setClipRect = BackendGDI_setClipRect;
}

BackendGDI::~BackendGDI()
{
	delete m_gpu;
}

void* BackendGDI::GetVideoDriverAPI()
{
	return m_gpu;
}

void BackendGDI::BeginDraw()
{
	m_window->m_hbmOld = (HBITMAP)SelectObject(m_window->m_hdcMem, m_window->m_hbmMem);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = m_window->m_size.x + m_window->m_borderSize.x;
	r.bottom = m_window->m_size.y + m_window->m_borderSize.y;
	FillRect(m_window->m_hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 1));
}

void BackendGDI::EndDraw()
{
	HDC dc = GetWindowDC(m_window->m_hWnd);
	BitBlt(dc,
		0, 0,
		m_window->m_size.x + m_window->m_borderSize.x,
		m_window->m_size.y + m_window->m_borderSize.y,
		m_window->m_hdcMem,
		0, 0,
		SRCCOPY);
	ReleaseDC(m_window->m_hWnd, dc);
}

mgTexture BackendGDI::CreateTexture(mgImage* img)
{
	return 0;
}

void BackendGDI::DestroyTexture(mgTexture)
{

}

void BackendGDI::DrawRectangle(int reason, void* object, mgRect* rct, mgColor* color1, mgColor* color2,
	mgTexture texture, mgVec4* UVRegion)
{
	unsigned int c1 = mgColorGetAsIntegerARGB(color1);
	unsigned int c2 = mgColorGetAsIntegerARGB(color2);
	HBRUSH brsh = CreateSolidBrush(RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
	SelectObject(m_window->m_hdcMem, brsh);

	RECT r;
	r.left = rct->left + m_window->m_borderSize.x;
	r.top = rct->top + m_window->m_borderSize.y;
	r.right = rct->right + m_window->m_borderSize.x;
	r.bottom = rct->bottom + m_window->m_borderSize.y;

	HRGN rgn = 0;
	rgn = CreateRectRgn(
		m_clipRect.left + m_window->m_borderSize.x, 
		m_clipRect.top + m_window->m_borderSize.y,
		m_clipRect.right + m_window->m_borderSize.x,
		m_clipRect.bottom + m_window->m_borderSize.y);
	SelectClipRgn(m_window->m_hdcMem, rgn);
	FillRect(m_window->m_hdcMem, &r, brsh);
	DeleteObject(rgn);
	DeleteObject(brsh);
	SelectClipRgn(m_window->m_hdcMem, 0);
}

void BackendGDI::DrawText(int reason, void* object, mgPoint* position, const wchar_t* text, int textLen,
	mgColor* color, mgFont* font)
{
	SelectObject(m_window->m_hdcMem, font->implementation);
	SetTextColor(m_window->m_hdcMem, mgColorGetAsIntegerBGR(color));
	SetBkMode(m_window->m_hdcMem, TRANSPARENT);

	HRGN rgn = CreateRectRgn(
		m_clipRect.left + m_window->m_borderSize.x,
		m_clipRect.top + m_window->m_borderSize.y,
		m_clipRect.right + m_window->m_borderSize.x,
		m_clipRect.bottom + m_window->m_borderSize.y);

	SelectClipRgn(m_window->m_hdcMem, rgn);
	TextOutW(m_window->m_hdcMem, 
		position->x + m_window->m_borderSize.x, 
		position->y + m_window->m_borderSize.y, 
		text, textLen);
	DeleteObject(rgn);
	SelectClipRgn(m_window->m_hdcMem, 0);
}

mgRect BackendGDI::SetClipRect(mgRect* r)
{
	mgRect old = m_clipRect;
	m_clipRect = *r;
	return old;
}


void BackendGDI_createBackBuffer(mgf::SystemWindowImpl* impl)
{
	HDC dc = GetWindowDC(impl->m_hWnd);
	impl->m_hdcMem = CreateCompatibleDC(dc);
	RECT r;
	GetWindowRect(impl->m_hWnd, &r);
	impl->m_hbmMem = CreateCompatibleBitmap(dc,
		r.right - r.left,
		r.bottom - r.top);
	ReleaseDC(impl->m_hWnd, dc);	
}

void BackendGDI::InitWindow(mgf::SystemWindow* w)
{
	if (m_window->m_hdcMem)
		return;
	BackendGDI_createBackBuffer(m_window);
}

void BackendGDI::SetActiveWindow(mgf::SystemWindow* w)
{
	m_window = (mgf::SystemWindowImpl*)w;
}

void BackendGDI::UpdateBackbuffer()
{
	if (m_window->m_hdcMem)
		DeleteDC(m_window->m_hdcMem);
	if (m_window->m_hbmMem)
		DeleteObject(m_window->m_hbmMem);
	
	BackendGDI_createBackBuffer(m_window);
}

void BackendGDI::GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	SelectObject(m_window->m_hdcMem, font->implementation);
	int c = wcslen(text);
	if (!c)
		return;
	SIZE s;
	GetTextExtentPoint32W(m_window->m_hdcMem, text, c, &s);
	sz->x = s.cx;
	sz->y = s.cy;
}

#endif
