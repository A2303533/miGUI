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

#include "OpenSSL/sha.h"

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

void Cryptography_Print(const unsigned char* buffer, size_t size, FILE* f, const char* prefix, const char* suffix)
{
	if (prefix)
		fprintf(f, "%s", prefix);
	for (size_t i = 0; i < size; ++i)
	{
		fprintf(f, "%02x", (unsigned char)buffer[i]);
	}
	if (suffix)
		fprintf(f, "%s", suffix);
}

void CryptographyMD::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthMD, f, prefix, suffix);
}

CryptographySHA1::CryptographySHA1(){}
CryptographySHA1::~CryptographySHA1(){}

void CryptographySHA1::Generate(const void* buf, size_t len)
{
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	SHA1_Update(&ctx, (const unsigned char*)buf, len);
	SHA1_Final(m_result, &ctx);
}

void CryptographySHA1::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthSHA1, f, prefix, suffix);
}

CryptographySHA224::CryptographySHA224() {}
CryptographySHA224::~CryptographySHA224() {}

void CryptographySHA224::Generate(const void* buf, size_t len)
{
	SHA256_CTX ctx;
	SHA224_Init(&ctx);
	SHA224_Update(&ctx, (const unsigned char*)buf, len);
	SHA224_Final(m_result, &ctx);
}

void CryptographySHA224::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthSHA224, f, prefix, suffix);
}

CryptographySHA256::CryptographySHA256() {}
CryptographySHA256::~CryptographySHA256() {}

void CryptographySHA256::Generate(const void* buf, size_t len)
{
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (const unsigned char*)buf, len);
	SHA256_Final(m_result, &ctx);
}

void CryptographySHA256::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthSHA256, f, prefix, suffix);
}

CryptographySHA384::CryptographySHA384() {}
CryptographySHA384::~CryptographySHA384() {}

void CryptographySHA384::Generate(const void* buf, size_t len)
{
	SHA512_CTX ctx;
	SHA384_Init(&ctx);
	SHA384_Update(&ctx, (const unsigned char*)buf, len);
	SHA384_Final(m_result, &ctx);
}

void CryptographySHA384::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthSHA384, f, prefix, suffix);
}

CryptographySHA512::CryptographySHA512() {}
CryptographySHA512::~CryptographySHA512() {}

void CryptographySHA512::Generate(const void* buf, size_t len)
{
	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, (const unsigned char*)buf, len);
	SHA512_Final(m_result, &ctx);
}

void CryptographySHA512::Print(FILE* f, const char* prefix, const char* suffix)
{
	Cryptography_Print(m_result, CryptographyDigestLengthSHA512, f, prefix, suffix);
}
