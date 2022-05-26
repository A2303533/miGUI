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
#ifdef MGF_BACKEND_GDI

#include "framework/Context.h"
#include "framework/SystemWindow.h"
#include "framework/BackendGDI.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"
#include "framework/Icons.h"
#include "framework/Image.h"
#include "framework/Archive.h"

#include "data/icons.h"

#ifdef MG_PLATFORM_WINDOWS

#include <Windows.h>
#define GDIPVER 0x0110
#include <gdiplus.h>
#include "framework/OS/UndefWindows.h"

#include <filesystem>
#include "framework/String.h"

using namespace mgf;

extern mgf::Backend* g_backend;
extern Framework* g_mgf;


void BackendGDI_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	g_backend->GetTextSize(text, font, sz);
}

mgTexture* BackendGDI_createTexture(mgImage* img)
{
	return g_backend->CreateTexture(img);
}

void BackendGDI_destroyTexture(mgTexture* t)
{
	g_backend->DestroyTexture(t);
}

void BackendGDI_beginDraw(int reason)
{
	g_backend->BeginDraw(reason);
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
	mgTexture* texture, /*optional*/
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

void BackendGDI_drawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	g_backend->DrawLine(reason, object, position, where, color, size);
}

mgRect BackendGDI_setClipRect(mgRect* r)
{
	return g_backend->SetClipRect(r);
}

mgSystemWindowOSData* BackendGDI_setCurrentWindow(mgSystemWindowOSData* data)
{
	if (data->userData)
	{
		mgf::SystemWindow* w = g_backend->SetCurrentWindow((mgf::SystemWindow*)data->userData);
		if (w) return w->GetOSData();
		else return 0;
	}
	else
	{
		static mgf::SystemWindow tmpWnd;
		tmpWnd.SetOSData(data);

		mgf::SystemWindow* w = g_backend->SetCurrentWindow(&tmpWnd);
		if (w) return w->GetOSData();
		else return 0;
	}
}

void BackendGDI_updateBackBuffer() 
{
	g_backend->UpdateBackBuffer();
}



//void BackendGDI::UpdateBackBuffer()
//{
//	DeleteBackBuffer();
//
//	m_currWindow
//	HWND hWnd = (HWND)g_currSystemWindowOSData->hWnd;
//
//	HDC dc = GetWindowDC(hWnd);
//	g_currSystemWindowOSData->hdcMem = CreateCompatibleDC(dc);
//
//	RECT windowRect;
//	GetWindowRect(hWnd, &windowRect);
//
//	g_currSystemWindowOSData->position.x = windowRect.left;
//	g_currSystemWindowOSData->position.y = windowRect.top;
//	g_currSystemWindowOSData->size.x = windowRect.right - windowRect.left;
//	g_currSystemWindowOSData->size.y = windowRect.bottom - windowRect.top;
//
//	g_currSystemWindowOSData->hbmMem = CreateCompatibleBitmap(dc,
//		g_currSystemWindowOSData->size.x,
//		g_currSystemWindowOSData->size.y);
//
//	ReleaseDC(hWnd, dc);
//}

BackendGDI::BackendGDI()
{
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

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
	((mgVideoDriverAPI*)m_gpu)->setCurrentWindow = BackendGDI_setCurrentWindow;
	((mgVideoDriverAPI*)m_gpu)->updateBackBuffer = BackendGDI_updateBackBuffer;

	blackImage = new Image;
	blackImage->Create(20, 20, mgColor(0xff020202));
	blackBitmap = new Gdiplus::Bitmap(blackImage->GetWidth(), blackImage->GetHeight(), blackImage->GetPitch(), 
		PixelFormat32bppARGB, blackImage->GetData());
}

BackendGDI::~BackendGDI()
{
	if (blackBitmap)
		delete blackBitmap;
	if (blackImage)
		delete blackImage;

	if (m_defaultFont)
		m_defaultFont->Release();

	if (m_defaultIcons)
	{
		if(m_defaultIcons->implementation)
			delete ((Gdiplus::Bitmap*)m_defaultIcons->implementation);
		delete m_defaultIcons;
	}

	if(m_gdiplusToken)
		Gdiplus::GdiplusShutdown(m_gdiplusToken);

	delete m_gpu;
}

