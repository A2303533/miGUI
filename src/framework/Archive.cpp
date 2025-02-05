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

#include "mgf.h"
#include "Archive.h"
#include "fastlz.h"
#include "brotli/encode.h"
#include "Log.h"
#include "unzip.h"

#include <iterator>
#include <filesystem>

using namespace mgf;

ArchiveSystem::ArchiveSystem()
{

}

ArchiveSystem::~ArchiveSystem()
{
	for (size_t i = 0; i < m_zipFiles.size(); ++i)
	{
		if (m_zipFiles[i]->m_implementation)
		{
			unzClose(m_zipFiles[i]->m_implementation);
		}
		delete m_zipFiles[i];
	}
}

ArchiveZipFile* ArchiveSystem::ZipAdd(const char* fn)
{
	assert(fn);
	LogWriteInfo("AddZip: [%s]\n", fn);

	if (!std::filesystem::exists(fn))
	{
		LogWriteWarning("AddZip: Unable to find zip file [%s]\n", fn);
		return 0;
	}

	unzFile uf = unzOpen64(fn);
	if (uf == NULL)
	{
		LogWriteWarning("AddZip: unzOpen64 failed [%s]\n", fn);
		return 0;
	}
	
	ArchiveZipFile* zp = new ArchiveZipFile;
	zp->m_fileName = fn;
	zp->m_implementation = uf;

	m_zipFiles.push_back(zp);

	return zp;
}

uint8_t* ArchiveSystem::ZipUnzip(const char* fileInZip, uint32_t* size, ArchiveZipFile* a)
{
	if (a)
		goto lockate_file;

	for (size_t i = 0, sz = m_zipFiles.size(); i < sz; ++i)
	{
		if (unzLocateFile(m_zipFiles[i]->m_implementation, fileInZip, 0) == UNZ_OK)
		{
			a = m_zipFiles[i];
			goto unzip_file;
		}
	}

	LogWriteWarning("file %s not found in the zipfile\n", fileInZip);
	return 0;

lockate_file:;
	if (unzLocateFile(a->m_implementation, fileInZip, 0) != UNZ_OK)
	{
		LogWriteWarning("file %s not found in the zipfile\n", fileInZip);
		return 0;
	}

unzip_file:;
	char filename_inzip[256];

	unz_file_info64 file_info;
	int err = unzGetCurrentFileInfo64(a->m_implementation, &file_info, 
		filename_inzip, 
		sizeof(filename_inzip), 
		NULL, 0, NULL, 0);

	if (err != UNZ_OK)
	{
		LogWriteWarning("error %d with zipfile in unzGetCurrentFileInfo [%s]\n", err, fileInZip);
		return 0;
	}

	err = unzOpenCurrentFilePassword(a->m_implementation, 0);
	if (err != UNZ_OK)
	{
		LogWriteWarning("error %d with zipfile in unzOpenCurrentFilePassword [%s]\n", err, fileInZip);
		return 0;
	}

	uint8_t* data = (uint8_t*)malloc(file_info.uncompressed_size);
	*size = (uint32_t)file_info.uncompressed_size;
	
	err = unzReadCurrentFile(a->m_implementation, data, file_info.uncompressed_size);
	if (err < 0)
	{
		LogWriteWarning("error %d with zipfile in unzReadCurrentFile [%s]\n", err, fileInZip);
		free(data);
		data = 0;
	}

	unzCloseCurrentFile(a->m_implementation);

	return data;
}

bool ArchiveSystem::Compress(CompressionInfo* info)
{
	assert(info);
	assert(info->m_dataUncompressed);
	assert(info->m_sizeUncompressed);

	info->m_dataCompressed = 0;
	info->m_sizeCompressed = 0;

	switch (info->m_compressorType)
	{
	case CompressorType::fastlz:
		return _compress_fastlz(info);
	}
	return false;
}

bool ArchiveSystem::Decompress(CompressionInfo* info)
{
	assert(info);
	assert(info->m_dataCompressed);
	assert(info->m_sizeCompressed);
	assert(info->m_dataUncompressed);
	assert(info->m_sizeUncompressed);

	switch (info->m_compressorType)
	{
	case CompressorType::fastlz:
		return _decompress_fastlz(info);
	}
	return false;
}

bool ArchiveSystem::Decompress(CompressionInfo* info, std::vector<uint8_t>& toVector)
{
	assert(info);

	toVector.clear();
	toVector.reserve(info->m_sizeUncompressed);
	if (!info->m_dataCompressed || !info->m_sizeCompressed)
	{
		if (info->m_dataUncompressed && info->m_sizeUncompressed)
		{
			std::copy(info->m_dataUncompressed, 
				info->m_dataUncompressed + info->m_sizeUncompressed,
				std::back_inserter(toVector));

			return true;
		}
		return false;
	}

	switch (info->m_compressorType)
	{
	case CompressorType::fastlz:
	{
		CompressionInfo cmpInf = *info;
		cmpInf.m_dataUncompressed = (uint8_t*)malloc(cmpInf.m_sizeUncompressed);
		bool result = _decompress_fastlz(&cmpInf);
		if (result)
		{
			std::copy(cmpInf.m_dataUncompressed,
				cmpInf.m_dataUncompressed + cmpInf.m_sizeUncompressed,
				std::back_inserter(toVector));
		}
		free(cmpInf.m_dataUncompressed);
		return result;
	}break;
	}
	return false;
}

bool ArchiveSystem::_compress_fastlz(CompressionInfo* info)
{
	if (info->m_level != 1 && info->m_level != 2)
		info->m_level = 2;

	if (info->m_sizeUncompressed < 32)
		return false;

	uint8_t* output = (uint8_t*)malloc(info->m_sizeUncompressed + (info->m_sizeUncompressed/3));

	int compressed_size = fastlz_compress_level(
		(int)info->m_level, 
		info->m_dataUncompressed,
		info->m_sizeUncompressed,
		output);

	if ((uint32_t)compressed_size >= info->m_sizeUncompressed)
	{
		free(output);
		return false;
	}

	output = (uint8_t*)realloc(output, compressed_size);
	
	info->m_dataCompressed = output;
	info->m_sizeCompressed = compressed_size;
	return true;
}

bool ArchiveSystem::_decompress_fastlz(CompressionInfo* info)
{
	int decompressed_size = fastlz_decompress(
		info->m_dataCompressed,
		info->m_sizeCompressed,
		info->m_dataUncompressed,
		info->m_sizeUncompressed);
	if (!decompressed_size)
	{
		mgf::LogWriteWarning("%s: %s\n", __FUNCTION__, "can't decompress");
		return false;
	}

	if (info->m_sizeUncompressed != decompressed_size)
	{
		info->m_sizeUncompressed = decompressed_size;
		mgf::LogWriteWarning("%s: %s\n", __FUNCTION__, "`decompressed size` is not the sme with m_sizeUncompressed");
	}
	return true;
}

//bool ArchiveSystem::_decompress_fastlz(CompressionInfo* info, std::vector<uint8_t>& toVector)
//{
//	
//	return true;
//}
