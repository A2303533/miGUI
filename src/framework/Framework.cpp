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
#include "miGUILoader.h"

#include "framework/mgf.h"
#include "framework/Framework.h"
#include "framework/Context.h"
#include "framework/SystemWindow.h"
#include "framework/Icons.h"
#include "framework/Popup.h"
#include "framework/FontImpl.h"
#include "framework/Log.h"
#include "framework/List.h"

//#include "curl/curl.h"

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
#include <Objbase.h>
HRESULT g_CoInitializeResult = 0;
#pragma comment(lib, "Crypt32.lib")
#endif

#ifdef MGF_OPENSSL
MGF_LINK_LIBRARY("OpenSSL")
#endif

//MGF_LINK_LIBRARY("curl")
//MGF_LINK_LIBRARY("libssh2")
MGF_LINK_LIBRARY("brotli")
MGF_LINK_LIBRARY("zlib")
MGF_LINK_LIBRARY("zstd")
//MGF_LINK_LIBRARY("cares")
//MGF_LINK_LIBRARY("libidn2")
//MGF_LINK_LIBRARY("nghttp2")
//MGF_LINK_LIBRARY("nghttp3")

using namespace mgf;

Backend* g_backend = 0;
Framework* g_mgf = 0;

#ifndef MG_NO_DLL
MG_LIB_HANDLE g_migui_dll = 0;
#endif

Framework* mgf::InitFramework()
{
	mgf::LogWriteInfo("%s: init framework\n", MGF_FUNCTION);

#ifndef MG_NO_DLL
	g_migui_dll = mgLoad();
	if (!g_migui_dll)
	{
		throw "Can't load migui.dll";
	}
#endif
	g_mgf = new Framework();
	return g_mgf;
}

Framework* mgf::GetFramework()
{
	return g_mgf;
}

Framework::Framework()
	:
	m_run(true)
{
	_initColors();
#ifdef MG_PLATFORM_WINDOWS
	wchar_t wcharBuffer[0xfff];
	GetCurrentDirectory(0xfff, wcharBuffer);
	m_appDirectory = wcharBuffer;
	m_appDirectory.flip_slash();
	m_appDirectory += L"/";

	//g_CoInitializeResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	g_CoInitializeResult = OleInitialize(0);
	if (FAILED(g_CoInitializeResult))
	{
		mgf::LogWriteError("%s: unable to initialize COM\n", MGF_FUNCTION);
	}
#endif

	//InitCURL();
}

Framework::~Framework()
{
	/*if (m_isCURLReady)
	{
		mgf::LogWriteInfo("%s: shutdown curl\n", MGF_FUNCTION);
		curl_global_cleanup();
	}*/

#ifdef MG_PLATFORM_WINDOWS
	if(g_CoInitializeResult == S_OK
		|| g_CoInitializeResult == S_FALSE)
		OleUninitialize();
		//CoUninitialize();
#endif

#ifndef MG_NO_DLL
	if(g_migui_dll)
		mgUnload(g_migui_dll);
#endif
}

StringW* Framework::GetAppDir()
{
	return &m_appDirectory;
}

#define FROM_ADDR    "<admin@mail.my>"
#define TO_ADDR      "<artembasov@outlook.com>"
#define CC_ADDR      "<artembasov@outlook.com>"
#define FROM_MAIL "Sender Person " FROM_ADDR
#define TO_MAIL   "A Receiver " TO_ADDR
#define CC_MAIL   "John CC Smith " CC_ADDR
static const char* payload_text =
"Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
"To: " TO_MAIL "\r\n"
"From: " FROM_MAIL "\r\n"
"Cc: " CC_MAIL "\r\n"
"Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
"rfcpedant.example.org>\r\n"
"Subject: SMTP example message\r\n"
"\r\n" /* empty line to divide headers from body, see RFC5322 */
"The body of the message starts here.\r\n"
"\r\n"
"It could be a lot of lines, could be MIME encoded, whatever.\r\n"
"Check RFC5322.\r\n";

struct upload_status {
	size_t bytes_read;
};

static size_t payload_source(char* ptr, size_t size, size_t nmemb, void* userp)
{
	struct upload_status* upload_ctx = (struct upload_status*)userp;
	const char* data;
	size_t room = size * nmemb;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}
	
	data = &payload_text[upload_ctx->bytes_read];

	if (data) {
		size_t len = strlen(data);
		if (room < len)
			len = room;
		memcpy(ptr, data, len);
		upload_ctx->bytes_read += len;

		return len;
	}

	return 0;
}

