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
#include "framework/Framework.h"
#include "framework/Cryptography.h"

#include "OpenSSL/md5.h"
#include "OpenSSL/md4.h"
#include "OpenSSL/md2.h"
#include "OpenSSL/mdc2.h"

using namespace mgf;

CryptographyMD::CryptographyMD(){}
CryptographyMD::~CryptographyMD(){}

void CryptographyMD::GenerateMD5(const void* buf, size_t len)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, buf, len);
	MD5_Final(m_result, &ctx);
}

void CryptographyMD::GenerateMD4(const void* buf, size_t len)
{
	MD4_CTX ctx;
	MD4_Init(&ctx);
	MD4_Update(&ctx, buf, len);
	MD4_Final(m_result, &ctx);
}

void CryptographyMD::GenerateMD2(const void* buf, size_t len)
{
	MD2_CTX ctx;
	MD2_Init(&ctx);
	MD2_Update(&ctx, (const unsigned char*)buf, len);
	MD2_Final(m_result, &ctx);
}

void CryptographyMD::GenerateMDC2(const void* buf, size_t len)
{
	MDC2_CTX ctx;
	MDC2_Init(&ctx);
	MDC2_Update(&ctx, (const unsigned char*)buf, len);
	MDC2_Final(m_result, &ctx);
}

void CryptographyMD::Print(FILE* f, const char* prefix, const char* suffix)
{
	if (prefix)
		fprintf(f, "%s", prefix);
	for (int i = 0; i < 16; ++i)
	{
		fprintf(f, "%x", (unsigned char)m_result[i]);
	}
	if (suffix)
		fprintf(f, "%s", suffix);
}