mgTexture_s* BackendGDI::GetDefaultIcons()
{

	if (m_defaultIcons)
		return m_defaultIcons;

	//m_defaultIcons = new mgTexture_s;
	//m_defaultIcons->implementation = new Gdiplus::Bitmap(L"../data/icons.png");
	
	//g_mgf->LoadImage("../data/icons.png");
	Image* image = 0;// g_mgf->LoadImage("../data/icons.png");

	mgf::ArchiveSystem archive;
	CompressionInfo cmpInf = _data_icons_cmpInf;
	std::vector<uint8_t> uncompressedData;
	if (archive.Decompress(&cmpInf, uncompressedData))
		image = g_mgf->LoadImage(uncompressedData.data(), uncompressedData.size(), ImageLoader::png);

	if (image)
	{
		m_defaultIcons = this->CreateTexture(image->GetMGImage());
		image->Release();
	}

	return m_defaultIcons;
}

void* BackendGDI::GetVideoDriverAPI()
{
	return m_gpu;
}

void BackendGDI::BeginDraw(int reason)
{
	m_currWindow->m_OSData->hbmOld = (HBITMAP)SelectObject(m_currWindow->m_OSData->hdcMem, m_currWindow->m_OSData->hbmMem);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = m_currWindow->m_OSData->size.x;
	r.bottom = m_currWindow->m_OSData->size.y;


	switch (reason)
	{
	default:
		m_currWindow->m_borderSizeCurrent = m_currWindow->m_borderSize;
		FillRect(m_currWindow->m_OSData->hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 1));
		break;
	case mgBeginDrawReason_popupWindow:
		m_currWindow->m_borderSizeCurrent.x = 0;
		m_currWindow->m_borderSizeCurrent.y = 0;
		FillRect(m_currWindow->m_OSData->hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 3));
		break;
	}
}

void BackendGDI::EndDraw()
{
	HDC dc = GetWindowDC(m_currWindow->m_OSData->hWnd);
	BitBlt(dc,
		m_currWindow->m_borderSizeCurrent.x + m_endDrawIndent.x,
		m_currWindow->m_borderSizeCurrent.y + m_endDrawIndent.y,
		m_currWindow->m_OSData->size.x,
		m_currWindow->m_OSData->size.y,
		m_currWindow->m_OSData->hdcMem,
		0, 0,
		SRCCOPY);
	ReleaseDC(m_currWindow->m_OSData->hWnd, dc);
}

mgTexture* BackendGDI::CreateTexture(mgImage* img)
{
	if (img->type == mgImageType_unknown)
		return 0;

	unsigned int pixelFormat = 0;
	switch (img->type)
	{
	case mgImageType_r8g8b8:
		pixelFormat = PixelFormat24bppRGB;
		break;
	case mgImageType_r8g8b8a8:
	{
		mgf::Image mgfImage(img);
		mgfImage.Convert(mgImageType_a8r8g8b8);
		pixelFormat = PixelFormat32bppARGB;
		mgfImage.Drop();
	}break;
	case mgImageType_a8r8g8b8:
		pixelFormat = PixelFormat32bppARGB;
		break;
	}
	
	mgTexture* newT = new mgTexture;
	newT->sourceCopy = new mgImage;
	*newT->sourceCopy = *img;
	newT->sourceCopy->data = (uint8_t*)malloc(img->dataSize);
	memcpy(newT->sourceCopy->data, img->data, img->dataSize);

	Gdiplus::Bitmap* myImage = new Gdiplus::Bitmap(newT->sourceCopy->width, newT->sourceCopy->height, newT->sourceCopy->pitch, pixelFormat, newT->sourceCopy->data);

	newT->implementation = myImage;
	
	//img->data = 0; // WHY? probably somewhere img will be deleted. GDI need to know data so I save it.
	               // probably need to remove this code.
	// solution probably this: create a copy of img

	return newT;
}

void BackendGDI::DestroyTexture(mgTexture* t)
{
	if(t->implementation)
		delete ((Gdiplus::Bitmap*)t->implementation);

	if (t->sourceCopy)
		delete t->sourceCopy;

	delete t;
}

