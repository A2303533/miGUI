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

#if defined(WIN32)
#define MG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MG_LOAD_LIB LoadLibraryA
#define MG_FREE_LIB FreeLibrary
#define MG_GET_PROC_ADDRESS GetProcAddress
#define MG_LIB_HANDLE HMODULE
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
		mgPoint* position,  /*left top corner*/
		mgPoint* size, 
		mgColor* color1, 
		mgColor* color2, 
		mgTexture texture, /*optional*/
		mgVec4* UVRegion); /*optional*/

	void(*drawText)(
		mgPoint* position,
		const wchar_t*,
		int textLen,
		mgColor*,
		mgFont*);

} mgVideoDriverAPI;

#include "mgInputContex.h"

/*Main interface*/
typedef struct mgContext_s {
	mgVideoDriverAPI* m_gpu;
	mgInputContext* m_input;

	/*Create bitmap font or load from file. 
	* You can create your own fonts (like winapi fonts),
	* don't forget to delete them...
	*/
	mgFont* (*createFont)(const char*, unsigned int flags, int size);

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


/*Load DLL. You must call mgUnload for unloading.*/
MG_LIB_HANDLE mgLoad();
/*Unload DLL*/
void mgUnload(MG_LIB_HANDLE);

#if defined(__cplusplus)
}
#endif

#endif