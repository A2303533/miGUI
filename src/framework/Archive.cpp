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

#include "mgf.h"
#include "Archive.h"
#include "fastlz.h"
#include "Log.h"

using namespace mgf;

ArchiveSystem::ArchiveSystem()
{

}

ArchiveSystem::~ArchiveSystem()
{

}

bool ArchiveSystem::Compress(CompressionInfo* info)
{
	assert(info);
	assert(info->m_dataUncompressed);
	assert(info->m_sizeUncompressed);

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

	switch (info->m_compressorType)
	{
	case CompressorType::fastlz:
		return _decompress_fastlz(info);
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
