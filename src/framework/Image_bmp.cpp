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

#pragma pack(push,2)
struct BitmapHeader {
	uint16_t		bfType;
	uint32_t		bfSize;
	uint16_t		bfReserved1;
	uint16_t		bfReserved2;
	uint32_t		bfOffBits;
};
#pragma pack(pop)

struct ciexyzTriple {
	int		ciexyzRed[3];
	int		ciexyzGreen[3];
	int		ciexyzBlue[3];
};

struct BitmapInfoHeader_v5 {
	uint32_t			bV5Size;		//	размер header в файле
	uint32_t			bV5Width;		//	ширина
	uint32_t			bV5Height;		//	высота
	uint16_t			bV5Planes;		//	хз что это, всегда 1
	uint16_t			bV5BitCount;	//	биты
	uint32_t			bV5Compression;	//	1 - RLE 8bit, 2 - RLE 4bit, 3 или что-то, видимо, специальные обозначения у разработчиков 2D редакторов.
	uint32_t			bV5SizeImage;	//	размер массива пикселей/индексов
	uint32_t			bV5XPelsPerMeter;// размер в чём-то, видимо для печати или вывода ещё кудато
	uint32_t			bV5YPelsPerMeter;//	для обычного использования в ПК не играет никакой роли
	uint32_t			bV5ClrUsed;		//	обычно тут ноль
	uint32_t			bV5ClrImportant;//	и тут ноль
	uint32_t			bV5RedMask;		//	для определения позиции цветов
	uint32_t			bV5GreenMask;	//	в форматах типа x1r5g5b5
	uint32_t			bV5BlueMask;
	uint32_t			bV5AlphaMask;
	uint32_t			bV5CSType;		//	далее информация для более специализированного
	ciexyzTriple bV5Endpoints;	//	использования.
	uint32_t			bV5GammaRed;	//	для передачи простой картинки достаточно того
	uint32_t			bV5GammaGreen;	//	что указано выше. А эта часть нужна для, например,
	uint32_t			bV5GammaBlue;	//	тех кто делает видео плеер, видео редактор
	uint32_t			bV5Intent;		//	что-то типа этого. Как бы универсальное решение
	uint32_t			bV5ProfileData;	//	от Microsoft
	uint32_t			bV5ProfileSize;
	uint32_t			bV5Reserved;
};


Image* mgf::Image_bmp(const char* fn)
{
	Image* img = 0;

	FILE* file = fopen(fn, "rb");
	if (!file)
		return 0;

	BitmapHeader header;
	BitmapInfoHeader_v5 info;
	if (fread(&header, 1, sizeof(BitmapHeader), file) != sizeof(BitmapHeader))
	{
		fclose(file);
		return 0;
	}

	fread(&info, 1, sizeof(BitmapInfoHeader_v5), file);
	if (info.bV5Size < 40U)
	{
		fclose(file);
		return 0;
	}

	if (header.bfType != 19778)
	{
		fclose(file);
		return 0;
	}

	int width = info.bV5Width;
	int height = info.bV5Height;
	int bits = info.bV5BitCount;
	int pitch = 0;
	int dataSize = 0;
	unsigned char* data = 0;

	if (bits == 24)
	{
		pitch = width * 3;
		dataSize = pitch * height;
		data = (unsigned char*)malloc(dataSize);
		fseek(file, 54, SEEK_SET);
		fread(data, 1, dataSize, file);
	}
	else if (bits == 32)
	{
		pitch = width * 4;
		uint32_t offset = header.bfOffBits;

		if (info.bV5Compression == 3) // BI_BITFIELDS
		{
			fseek(file, offset, SEEK_SET);
			dataSize = pitch * height;
			data = (unsigned char*)malloc(dataSize);

			fread(data, 1, dataSize, file);
			/*image.m_data->flipPixel();*/
		}
		else
		{
			fclose(file);
			return 0;
		}
	}

	fclose(file);

	img = new Image;
	img->m_image = (mgImage_s*)calloc(1, sizeof(mgImage_s));
	img->m_image->data = data;
	img->m_image->dataSize = dataSize;
	img->m_image->width = width;
	img->m_image->height = height;
	img->m_image->bits = bits;
	img->m_image->pitch = pitch;

	return img;
}

