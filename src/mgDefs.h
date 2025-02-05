﻿/*
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

#pragma once
#ifndef _MG_DEFS_H_
#define _MG_DEFS_H_

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include <stdint.h>


#if defined(WIN32) | defined(_WIN64) | defined(_WIN32)
#define MG_PLATFORM_WINDOWS
#define MG_C_DECL _cdecl
#else
#error Please write code for other OS
#endif



#ifdef _DEBUG
#define MG_DEBUG
#endif

#define MG_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((unsigned int)(unsigned char)(ch0)|((unsigned int)(unsigned char)(ch1)<<8)|\
	((unsigned int)(unsigned char)(ch2)<<16)|((unsigned int)(unsigned char)(ch3)<<24))

#ifdef MG_PLATFORM_WINDOWS
#if defined _WIN64 || defined __x86_64__
#define MG_BIT_64
#endif
#else
#endif

#ifdef _MSC_VER
#define MGF_FUNCTION __FUNCTION__
#else
#define MGF_FUNCTION "functionname"
#endif

#ifdef _MSC_VER

#if _MSC_VER < 1930
#define MGF_LINK_LIBRARY_CMP "_v142"
#endif

#ifdef MG_BIT_64
#define MGF_LINK_LIBRARY_ARCH "_x64"
#else
#define MGF_LINK_LIBRARY_ARCH "_x86"
#endif

#ifdef MG_DEBUG
#define MGF_LINK_LIBRARY_CONF "_Debug"
#else
#define MGF_LINK_LIBRARY_CONF "_Release"
#endif

#define MGF_LINK_LIBRARY(n) \
	__pragma(comment(lib, n MGF_LINK_LIBRARY_CMP MGF_LINK_LIBRARY_ARCH MGF_LINK_LIBRARY_CONF))
#endif

#define MG_LO32(l)           ((uint16_t)(((uint32_t)(l)) & 0xffff))
#define MG_HI32(l)           ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))

#endif