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
#include "framework/Image.h"

using namespace mgf;

struct pixel4
{
	char c[4];
};

Image::Image()
	:
m_image(0)
{
}

Image::Image(mgImage_s* ptr)
	:
m_image(ptr)
{

}

Image::~Image()
{
	Free();
}

void Image::Drop()
{
	m_image = 0;
}

void Image::Free()
{
	if (m_image)
	{
		if (m_image->data)
			free(m_image->data);
		free(m_image);
		m_image = 0;
	}
}

void Image::Fill(const mgColor& c)
{
	struct rgba
	{
		uint8_t r, g, b, a;
	};
	rgba* ptr = (rgba*)m_image->data;
	for (uint32_t i = 0, sz = m_image->width * m_image->height; i < sz; ++i)
	{
		ptr->a = c.getAsByteAlpha();
		ptr->r = c.getAsByteRed();
		ptr->g = c.getAsByteGreen();
		ptr->b = c.getAsByteBlue();
		++ptr;
	}
}

void Image::Create(uint32_t x, uint32_t y, const mgColor& c)
{
	Free();
	m_image = (mgImage_s*)calloc(1, sizeof(mgImage_s));
	m_image->width = x;
	m_image->height = y;
	m_image->bits = 32;
	m_image->pitch = m_image->width * 4;
	m_image->type = mgImageType_r8g8b8a8;
	m_image->dataSize = m_image->pitch * m_image->height;
	m_image->data = (unsigned char*)malloc(m_image->dataSize);
	Fill(c);
}

uint32_t Image::GetPitch()
{
	if (!m_image)
		return 0;
	return m_image->pitch;
}

uint32_t Image::GetWidth()
{
	if (!m_image)
		return 0;
	return m_image->width;
}

uint32_t Image::GetHeight()
{
	if (!m_image)
		return 0;
	return m_image->height;
}

uint8_t* Image::GetData()
{
	if (!m_image)
		return 0;
	return m_image->data;
}

uint32_t Image::GetDataSize()
{
	if (!m_image)
		return 0;
	return m_image->dataSize;
}



void Image::Convert(uint32_t newType)
{
	if (m_image->type == newType)
		return;

	

	switch (m_image->type)
	{
	case mgImageType_unknown:
		break;
	case mgImageType_r8g8b8:
		switch (newType)
		{
		default:
			printf("NOT IMPLEMENTED!\n");
			break;
		case mgImageType_unknown:
		case mgImageType_r8g8b8:
			break;
		case mgImageType_a8r8g8b8:
			_convert_r8g8b8_to_a8r8g8b8();
			break;
		case mgImageType_r8g8b8a8:
			_convert_r8g8b8_to_r8g8b8a8();
			break;
		}
		break;
	case mgImageType_a8r8g8b8:
		switch (newType)
		{
		default:
			printf("NOT IMPLEMENTED!\n");
			break;
		case mgImageType_unknown:
		case mgImageType_a8r8g8b8:
			break;
		case mgImageType_r8g8b8:
			_convert_a8r8g8b8_to_r8g8b8();
			break;
		case mgImageType_r8g8b8a8:
			_convert_a8r8g8b8_to_r8g8b8a8();
			break;
		}
		break;
	case mgImageType_r8g8b8a8:
		switch (newType)
		{
		default:
			printf("NOT IMPLEMENTED!\n");
			break;
		case mgImageType_unknown:
		case mgImageType_r8g8b8a8:
			break;
		case mgImageType_r8g8b8:
			_convert_r8g8b8a8_to_r8g8b8();
			break;
		case mgImageType_a8r8g8b8:
			_convert_r8g8b8a8_to_a8r8g8b8();
			break;
		}
		break;
	default:
		printf("NOT IMPLEMENTED!\n");
		break;
	}
	m_image->type = newType;
}

void Image::_convert_r8g8b8_to_r8g8b8a8()
{

}

void Image::_convert_r8g8b8_to_a8r8g8b8()
{

}

void Image::_convert_r8g8b8a8_to_r8g8b8()
{

}

void Image::_convert_r8g8b8a8_to_a8r8g8b8()
{
	pixel4* p = (pixel4*)m_image->data;
	for (int i = 0, sz = m_image->width * m_image->height; i < sz; ++i)
	{
		pixel4 p2 = *p;
		p->c[0] = p2.c[2];
		p->c[1] = p2.c[1];
		p->c[2] = p2.c[0];
		p->c[3] = p2.c[3];
		++p;
	}
}

void Image::_convert_a8r8g8b8_to_r8g8b8()
{

}

void Image::_convert_a8r8g8b8_to_r8g8b8a8()
{

}

mgImage_s* Image::GetMGImage()
{
	return m_image;
}
