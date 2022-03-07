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

#include "lodepng.h"

Image* mgf::Image_png(const char* fn)
{
	Image* img = 0;

	unsigned int width = 0;
	unsigned int height = 0;
	int bits = 0;
	int pitch = 0;
	int dataSize = 0;
	unsigned char* data = 0;
	
	int type = mgImageType_a8r8g8b8;
	bits = 32;

	std::vector<unsigned char> image;
	std::string fileName = fn;

	unsigned error = lodepng::decode(image, width, height, fileName, LCT_RGBA, 8);
	if (error)
		return 0;

	pitch = width * 4;
	dataSize = image.size();

	data = (unsigned char*)malloc(dataSize);
	memcpy(data, image.data(), dataSize);

	struct pixel
	{
		char c[4];
	};
	pixel* p = (pixel*)data;
	for (int i = 0, sz = width * height; i < sz; ++i)
	{
		pixel p2 = *p;
		p->c[0] = p2.c[2];
		p->c[1] = p2.c[1];
		p->c[2] = p2.c[0];
		p->c[3] = p2.c[3];
		++p;
	}

	img = new Image;
	img->m_image = (mgImage_s*)calloc(1, sizeof(mgImage_s));
	img->m_image->data = data;
	img->m_image->dataSize = dataSize;
	img->m_image->width = width;
	img->m_image->height = height;
	img->m_image->bits = bits;
	img->m_image->pitch = pitch;
	img->m_image->type = type;

	return img;
}

