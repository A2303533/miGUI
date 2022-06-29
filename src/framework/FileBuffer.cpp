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
#include "FileBuffer.h"

using namespace mgf;

FileBuffer::FileBuffer(const uint8_t* buffer, size_t size)
	:
	m_buffer((uint8_t*)buffer),
	m_size(size)
{
}

FileBuffer::~FileBuffer()
{
}

void FileBuffer::seek(size_t offset, int where)
{
	switch (where)
	{
	default:
	case SEEK_CUR:
	{
		m_cursor += offset;
		if (m_cursor > m_size)
			m_cursor = m_size;
	}break;
	case SEEK_END:
		m_cursor = m_size;
		break;
	case SEEK_SET:
		m_cursor = offset;
		break;
	}
}

size_t FileBuffer::tell()
{
	return m_cursor;
}

size_t FileBuffer::read(void* buffer, size_t size)
{
	uint8_t* bytes = (uint8_t*)buffer;

	size_t numRead = 0;
	for (numRead = 0; numRead < size; ++numRead)
	{
		if (m_cursor == m_size)
			return numRead;

		bytes[numRead] = m_buffer[m_cursor];
		++m_cursor;
	}
	return numRead;
}