//void Framework::InitCURL()
//{
//	if (!m_isCURLReady)
//	{
//		if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK)
//		{
//			m_isCURLReady = 1;
//			curl_version_info_data* vi = curl_version_info(CURLVERSION_FIRST);
//			mgf::LogWriteInfo("curl version %s (%u)\n", vi->version, vi->version_num);
//			mgf::LogWriteInfo("libssh version %s\n", vi->libssh_version);
//			mgf::LogWriteInfo("OpenSSL version %s (%u)\n", vi->ssl_version, vi->ssl_version_num);
//			mgf::LogWriteInfo("Brotli version %s (%u)\n", vi->brotli_version, vi->brotli_ver_num);
//			mgf::LogWriteInfo("zlib version %s\n", vi->libz_version);
//			mgf::LogWriteInfo("Zstandard version %s (%u)\n", vi->zstd_version, vi->zstd_ver_num);
//			mgf::LogWriteInfo("c-ares version %s (%u)\n", vi->ares, vi->ares_num);
//			mgf::LogWriteInfo("libidn2 version %s \n", vi->libidn);
//			mgf::LogWriteInfo("nghttp2 version %s (%u)\n", vi->nghttp2_version, vi->nghttp2_ver_num);
//			//mgf::LogWriteInfo("quic version %s\n", vi->quic_version);
//
//			
//		}
//		else
//			mgf::LogWriteError("%s: can't init curl\n", MGF_FUNCTION);
//	}
//}

bool Framework::Run()
{
	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		Context* c = m_contexts[i];
		mgStartFrame(c->GetGUIContext());
	}

#ifdef MG_PLATFORM_WINDOWS
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif

	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		Context* c = m_contexts[i];
		if(c->m_window->m_isVisible)
			mgUpdate(c->m_gui_context);
	}

	return m_run;
}

void Framework::DrawAll()
{
	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		Context* c = m_contexts[i];
		if (!c->m_window->m_isVisible)
			continue;

		c->DrawAll();
	}
}

Context* Framework::CreateContext(mgf::SystemWindow* sw, Backend* backend)
{
	Context* c = new Context(sw, backend);
	m_contexts.push_back(c);
	return c;
}

mgStyle_s Framework::GetNewStyle(int isLightTheme)
{
	mgStyle_s style;
	mgInitStyleLight(&style);
	return style;
}

Image* Framework::LoadImage(const wchar_t* imageFile)
{
	StringW strw = imageFile;
	StringA stra = strw.to_utf8();
	return LoadImage(stra.data());
}

Image* Framework::LoadImage(const uint8_t* buffer, uint32_t bufferSize, ImageLoader il)
{
	switch (il)
	{
	case mgf::ImageLoader::bmp:
		return Image_bmp_buf(buffer, bufferSize);
	case mgf::ImageLoader::png:
		return Image_png_buf(buffer, bufferSize);
	}
	return NULL;
}

Image* Framework::LoadImage(const char* imageFile)
{
	assert(imageFile);

	size_t slen = strlen(imageFile);
	if (!slen || slen > 1999)
		return 0;

	char buffer[2000];
	for (size_t i = 0; i < slen; ++i)
	{
		buffer[i] = tolower(imageFile[i]);
	}
	buffer[slen] = 0;

	const char* pch = strrchr(buffer, '.');

	if (!pch)
		return 0;

	char* pfn = (char*)pch;

	pch = strtok(pfn, " .");
	while (pch != 0)
	{
		if (strcmp(pch, "bmp") == 0)
			return Image_bmp(buffer);
		if (strcmp(pch, "png") == 0)
			return Image_png(buffer);
		pch = strtok(0, " .");
	}
	return 0;
}

Icons* Framework::CreateIcons(const wchar_t* imageFile, Backend* backend)
{
	StringW strw = imageFile;
	StringA stra = strw.to_utf8();
	return CreateIcons(stra.data(), backend);
}

Icons* Framework::CreateIcons(const char* imageFile, Backend* backend)
{
	Icons* icons = 0;
	Image* image = LoadImage(imageFile);
	if (image)
	{
		icons = new Icons(backend);
		icons->m_texture = backend->CreateTexture(image->m_image);
		mgPointSet(&icons->m_textureSize, image->m_image->width, image->m_image->height);
		image->Release();
		
		/*icons->m_icons = mgCreateIcons(t, image->m_image->width, image->m_image->height, iconsNum);*/
	}
	return icons;
}

mgColor Framework::GetColor(ColorName cn)
{
	return m_colors[static_cast<uint32_t>(cn)];
}

mgColor* Framework::GetColorPtr(ColorName cn)
{
	return &m_colors[static_cast<uint32_t>(cn)];
}
