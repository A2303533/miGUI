/*
  Copyright (C) 2021 Basov Artyom
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

#ifndef _MI_GUI_H_
#define _MI_GUI_H_

#include <stddef.h>

#if defined(WIN32)
#define MG_PLATFORM_WINDOWS
#define MG_C_DECL _cdecl
#ifdef MG_DLL
#define MG_API _declspec(dllexport) 
#else
#define MG_API _declspec(dllimport) 
#endif
#else
#error Please write code for other OS
#endif

#define MG_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((unsigned int)(unsigned char)(ch0)|((unsigned int)(unsigned char)(ch1)<<8)|\
	((unsigned int)(unsigned char)(ch2)<<16)|((unsigned int)(unsigned char)(ch3)<<24))

struct mgContext_s;

#include "mgPoint.h"
#include "mgRect.h"
#include "mgVec4.h"
#include "mgColor.h"
#include "mgFont.h"
#include "mgImage.h"

typedef void* mgTexture;

#include "mgElement.h"

/* Before creating GUI context you must create this objects.
 VideoDriverAPI - callbacks for drawing.
 InputContext - information about pressed buttons, cursor position and other*/
typedef struct mgVideoDriverAPI_s {
	/* Create GPU texture using RGBA data.
	 Library can generate fonts, fonts is a textures.*/
	mgTexture(*createTexture)(mgImage*);

	/* Destroy texture.For fonts. */
	void(*destroyTexture)(mgTexture);

	void(*beginDraw)(); /*prepare for drawing, set shaders and other things*/
	void(*endDraw)();

	void(*drawRectangle)(
		mgElement* element, /*current element, can be null*/
		mgPoint* position,
		mgPoint* size, 
		mgColor* color1, 
		mgColor* color2, 
		mgTexture texture, /*optional*/
		mgVec4* UVRegion); /*optional*/

	void(*drawText)( mgPoint* position, wchar_t* text, int textLen, mgColor*, mgFont*);

	/*set new and return old clip rect*/
	mgRect(*setClipRect)(mgRect*);

} mgVideoDriverAPI;

#include "mgInputContex.h"

typedef struct mgContext_s {
	mgVideoDriverAPI* gpu;
	mgInputContext* input;
	mgElement* rootElement;
} mgContext;

#if defined(__cplusplus)
extern "C" {
#endif


typedef mgContext* (*PFNMGCREATECONTEXTPROC)(mgVideoDriverAPI*, mgInputContext*);
extern PFNMGCREATECONTEXTPROC mgCreateContext_p;
#define mgCreateContext mgCreateContext_p

typedef void (*PFNMGDESTROYCONTEXTPROC)(mgContext*);
extern PFNMGDESTROYCONTEXTPROC mgDestroyContext_p;
#define mgDestroyContext mgDestroyContext_p

/*Create bitmap font or load from file.*/
typedef mgFont* (*PFNMGCREATEFONTPROC)(struct mgContext_s*, const char*, unsigned int flags, int size);
extern PFNMGCREATEFONTPROC mgCreateFont_p;
#define mgCreateFont mgCreateFont_p

/*call before event loop*/
typedef void (*PFNMGSTARTFRAMEPROC)(struct mgContext_s*);
extern PFNMGSTARTFRAMEPROC mgStartFrame_p;
#define mgStartFrame mgStartFrame_p

/*do work*/
typedef void (*PFNMGUPDATEPROC)(struct mgContext_s*);
extern PFNMGUPDATEPROC mgUpdate_p;
#define mgUpdate mgUpdate_p

typedef void (*PFNMGSETPARENTPROC)(mgElement* object, mgElement* parent);
extern PFNMGSETPARENTPROC mgSetParent_p;
#define mgSetParent mgSetParent_p

typedef mgElement* (*PFNMGCREATERECTANGLEPROC)(struct mgContext_s* c, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2);
extern PFNMGCREATERECTANGLEPROC mgCreateRectangle_p;
#define mgCreateRectangle mgCreateRectangle_p

/*set visible include all children*/
typedef void (*PFNMGSETVISIBLEPROC)(mgElement*, int);
extern PFNMGSETVISIBLEPROC mgSetVisible_p;
#define mgSetVisible mgSetVisible_p

/*draw all*/
typedef void (*PFNMGDRAWPROC)(mgContext*);
extern PFNMGDRAWPROC mgDraw_p;
#define mgDraw mgDraw_p

#if defined(__cplusplus)
}
#endif

#endif