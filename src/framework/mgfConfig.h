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

#pragma once
#ifndef _MG_FRAMEWORK_CONFIG_H_
#define _MG_FRAMEWORK_CONFIG_H_

//#define MGF_USE_CRT

//#define MGF_NO_BACKEND_GDI
//#define MGF_NO_BACKEND_OPENGL
//#define MGF_NO_BACKEND_D3D11
//#define MGF_NO_GS_D3D11
//#define MGF_NO_OPENSSL

#ifndef MGF_NO_GS_D3D11
#define MGF_GS_D3D11
#endif

#ifndef MGF_NO_BACKEND_GDI
#define MGF_BACKEND_GDI
#endif

// simple opengl
#ifndef MGF_NO_BACKEND_OPENGL
#define MGF_BACKEND_OPENGL
#endif

#if !defined(MGF_NO_BACKEND_D3D11) || defined(MGF_GS_D3D11)
#define MGF_BACKEND_D3D11
#endif

#ifndef MGF_NO_OPENSSL
#define MGF_OPENSSL
#endif

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")

#endif