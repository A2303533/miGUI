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
#ifdef MGF_BACKEND_OPENGL

#include "framework/Context.h"
#include "framework/SystemWindow.h"
#include "framework/BackendOpenGL.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"

#include "GL/wgl.h"


PFNWGLGETEXTENSIONSSTRINGEXTPROC oldgl_wglGetExtensionsStringEXT = 0;
PFNWGLSWAPINTERVALEXTPROC       oldgl_wglSwapIntervalEXT = 0;

struct BackendOpenGL_texture
{
	GLuint textureID = 0;
};

using namespace mgf;

extern mgf::Backend* g_backend;

void BackendOpenGL_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	g_backend->GetTextSize(text, font, sz);
}

mgTexture* BackendOpenGL_createTexture(mgImage* img)
{
	return g_backend->CreateTexture(img);
}

void BackendOpenGL_destroyTexture(mgTexture* t)
{
	g_backend->DestroyTexture(t);
}

void BackendOpenGL_beginDraw(int reason)
{
	g_backend->BeginDraw(reason);
}

void BackendOpenGL_endDraw()
{
	g_backend->EndDraw();
}

void BackendOpenGL_drawRectangle(
	int reason,
	void* object,
	mgRect* rct,
	mgColor* color1,
	mgColor* color2,
	mgTexture* texture, /*optional*/
	mgVec4* UVRegion)
{
	g_backend->DrawRectangle(reason, object, rct, color1, color2, texture, UVRegion);
}

void BackendOpenGL_drawText(
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

void BackendOpenGL_drawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	g_backend->DrawLine(reason, object, position, where, color, size);
}

mgRect BackendOpenGL_setClipRect(mgRect* r)
{
	return g_backend->SetClipRect(r);
}

BackendOpenGL::BackendOpenGL(BackendOpenGLParams params)
{
	m_params = params;
	m_getTextSize = BackendOpenGL_getTextSize;
	g_backend = this;
	m_gpu = new mgVideoDriverAPI;
	((mgVideoDriverAPI*)m_gpu)->createTexture = BackendOpenGL_createTexture;
	((mgVideoDriverAPI*)m_gpu)->destroyTexture = BackendOpenGL_destroyTexture;
	((mgVideoDriverAPI*)m_gpu)->beginDraw = BackendOpenGL_beginDraw;
	((mgVideoDriverAPI*)m_gpu)->endDraw = BackendOpenGL_endDraw;
	((mgVideoDriverAPI*)m_gpu)->drawRectangle = BackendOpenGL_drawRectangle;
	((mgVideoDriverAPI*)m_gpu)->drawText = BackendOpenGL_drawText;
	((mgVideoDriverAPI*)m_gpu)->setClipRect = BackendOpenGL_setClipRect;
	
	blackImage = new Image;
	blackImage->Create(20, 20, mgColor(0xff020202));
	/*blackBitmap = new Gdiplus::Bitmap(blackImage->GetWidth(), blackImage->GetHeight(), blackImage->GetPitch(), 
		PixelFormat32bppARGB, blackImage->GetData());*/
}

BackendOpenGL::~BackendOpenGL()
{
	if (blackImage)
		delete blackImage;

	if (m_defaultFont)
		m_defaultFont->Release();

	if (m_defaultIcons)
	{
		/*if(m_defaultIcons->implementation)
			delete ((Gdiplus::Bitmap*)m_defaultIcons->implementation);*/
		delete m_defaultIcons;
	}

	if (m_params.withInit)
	{
		wglMakeCurrent(NULL, NULL);
		if (m_params.hglrc)
			wglDeleteContext(m_params.hglrc);
	}

	delete m_gpu;
}


mgTexture_s* BackendOpenGL::GetDefaultIcons()
{

	if (m_defaultIcons)
		return m_defaultIcons;

	m_defaultIcons = new mgTexture_s;
	//m_defaultIcons->implementation = new Gdiplus::Bitmap(L"../data/icons.png");
	return m_defaultIcons;
}

void* BackendOpenGL::GetVideoDriverAPI()
{
	return m_gpu;
}

void BackendOpenGL::BeginDraw(int reason)
{
	if (m_params.withClearColorAndDepth)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, m_currWindow->m_size.x, m_currWindow->m_size.y, 0);
}

void BackendOpenGL::EndDraw()
{
	if (m_params.withSwapBuffers)
	{
		HDC dc = GetWindowDC(m_currWindow->m_OSData->hWnd);
		SwapBuffers(dc);
		ReleaseDC(m_currWindow->m_OSData->hWnd, dc);
	}
}

