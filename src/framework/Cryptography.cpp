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

#ifdef MGF_OPENSSL

#include <openssl/evp.h>
#include <openssl/bio.h>

#include "OpenSSL/md5.h"
#include "OpenSSL/md4.h"
#include "OpenSSL/md2.h"
#include "OpenSSL/mdc2.h"

#include "OpenSSL/sha.h"

#define CRYPTO_FREE_MD(x) if(x) EVP_MD_CTX_free(x)

using namespace mgf;

struct OpensslData
{
	EVP_MD_CTX* m_md_sha1_ctx = 0;
	EVP_MD_CTX* m_md_sha224_ctx = 0;
	EVP_MD_CTX* m_md_sha384_ctx = 0;
	EVP_MD_CTX* m_md_sha512_ctx = 0;
	EVP_MD_CTX* m_md_sha512_224_ctx = 0;
	EVP_MD_CTX* m_md_sha512_256_ctx = 0;
	EVP_MD_CTX* m_md_sha3_224 = 0;
	EVP_MD_CTX* m_md_sha3_256 = 0;
	EVP_MD_CTX* m_md_sha3_384 = 0;
	EVP_MD_CTX* m_md_sha3_512 = 0;
	EVP_MD_CTX* m_md_shake_128 = 0;
	EVP_MD_CTX* m_md_shake_256 = 0;
	EVP_MD_CTX* m_md_md2 = 0;
	EVP_MD_CTX* m_md_md4 = 0;
	EVP_MD_CTX* m_md_md5 = 0;
	EVP_MD_CTX* m_md_md5_sha1 = 0;
	EVP_MD_CTX* m_md_blake2b512 = 0;
	EVP_MD_CTX* m_md_blake2s256 = 0;
	EVP_MD_CTX* m_md_mdc2 = 0;
	EVP_MD_CTX* m_md_ripemd160 = 0;
	EVP_MD_CTX* m_md_whirlpool = 0;
	EVP_MD_CTX* m_md_sm3 = 0;
};

Crypto::Crypto()
{
	OpensslData* ossdata = new OpensslData;
	ossdata->m_md_sha1_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha224_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha384_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha512_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha512_224_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha512_256_ctx = EVP_MD_CTX_new();
	ossdata->m_md_sha3_224 = EVP_MD_CTX_new();
	ossdata->m_md_sha3_256 = EVP_MD_CTX_new();
	ossdata->m_md_sha3_384 = EVP_MD_CTX_new();
	ossdata->m_md_sha3_512 = EVP_MD_CTX_new();
	ossdata->m_md_shake_128 = EVP_MD_CTX_new();
	ossdata->m_md_shake_256 = EVP_MD_CTX_new();
	ossdata->m_md_md2 = EVP_MD_CTX_new();
	ossdata->m_md_md4 = EVP_MD_CTX_new();
	ossdata->m_md_md5 = EVP_MD_CTX_new();
	ossdata->m_md_md5_sha1 = EVP_MD_CTX_new();
	ossdata->m_md_blake2b512 = EVP_MD_CTX_new();
	ossdata->m_md_blake2s256 = EVP_MD_CTX_new();
	ossdata->m_md_mdc2 = EVP_MD_CTX_new();
	ossdata->m_md_ripemd160 = EVP_MD_CTX_new();
	ossdata->m_md_whirlpool = EVP_MD_CTX_new();
	ossdata->m_md_sm3 = EVP_MD_CTX_new();

	m_opensslData = ossdata;
}

Crypto::~Crypto()
{
	OpensslData* ossdata = (OpensslData*)m_opensslData;

	CRYPTO_FREE_MD(ossdata->m_md_sha1_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha224_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha384_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha512_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha512_224_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha512_256_ctx);
	CRYPTO_FREE_MD(ossdata->m_md_sha3_224);
	CRYPTO_FREE_MD(ossdata->m_md_sha3_256);
	CRYPTO_FREE_MD(ossdata->m_md_sha3_384);
	CRYPTO_FREE_MD(ossdata->m_md_sha3_512);
	CRYPTO_FREE_MD(ossdata->m_md_shake_128);
	CRYPTO_FREE_MD(ossdata->m_md_shake_256);
	CRYPTO_FREE_MD(ossdata->m_md_md2);
	CRYPTO_FREE_MD(ossdata->m_md_md4);
	CRYPTO_FREE_MD(ossdata->m_md_md5);
	CRYPTO_FREE_MD(ossdata->m_md_md5_sha1);
	CRYPTO_FREE_MD(ossdata->m_md_blake2b512);
	CRYPTO_FREE_MD(ossdata->m_md_blake2s256);
	CRYPTO_FREE_MD(ossdata->m_md_mdc2);
	CRYPTO_FREE_MD(ossdata->m_md_ripemd160);
	CRYPTO_FREE_MD(ossdata->m_md_whirlpool);
	CRYPTO_FREE_MD(ossdata->m_md_sm3);
}

void Crypto::UpdateDigestSHA1(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha1_ctx, EVP_sha1(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha1_ctx, d, sz);
}

