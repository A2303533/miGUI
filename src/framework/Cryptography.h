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

	class Crypto
	{
		void* m_opensslData = 0;
	public:
		Crypto();
		~Crypto();

		void InitDigestSHA1();
		void InitDigestSHA224();
		void InitDigestSHA384();
		void InitDigestSHA512();
		void InitDigestSHA512_224();
		void InitDigestSHA512_256();
		void InitDigestSHA3_224();
		void InitDigestSHA3_256();
		void InitDigestSHA3_384();
		void InitDigestSHA3_512();
		void InitDigestSHAKE_128();
		void InitDigestSHAKE_256();
		void InitDigestMD2();
		void InitDigestMD4();
		void InitDigestMD5();
		void InitDigestMD5_SHA1();
		void InitDigestBLAKE2b512();
		void InitDigestBLAKE2s256();
		void InitDigestMDC2();
		void InitDigestRIPEMD160();
		void InitDigestWHIRLPOOL();
		void InitDigestSM3();

		void UpdateDigestSHA1(const void* d, size_t sz);
		void UpdateDigestSHA224(const void* d, size_t sz);
		void UpdateDigestSHA384(const void* d, size_t sz);
		void UpdateDigestSHA512(const void* d, size_t sz);
		void UpdateDigestSHA512_224(const void* d, size_t sz);
		void UpdateDigestSHA512_256(const void* d, size_t sz);
		void UpdateDigestSHA3_224(const void* d, size_t sz);
		void UpdateDigestSHA3_256(const void* d, size_t sz);
		void UpdateDigestSHA3_384(const void* d, size_t sz);
		void UpdateDigestSHA3_512(const void* d, size_t sz);
		void UpdateDigestSHAKE_128(const void* d, size_t sz);
		void UpdateDigestSHAKE_256(const void* d, size_t sz);
		void UpdateDigestMD2(const void* d, size_t sz);
		void UpdateDigestMD4(const void* d, size_t sz);
		void UpdateDigestMD5(const void* d, size_t sz);
		void UpdateDigestMD5_SHA1(const void* d, size_t sz);
		void UpdateDigestBLAKE2b512(const void* d, size_t sz);
		void UpdateDigestBLAKE2s256(const void* d, size_t sz);
		void UpdateDigestMDC2(const void* d, size_t sz);
		void UpdateDigestRIPEMD160(const void* d, size_t sz);
		void UpdateDigestWHIRLPOOL(const void* d, size_t sz);
		void UpdateDigestSM3(const void* d, size_t sz);

		void FinalDigestSHA1(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA224(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA384(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA512(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA512_224(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA512_256(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA3_224(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA3_256(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA3_384(unsigned char* md, unsigned int* sz);
		void FinalDigestSHA3_512(unsigned char* md, unsigned int* sz);
		void FinalDigestSHAKE_128(unsigned char* md, unsigned int* sz);
		void FinalDigestSHAKE_256(unsigned char* md, unsigned int* sz);
		void FinalDigestMD2(unsigned char* md, unsigned int* sz);
		void FinalDigestMD4(unsigned char* md, unsigned int* sz);
		void FinalDigestMD5(unsigned char* md, unsigned int* sz);
		void FinalDigestMD5_SHA1(unsigned char* md, unsigned int* sz);
		void FinalDigestBLAKE2b512(unsigned char* md, unsigned int* sz);
		void FinalDigestBLAKE2s256(unsigned char* md, unsigned int* sz);
		void FinalDigestMDC2(unsigned char* md, unsigned int* sz);
		void FinalDigestRIPEMD160(unsigned char* md, unsigned int* sz);
		void FinalDigestWHIRLPOOL(unsigned char* md, unsigned int* sz);
		void FinalDigestSM3(unsigned char* md, unsigned int* sz);
	};

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