mgTexture* BackendOpenGL::CreateTexture(mgImage* img)
{
	if (img->type == mgImageType_unknown)
		return 0;
	
	int bytes = GL_RGBA8;
	unsigned int pixelFormat = 0;
	switch (img->type)
	{
	case mgImageType_r8g8b8:
		pixelFormat = GL_RGB;
		bytes = GL_RGB8;
		break;
	case mgImageType_r8g8b8a8:
		pixelFormat = GL_RGBA;
		break;
	case mgImageType_a8r8g8b8:
		pixelFormat = GL_RGBA;
		break;
	}

	mgTexture* newT = new mgTexture;
	
	BackendOpenGL_texture* hardwareData = new BackendOpenGL_texture;
	newT->implementation = hardwareData;

	glGenTextures(1, &hardwareData->textureID);
	//auto err = glGetError();
	glBindTexture(GL_TEXTURE_2D, hardwareData->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, bytes, img->width, img->height, 0, pixelFormat, GL_UNSIGNED_BYTE, img->data);
	glBindTexture(GL_TEXTURE_2D, 0);
	//img->data = 0;

	return newT;
}

void BackendOpenGL::DestroyTexture(mgTexture* t)
{
	BackendOpenGL_texture* hwdata = (BackendOpenGL_texture*)t->implementation;
	if (hwdata->textureID)
		glDeleteTextures(1, &hwdata->textureID);
	delete (hwdata);
	delete t;
}

void BackendOpenGL::_drawRectangle()
{
	glBegin(GL_TRIANGLES);

	glTexCoord2f(m_drrect_tcoords.z, m_drrect_tcoords.y);
	glColor4fv(&m_drrect_color1->r);
	glVertex3f(m_drrect_rect.right, m_drrect_rect.top, 0.0f);

	glTexCoord2f(m_drrect_tcoords.x, m_drrect_tcoords.w);
	glColor4fv(&m_drrect_color2->r);
	glVertex3f(m_drrect_rect.left, m_drrect_rect.bottom, 0.0f);

	glTexCoord2f(m_drrect_tcoords.z, m_drrect_tcoords.w);
	glVertex3f(m_drrect_rect.right, m_drrect_rect.bottom, 0.0f);

	glTexCoord2f(m_drrect_tcoords.z, m_drrect_tcoords.y);
	glColor4fv(&m_drrect_color1->r);
	glVertex3f(m_drrect_rect.right, m_drrect_rect.top, 0.0f);

	glTexCoord2f(m_drrect_tcoords.x, m_drrect_tcoords.y);
	glVertex3f(m_drrect_rect.left, m_drrect_rect.top, 0.0f);

	glTexCoord2f(m_drrect_tcoords.x, m_drrect_tcoords.w);
	glColor4fv(&m_drrect_color2->r);
	glVertex3f(m_drrect_rect.left, m_drrect_rect.bottom, 0.0f);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}
void BackendOpenGL::DrawRectangle(int reason, void* object, mgRect* rct, mgColor* color1, mgColor* color2,
	mgTexture* texture, mgVec4* UVRegion)
{
	m_drrect_color1 = color1;
	m_drrect_color2 = color2;

	m_drrect_rect.left = (float)(rct->left);
	m_drrect_rect.top = (float)(rct->top);
	m_drrect_rect.right = (float)(rct->right);
	m_drrect_rect.bottom = (float)(rct->bottom);

	m_drrect_tcoords.x = 0.f;
	m_drrect_tcoords.y = 0.f;
	m_drrect_tcoords.z = 1.f;
	m_drrect_tcoords.w = 1.f;
	if (texture)
	{
		BackendOpenGL_texture* timpl = (BackendOpenGL_texture*)texture->implementation;
		glBindTexture(GL_TEXTURE_2D, timpl->textureID);

		if (UVRegion)
			m_drrect_tcoords = *UVRegion;
	}

	bool draw = true;

	switch (reason)
	{
	case mgDrawRectangleReason_listItemBG1:
	case mgDrawRectangleReason_listItemBG2:
		break;
	default:
		if (reason == mgDrawRectangleReason_popupSeparator)
		{
			m_drrect_rect.top += 2;
			m_drrect_rect.bottom -= 2;
			m_drrect_rect.left += 2;
			m_drrect_rect.right -= 2;
		}

		_drawRectangle();

		if (reason == mgDrawRectangleReason_popupBG)
		{
			mgColor c;
			c.setAsIntegerRGB(0x666666);
			glBegin(GL_LINES);
			glColor4fv(&c.r);
			glVertex3f(m_drrect_rect.left, m_drrect_rect.top + 1, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.top + 1, 0.0f);
			glVertex3f(m_drrect_rect.left, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.left + 1, m_drrect_rect.top, 0.0f);
			glVertex3f(m_drrect_rect.left + 1, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.top, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.bottom, 0.0f);
			glEnd();
		}
		break;
	}
	glScissor(0, 0, m_currWindow->m_size.x, m_currWindow->m_size.y);
}