void Crypto::UpdateDigestSHA224(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha224_ctx, EVP_sha224(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha224_ctx, d, sz);
}
void Crypto::UpdateDigestSHA384(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha384_ctx, EVP_sha384(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha384_ctx, d, sz);
}
void Crypto::UpdateDigestSHA512(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha512_ctx, EVP_sha512(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha512_ctx, d, sz);
}
void Crypto::UpdateDigestSHA512_224(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha512_224_ctx, EVP_sha512_224(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha512_224_ctx, d, sz);
}
void Crypto::UpdateDigestSHA512_256(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha512_256_ctx, EVP_sha512_256(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha512_256_ctx, d, sz);
}
void Crypto::UpdateDigestSHA3_224(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha3_224, EVP_sha3_224(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha3_224, d, sz);
}
void Crypto::UpdateDigestSHA3_256(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha3_256, EVP_sha3_256(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha3_256, d, sz);
}
void Crypto::UpdateDigestSHA3_384(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha3_384, EVP_sha3_384(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha3_384, d, sz);
}
void Crypto::UpdateDigestSHA3_512(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sha3_512, EVP_sha3_512(), 0);
	EVP_DigestUpdate(ossdata->m_md_sha3_512, d, sz);
}
void Crypto::UpdateDigestSHAKE_128(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_shake_128, EVP_shake128(), 0);
	EVP_DigestUpdate(ossdata->m_md_shake_128, d, sz);
}
void Crypto::UpdateDigestSHAKE_256(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_shake_256, EVP_shake256(), 0);
	EVP_DigestUpdate(ossdata->m_md_shake_256, d, sz);
}
void Crypto::UpdateDigestMD2(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_md2, EVP_md2(), 0);
	EVP_DigestUpdate(ossdata->m_md_md2, d, sz);
}
void Crypto::UpdateDigestMD4(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_md4, EVP_md4(), 0);
	EVP_DigestUpdate(ossdata->m_md_md4, d, sz);
}
void Crypto::UpdateDigestMD5(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_md5, EVP_md5(), 0);
	EVP_DigestUpdate(ossdata->m_md_md5, d, sz);
}
void Crypto::UpdateDigestMD5_SHA1(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_md5_sha1, EVP_md5_sha1(), 0);
	EVP_DigestUpdate(ossdata->m_md_md5_sha1, d, sz);
}
void Crypto::UpdateDigestBLAKE2b512(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_blake2b512, EVP_blake2b512(), 0);
	EVP_DigestUpdate(ossdata->m_md_blake2b512, d, sz);
}
void Crypto::UpdateDigestBLAKE2s256(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_blake2s256, EVP_blake2s256(), 0);
	EVP_DigestUpdate(ossdata->m_md_blake2s256, d, sz);
}
void Crypto::UpdateDigestMDC2(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_mdc2, EVP_mdc2(), 0);
	EVP_DigestUpdate(ossdata->m_md_mdc2, d, sz);
}
void Crypto::UpdateDigestRIPEMD160(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_ripemd160, EVP_ripemd160(), 0);
	EVP_DigestUpdate(ossdata->m_md_ripemd160, d, sz);
}
void Crypto::UpdateDigestWHIRLPOOL(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_whirlpool, EVP_whirlpool(), 0);
	EVP_DigestUpdate(ossdata->m_md_whirlpool, d, sz);
}
void Crypto::UpdateDigestSM3(const void* d, size_t sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestInit_ex(ossdata->m_md_sm3, EVP_sm3(), 0);
	EVP_DigestUpdate(ossdata->m_md_sm3, d, sz);
}

void Crypto::FinalDigestSHA1(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha1_ctx, md, sz);
}

void Crypto::FinalDigestSHA224(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha224_ctx, md, sz);
}
void Crypto::FinalDigestSHA384(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha384_ctx, md, sz);
}
void Crypto::FinalDigestSHA512(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha512_ctx, md, sz);
}
void Crypto::FinalDigestSHA512_224(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha512_224_ctx, md, sz);
}
void Crypto::FinalDigestSHA512_256(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha512_256_ctx, md, sz);
}
void Crypto::FinalDigestSHA3_224(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha3_224, md, sz);
}
void Crypto::FinalDigestSHA3_256(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha3_256, md, sz);
}
void Crypto::FinalDigestSHA3_384(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha3_384, md, sz);
}
void Crypto::FinalDigestSHA3_512(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sha3_512, md, sz);
}
void Crypto::FinalDigestSHAKE_128(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_shake_128, md, sz);
}
void Crypto::FinalDigestSHAKE_256(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_shake_256, md, sz);
}
void Crypto::FinalDigestMD2(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_md2, md, sz);
}
void Crypto::FinalDigestMD4(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_md4, md, sz);
}
void Crypto::FinalDigestMD5(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_md5, md, sz);
}
void Crypto::FinalDigestMD5_SHA1(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_md5_sha1, md, sz);
}
void Crypto::FinalDigestBLAKE2b512(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_blake2b512, md, sz);
}
void Crypto::FinalDigestBLAKE2s256(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_blake2s256, md, sz);
}
void Crypto::FinalDigestMDC2(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_mdc2, md, sz);
}
void Crypto::FinalDigestRIPEMD160(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_ripemd160, md, sz);
}
void Crypto::FinalDigestWHIRLPOOL(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_whirlpool, md, sz);
}
void Crypto::FinalDigestSM3(unsigned char* md, unsigned int* sz) {
	OpensslData* ossdata = (OpensslData*)m_opensslData;
	EVP_DigestFinal(ossdata->m_md_sm3, md, sz);
}

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

#endif