void BackendGDI::DrawRectangle(int reason, void* object, mgRect* rct, mgColor* color1, mgColor* color2,
	mgTexture* texture, mgVec4* UVRegion)
{
	unsigned int c1 = mgColorGetAsIntegerARGB(color1);
	unsigned int c2 = mgColorGetAsIntegerARGB(color2);
	HBRUSH brsh = CreateSolidBrush(RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
	SelectObject(m_currWindow->m_OSData->hdcMem, brsh);


	RECT r;
	r.left = rct->left;
	r.top = rct->top;
	r.right = rct->right;
	r.bottom = rct->bottom;

	HRGN rgn = 0;

	switch (reason)
	{
	default:
	{
		if (reason == mgDrawRectangleReason_popupSeparator)
		{
			r.top += 2;
			r.bottom -= 2;
			r.left += 2;
			r.right -= 2;
		}
		rgn = CreateRectRgn(
			m_clipRect.left,
			m_clipRect.top,
			m_clipRect.right,
			m_clipRect.bottom);
		SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
		FillRect(m_currWindow->m_OSData->hdcMem, &r, brsh);
	}break;
	case mgDrawRectangleReason_listItemBG1:
	case mgDrawRectangleReason_listItemBG2:
		break;
	case mgDrawRectangleReason_tableCellBG:
	case mgDrawRectangleReason_tableColTitleColBG:
	{
		rgn = CreateRectRgn(
			m_clipRect.left,
			m_clipRect.top,
			m_clipRect.right,
			m_clipRect.bottom);
		SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
		MoveToEx(m_currWindow->m_OSData->hdcMem, r.right - 1, r.top, 0);
		HPEN p = CreatePen(0, 2, RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
		SelectObject(m_currWindow->m_OSData->hdcMem, p);
		LineTo(m_currWindow->m_OSData->hdcMem, r.right - 1, r.bottom);
		DeleteObject(p);
	}break;
	case mgDrawRectangleReason_tableRowBG1:
	case mgDrawRectangleReason_tableRowBG2:
	{
		rgn = CreateRectRgn(
			m_clipRect.left,
			m_clipRect.top,
			m_clipRect.right,
			m_clipRect.bottom);
		SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
		MoveToEx(m_currWindow->m_OSData->hdcMem, r.left, r.bottom, 0);
		HPEN p = CreatePen(0, 2, RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
		SelectObject(m_currWindow->m_OSData->hdcMem, p);
		LineTo(m_currWindow->m_OSData->hdcMem, r.right, r.bottom);
		DeleteObject(p);
	}break;
	case mgDrawRectangleReason_popupBG:
	{
		{
			Gdiplus::Rect gdirct;
			gdirct.X = rct->left;
			gdirct.Y = rct->top;
			gdirct.Width = rct->right - rct->left + 5;
			gdirct.Height = rct->bottom - rct->top + 4;

			Gdiplus::BlurParams blurParams;
			blurParams.radius = 2;
			blurParams.expandEdge = 0;
			Gdiplus::Blur blur;
			blur.SetParameters(&blurParams);
			RECT rectOfInterest = { -1, -1, 21, 21 };

			Gdiplus::Bitmap* outputBitmapBlur = NULL;
			Gdiplus::Bitmap::ApplyEffect(&blackBitmap, 1, &blur, &rectOfInterest, 0, &outputBitmapBlur);

			Gdiplus::BrightnessContrastParams brtParams;
			brtParams.brightnessLevel = 100;
			brtParams.contrastLevel = 1;
			Gdiplus::BrightnessContrast brt;
			brt.SetParameters(&brtParams);
			Gdiplus::Bitmap* outputBitmapBlurAndBrt = NULL;
			Gdiplus::Bitmap::ApplyEffect(&outputBitmapBlur, 1, &brt, &rectOfInterest, 0, &outputBitmapBlurAndBrt);

			Gdiplus::Graphics graphics(m_currWindow->m_OSData->hdcMem);
			Gdiplus::Status status = graphics.DrawImage(outputBitmapBlurAndBrt, gdirct,
				-1, -1, 21, 21, Gdiplus::UnitPixel);
			delete outputBitmapBlur;
			delete outputBitmapBlurAndBrt;

		}
		
		rgn = CreateRectRgn(
				m_clipRect.left,
				m_clipRect.top,
				m_clipRect.right,
				m_clipRect.bottom);
		SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
		FillRect(m_currWindow->m_OSData->hdcMem, &r, brsh);

		if (reason == mgDrawRectangleReason_popupBG)
		{
			HPEN p = CreatePen(0, 3, 0xf4f4f4);
			SelectObject(m_currWindow->m_OSData->hdcMem, p);
			MoveToEx(m_currWindow->m_OSData->hdcMem, r.left, r.top, 0);
			LineTo(m_currWindow->m_OSData->hdcMem, r.right, r.top);
			MoveToEx(m_currWindow->m_OSData->hdcMem, r.left, r.top, 0);
			LineTo(m_currWindow->m_OSData->hdcMem, r.left, r.bottom);
			DeleteObject(p);
		}
	}break;
	case mgDrawRectangleReason_windowCloseButton:
	case mgDrawRectangleReason_windowCollapseButton:
	case mgDrawRectangleReason_buttonIcon:
	case mgDrawRectangleReason_popupNextIcon:
	case mgDrawRectangleReason_popupCheckIcon:
	{
		if (texture)
		{
			Gdiplus::Rect gdirct;
			gdirct.X = rct->left;
			gdirct.Y = rct->top;
			gdirct.Width = rct->right - rct->left;
			gdirct.Height = rct->bottom - rct->top;
			
			Gdiplus::Graphics graphics(m_currWindow->m_OSData->hdcMem);

			{
				Gdiplus::Bitmap* bmp = (Gdiplus::Bitmap*)texture->implementation;
				
				Gdiplus::Rect clrct;
				clrct.X = 0;
				clrct.Y = 0;
				clrct.Width = bmp->GetWidth();
				clrct.Height = bmp->GetHeight();

				Gdiplus::Bitmap* bitmapCopy = ((Gdiplus::Bitmap*)texture->implementation)->Clone(clrct, bmp->GetPixelFormat());
				
				Gdiplus::ImageAttributes  imageAttributes;
				Gdiplus::ColorMatrix colorMatrix = {
				   2.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				   0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				   0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				   color1->r, color1->g, color1->b, 0.0f, 1.0f };
				imageAttributes.SetColorMatrix(
					&colorMatrix,
					Gdiplus::ColorMatrixFlagsDefault,
					Gdiplus::ColorAdjustTypeBitmap);

				Gdiplus::Status status = graphics.DrawImage(bitmapCopy, gdirct,
					m_context->m_gui_context->currentIcon->lt.x,
					m_context->m_gui_context->currentIcon->lt.y,
					m_context->m_gui_context->currentIcon->sz.x,
					m_context->m_gui_context->currentIcon->sz.y,
					Gdiplus::UnitPixel, &imageAttributes);

				delete bitmapCopy;
			}
		}
	}break;
	case mgDrawRectangleReason_rectangleWithTexture:
	{
		if (texture)
		{
			Gdiplus::Rect gdirct;
			gdirct.X = rct->left;
			gdirct.Y = rct->top;
			gdirct.Width = rct->right - rct->left;
			gdirct.Height = rct->bottom - rct->top;

			Gdiplus::Graphics graphics(m_currWindow->m_OSData->hdcMem);

			rgn = CreateRectRgn(
				m_clipRect.left,
				m_clipRect.top,
				m_clipRect.right,
				m_clipRect.bottom);
			graphics.SetClip(rgn);

			{
				Gdiplus::Bitmap* bmp = (Gdiplus::Bitmap*)texture->implementation;

				Gdiplus::Rect clrct;
				clrct.X = 0;
				clrct.Y = 0;
				clrct.Width = bmp->GetWidth();
				clrct.Height = bmp->GetHeight();

				Gdiplus::Bitmap* bitmap = ((Gdiplus::Bitmap*)texture->implementation);

				Gdiplus::Status status = graphics.DrawImage(bitmap, gdirct,
					0,
					0,
					clrct.Width,
					clrct.Height,
					Gdiplus::UnitPixel);
			}
		}
	}break;
	case mgDrawRectangleReason_windowTitlebar:
	case mgDrawRectangleReason_windowBG:
	{
		int roundRectBtm = r.bottom + 1;
		if (reason == mgDrawRectangleReason_windowTitlebar)
			roundRectBtm += 7;

		rgn = CreateRoundRectRgn(r.left, r.top, r.right + 1, roundRectBtm, 7, 7);

		SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
		{
			TRIVERTEX vertex[2];
			vertex[0].x = r.left;
			vertex[0].y = r.top;
			vertex[0].Red = (c1 & 0x000000ff) << 8;
			vertex[0].Green = (c1 & 0x0000ff00);
			vertex[0].Blue = (c1 & 0x00ff0000) >> 8;
			vertex[0].Alpha = (c1 & 0xff000000) >> 16;

			vertex[1].x = r.right;
			vertex[1].y = r.bottom;
			vertex[1].Red = (c2 & 0x000000ff) << 8;
			vertex[1].Green = (c2 & 0x0000ff00);
			vertex[1].Blue = (c2 & 0x00ff0000) >> 8;
			vertex[1].Alpha = (c2 & 0xff000000) >> 16;

			GRADIENT_RECT gRect;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			GradientFill(m_currWindow->m_OSData->hdcMem, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
		}
	}break;
	}

	DeleteObject(rgn);
	DeleteObject(brsh);
	SelectClipRgn(m_currWindow->m_OSData->hdcMem, 0);
}

void BackendGDI::DrawText(int reason, void* object, mgPoint* position, const wchar_t* text, int textLen,
	mgColor* color, mgFont* font)
{
	SelectObject(m_currWindow->m_OSData->hdcMem, font->implementation);
	SetTextColor(m_currWindow->m_OSData->hdcMem, mgColorGetAsIntegerBGR(color));
	SetBkMode(m_currWindow->m_OSData->hdcMem, TRANSPARENT);

	HRGN rgn = CreateRectRgn(
		m_clipRect.left,
		m_clipRect.top,
		m_clipRect.right,
		m_clipRect.bottom);

	SelectClipRgn(m_currWindow->m_OSData->hdcMem, rgn);
	TextOutW(m_currWindow->m_OSData->hdcMem,
		position->x, 
		position->y, 
		text, textLen);
	DeleteObject(rgn);
	SelectClipRgn(m_currWindow->m_OSData->hdcMem, 0);
}

void BackendGDI::DrawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	Gdiplus::Graphics graphics(m_currWindow->m_OSData->hdcMem);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);
}

mgRect BackendGDI::SetClipRect(mgRect* r)
{
	mgRect old = m_clipRect;
	m_clipRect = *r;
	return old;
}

void BackendGDI::DeleteBackBuffer()
{

}

void BackendGDI::_createBackbuffer()
{
	HDC dc = GetWindowDC(m_currWindow->m_OSData->hWnd);
	m_currWindow->m_OSData->hdcMem = CreateCompatibleDC(dc);
	RECT r;
	GetWindowRect(m_currWindow->m_OSData->hWnd, &r);
	m_currWindow->m_OSData->hbmMem = CreateCompatibleBitmap(dc,
		r.right - r.left,
		r.bottom - r.top);
	ReleaseDC(m_currWindow->m_OSData->hWnd, dc);
}

void BackendGDI::InitWindow(mgf::SystemWindow* w)
{
	if (m_currWindow->m_OSData->hdcMem)
		return;
	_createBackbuffer();
}

mgf::SystemWindow* BackendGDI::SetCurrentWindow(mgf::SystemWindow* w)
{
	mgf::SystemWindow* prev = m_currWindow;
	m_currWindow = w;
	return prev;
}

void BackendGDI::SetActiveContext(mgf::Context* c)
{
	m_context = c;
}

void BackendGDI::UpdateBackBuffer()
{
	if (m_currWindow->m_OSData->hdcMem)
		DeleteDC(m_currWindow->m_OSData->hdcMem);
	if (m_currWindow->m_OSData->hbmMem)
		DeleteObject(m_currWindow->m_OSData->hbmMem);
	
	_createBackbuffer();
}

void BackendGDI::GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	SelectObject(m_currWindow->m_OSData->hdcMem, font->implementation);
	size_t c = wcslen(text);
	if (!c)
		return;
	SIZE s;
	GetTextExtentPoint32W(m_currWindow->m_OSData->hdcMem, text, (int)c, &s);
	sz->x = s.cx;
	sz->y = s.cy;
}

Font* BackendGDI::CreateFontPrivate(const wchar_t* file, int size, bool bold, bool italic, const wchar_t* name)
{
	FontImpl* newFont = 0;

	StringW strw = file;
	StringA stra = strw.to_utf8();

	std::filesystem::path filePath = file;
	if (std::filesystem::is_regular_file(filePath))
	{
		int result = AddFontResourceExW(file, FR_PRIVATE, 0);
		if (result)
		{
			newFont = (FontImpl*)CreateFont(name, size, bold, italic);
			newFont->m_private = file;
		}
	}

	return newFont;
}

Font* BackendGDI::CreateFont(const wchar_t* file, int size, bool bold, bool italic)
{
	FontImpl* newFont = new FontImpl;
	newFont->m_context = this->m_context;
	{
		mgFont* f = (mgFont*)calloc(1, sizeof(mgFont));
		HDC g_dc = GetWindowDC(m_currWindow->m_OSData->hWnd);
		f->implementation = CreateFontW(
			-MulDiv(size, GetDeviceCaps(g_dc, LOGPIXELSY), 72),
			0, 0, 0,
			bold ? FW_BOLD : FW_NORMAL,
			italic ? 1 : 0,
			0,
			0,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			VARIABLE_PITCH,
			file);
		
		SelectObject(m_currWindow->m_OSData->hdcMem, f->implementation);

#ifdef _UNICODE
		TEXTMETRICW tm;
		GetTextMetrics(m_currWindow->m_OSData->hdcMem, &tm);
		f->maxSize.x = tm.tmMaxCharWidth;
		f->maxSize.y = tm.tmHeight;
#else
		TEXTMETRICA tm;
		GetTextMetrics(m_currWindow->m_OSData->hdcMem, &tm);
		f->maxSize.x = tm.tmMaxCharWidth;
		f->maxSize.y = tm.tmHeight;
#endif

		ReleaseDC(m_currWindow->m_OSData->hWnd, g_dc);
		newFont->m_font = f;
	}
	newFont->m_backend = this;
	return newFont;
}

void BackendGDI::DestroyFont(Font* f)
{
	if (!f)
		return;
	FontImpl* fi = (FontImpl*)f;

	if (fi->m_private.size())
	{
		RemoveFontResourceExW(fi->m_private.data(), FR_PRIVATE, 0);
	}

	if (fi->m_font)
	{
		if (fi->m_font->implementation)
			DeleteObject(fi->m_font->implementation);
		free(fi->m_font);
	}
}

Font* BackendGDI::GetDefaultFont()
{
	if(!m_defaultFont)
		m_defaultFont = this->CreateFont(L"Arial", 10, 1, 0);

	return m_defaultFont;
}

void BackendGDI::SetIcon(mgf::Icons* ic, int id)
{
	m_context->m_gui_context->currentIcon = &ic->GetMGIcons()->iconNodes[id];
}

void BackendGDI::UpdateTexture(mgTexture* t, mgImage* img)
{
	Gdiplus::Bitmap* gdiImage = (Gdiplus::Bitmap*)t->implementation;
	//memcpy(t->sourceCopy->data, img->data, img->dataSize);

	ImagePixelFormat_RGBA8* pixelRGBA8_dst = (ImagePixelFormat_RGBA8*)t->sourceCopy->data;
	ImagePixelFormat_RGBA8* pixelRGBA8_src = (ImagePixelFormat_RGBA8*)img->data;

	uint32_t wh = img->height * img->width;
	for (uint32_t i = 0; i < wh; ++i)
	{
		pixelRGBA8_dst->r = pixelRGBA8_src->b;
		pixelRGBA8_dst->g = pixelRGBA8_src->g;
		pixelRGBA8_dst->b = pixelRGBA8_src->r;
		pixelRGBA8_dst->a = pixelRGBA8_src->a;
		++pixelRGBA8_src;
		++pixelRGBA8_dst;
	}
}

#endif
#endif