void BackendOpenGL::DrawText(int reason, void* object, mgPoint* position, const wchar_t* text, int textLen,
	mgColor* color, mgFont* font)
{
	mgPoint _position = *position;
	for (int i = 0; i < textLen; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			_position.x += glyph->underhang;
			mgVec4 corners;
			corners.x = (float)_position.x;
			corners.y = (float)_position.y;

			corners.z = corners.x + glyph->width;
			corners.w = corners.y + glyph->height;

			BackendOpenGL_texture* texture = (BackendOpenGL_texture*)((mgFontBitmap*)font->implementation)[glyph->textureSlot].gpuTexture->implementation;
			glBindTexture(GL_TEXTURE_2D, texture->textureID);

			glBegin(GL_TRIANGLES);
			glTexCoord2f(glyph->UV.z, glyph->UV.y);
			glColor4fv(&color->r);
			glVertex3f(corners.z, corners.y, 0.0f);

			glTexCoord2f(glyph->UV.x, glyph->UV.w);
			glVertex3f(corners.x, corners.w, 0.0f);

			glTexCoord2f(glyph->UV.z, glyph->UV.w);
			glVertex3f(corners.z, corners.w, 0.0f);

			glTexCoord2f(glyph->UV.z, glyph->UV.y);
			glVertex3f(corners.z, corners.y, 0.0f);

			glTexCoord2f(glyph->UV.x, glyph->UV.y);
			glVertex3f(corners.x, corners.y, 0.0f);

			glTexCoord2f(glyph->UV.x, glyph->UV.w);
			glVertex3f(corners.x, corners.w, 0.0f);
			glEnd();

			_position.x += glyph->width + glyph->overhang + font->characterSpacing;
			if (character == L' ')
				_position.x += font->spaceSize;
			if (character == L'\t')
				_position.x += font->tabSize;
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	mgVec4 tcoords(0.f, 0.f, 1.f, 1.f);
	/*if (texture)
	{
		BackendOpenGL_texture* timpl = (BackendOpenGL_texture*)texture->implementation;
		glBindTexture(GL_TEXTURE_2D, timpl->textureID);

		if (UVRegion)
			tcoords = *UVRegion;
	}*/

	
	/*SelectObject(m_currWindow->m_hdcMem, font->implementation);
	SetTextColor(m_currWindow->m_hdcMem, mgColorGetAsIntegerBGR(color));
	SetBkMode(m_currWindow->m_hdcMem, TRANSPARENT);

	HRGN rgn = CreateRectRgn(
		m_clipRect.left + m_currWindow->m_borderSize.x,
		m_clipRect.top + m_currWindow->m_borderSize.y,
		m_clipRect.right + m_currWindow->m_borderSize.x,
		m_clipRect.bottom + m_currWindow->m_borderSize.y);

	SelectClipRgn(m_currWindow->m_hdcMem, rgn);
	TextOutW(m_currWindow->m_hdcMem, 
		position->x + m_currWindow->m_borderSize.x, 
		position->y + m_currWindow->m_borderSize.y, 
		text, textLen);
	DeleteObject(rgn);
	SelectClipRgn(m_currWindow->m_hdcMem, 0);*/

	glScissor(0,0,m_currWindow->m_size.x,m_currWindow->m_size.y);
}

void BackendOpenGL::DrawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	/*Gdiplus::Graphics graphics(m_currWindow->m_hdcMem);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);*/
}

mgRect BackendOpenGL::SetClipRect(mgRect* r)
{
	mgRect old = m_clipRect;
	m_clipRect = *r;
	int y = m_currWindow->m_size.y - m_clipRect.top;
	int w = m_clipRect.bottom - m_clipRect.top;
	glScissor(
		m_clipRect.left, 
		y - w,
		m_clipRect.right - m_clipRect.left, 
		w);
	return old;
}


