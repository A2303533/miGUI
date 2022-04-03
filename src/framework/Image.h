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
#ifndef _MGF_IMAGE_H_
#define _MGF_IMAGE_H_


namespace mgf
{
	class Image : public BaseClass
	{
		friend class Framework;
		friend Image* Image_bmp(const char* fn);
		friend Image* Image_png(const char* fn);

		mgImage_s* m_image = 0;

		void _convert_r8g8b8_to_r8g8b8a8();
		void _convert_r8g8b8_to_a8r8g8b8();

		void _convert_r8g8b8a8_to_r8g8b8();
		void _convert_r8g8b8a8_to_a8r8g8b8();

		void _convert_a8r8g8b8_to_r8g8b8();
		void _convert_a8r8g8b8_to_r8g8b8a8();
	public:
		Image();
		Image(mgImage_s*); //only pointer, not copy!
		virtual ~Image();

		//m_image = 0;
		void Drop();

		void Create(uint32_t x, uint32_t y, const mgColor&);
		void Free();

		void Fill(const mgColor&);

		// mgImageType from mgImage.h
		void Convert(uint32_t);

		uint32_t GetPitch();
		uint32_t GetWidth();
		uint32_t GetHeight();
		uint8_t* GetData();
		uint32_t GetDataSize();

		mgImage_s* GetMGImage();
	};
	
	Image* Image_bmp(const char* fn);
	Image* Image_png(const char* fn);
}


#endif