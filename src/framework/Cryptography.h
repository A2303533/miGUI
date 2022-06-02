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
#ifndef _MGF_CRYPTOGRAPHY_H_
#define _MGF_CRYPTOGRAPHY_H_

namespace mgf
{

#ifdef MGF_OPENSSL

#define CryptographyDigestLengthMD 16
#define CryptographyDigestLengthSHA1 20
#define CryptographyDigestLengthSHA224 28
#define CryptographyDigestLengthSHA256 32
#define CryptographyDigestLengthSHA384 48
#define CryptographyDigestLengthSHA512 64

	class CryptographyMD : public BaseClass
	{
	public:
		CryptographyMD();
		virtual ~CryptographyMD();

		void GenerateMD5(const void* buf, size_t len);
		void GenerateMD4(const void* buf, size_t len);
		void GenerateMD2(const void* buf, size_t len);
		void GenerateMDC2(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);

		unsigned char m_result[CryptographyDigestLengthMD];
	};

	class CryptographySHA1 : public BaseClass
	{
	public:
		CryptographySHA1();
		virtual ~CryptographySHA1();
		void Generate(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);
		unsigned char m_result[CryptographyDigestLengthSHA1];
	};

	class CryptographySHA224 : public BaseClass
	{
	public:
		CryptographySHA224();
		virtual ~CryptographySHA224();
		void Generate(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);
		unsigned char m_result[CryptographyDigestLengthSHA224];
	};

	class CryptographySHA256 : public BaseClass
	{
	public:
		CryptographySHA256();
		virtual ~CryptographySHA256();
		void Generate(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);
		unsigned char m_result[CryptographyDigestLengthSHA256];
	};

	class CryptographySHA384 : public BaseClass
	{
	public:
		CryptographySHA384();
		virtual ~CryptographySHA384();
		void Generate(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);
		unsigned char m_result[CryptographyDigestLengthSHA384];
	};

	class CryptographySHA512 : public BaseClass
	{
	public:
		CryptographySHA512();
		virtual ~CryptographySHA512();
		void Generate(const void* buf, size_t len);
		void Print(FILE*, const char* prefix, const char* suffix);
		unsigned char m_result[CryptographyDigestLengthSHA512];
	};

#endif
}

#endif