void BackendOpenGL::_createBackbuffer(mgf::SystemWindow* impl)
{
}

void BackendOpenGL::InitWindow(mgf::SystemWindow* w)
{
	if (m_params.withInit)
	{
		HDC dc = GetDC(m_currWindow->m_OSData->hWnd);
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
			1,                     // version number  
			PFD_DRAW_TO_WINDOW |   // support window  
			PFD_SUPPORT_OPENGL |   // support OpenGL  
			PFD_DOUBLEBUFFER,      // double buffered  
			PFD_TYPE_RGBA,         // RGBA type  
			24,                    // 24-bit color depth  
			0, 0, 0, 0, 0, 0,      // color bits ignored  
			0,                     // no alpha buffer  
			0,                     // shift bit ignored  
			0,                     // no accumulation buffer  
			0, 0, 0, 0,            // accum bits ignored  
			32,                    // 32-bit z-buffer  
			0,                     // no stencil buffer  
			0,                     // no auxiliary buffer  
			PFD_MAIN_PLANE,        // main layer  
			0,                     // reserved  
			0, 0, 0                // layer masks ignored  
		};
		int iPixelFormat = ChoosePixelFormat(dc, &pfd);
		BOOL r = SetPixelFormat(dc, iPixelFormat, &pfd);
		ReleaseDC(m_currWindow->m_OSData->hWnd, dc);
	}

	if (m_params.withInit && !m_params.hglrc)
	{
		HDC dc = GetDC(m_currWindow->m_OSData->hWnd);
		m_params.hglrc = wglCreateContext(dc);
		wglMakeCurrent(dc, m_params.hglrc);

		/*oldgl_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
		oldgl_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		oldgl_wglSwapIntervalEXT(1);*/
		ReleaseDC(m_currWindow->m_OSData->hWnd, dc);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glEnable(GL_TEXTURE_2D);
	}

	_createBackbuffer(m_currWindow);
}

mgf::SystemWindow* BackendOpenGL::SetCurrentWindow(mgf::SystemWindow* w)
{
	mgf::SystemWindow* prev = m_currWindow;
	m_currWindow = w;
	return prev;
}

void BackendOpenGL::SetActiveContext(mgf::Context* c)
{
	m_context = c;
}

void BackendOpenGL::UpdateBackBuffer()
{
	/*if (m_currWindow->m_hdcMem)
		DeleteDC(m_currWindow->m_hdcMem);
	if (m_currWindow->m_hbmMem)
		DeleteObject(m_currWindow->m_hbmMem);*/
	
	glViewport(0, 0, m_currWindow->m_size.x, m_currWindow->m_size.y);

	_createBackbuffer(m_currWindow);
}

void BackendOpenGL::DeleteBackBuffer()
{

}

void BackendOpenGL::GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	sz->x = 0;
	sz->y = 0;
	int c = (int)wcslen(text);
	if (!c)
		return;
	for (int i = 0; i < c; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			if (glyph->height > sz->y)
				sz->y = glyph->height;

			sz->x += glyph->width + glyph->overhang + glyph->underhang + font->characterSpacing;
			if (character == L' ')
				sz->x += font->spaceSize;
			if (character == L'\t')
				sz->x += font->tabSize;
		}
	}
}

Font* BackendOpenGL::CreateFont(const wchar_t* file, int size, bool bold, bool italic)
{
	StringW str = file;
	StringA stra = str.to_utf8();

	int flags = 0;

	if (bold)
		flags |= MG_FNTFL_BOLD;
	if (italic)
		flags |= MG_FNTFL_ITALIC;

	FontImpl* newFont = new FontImpl;
	newFont->m_context = this->m_context;

	newFont->m_font = mgCreateFont(this->m_context->m_gui_context, stra.data(), flags, size, 0);

	newFont->m_backend = this;
	return newFont;
}

void BackendOpenGL::DestroyFont(Font* f)
{
	if (!f)
		return;
	FontImpl* fi = (FontImpl*)f;
	if (fi->m_font)
	{
		if (fi->m_font->implementation)
			DeleteObject(fi->m_font->implementation);
		free(fi->m_font);
	}
}

Font* BackendOpenGL::GetDefaultFont()
{
	if(!m_defaultFont)
		m_defaultFont = this->CreateFont(L"Arial", 10, 1, 0);

	return m_defaultFont;
}

#endif
