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
#ifndef _MGF_AUDIOENGINE_H_
#define _MGF_AUDIOENGINE_H_

#include <stdlib.h>
#include <stdint.h>

namespace mgf
{
	/// <summary>
	/// Raw data.
	/// </summary>
	class AudioSourceData
	{
	public:
		AudioSourceData() {}
		~AudioSourceData() 
		{
			if (m_data)
				free(m_data);
		}

		uint8_t* m_data = 0;
		uint32_t m_dataSize = 0;

		uint32_t m_channels = 0;
		uint32_t m_samplesPerSec = 0;
		uint32_t m_avgBytesPerSec = 0;
		uint32_t m_bitsPerSample = 0;
	};

	class AudioSource : public BaseClass
	{
	public:
		AudioSource() {}
		virtual ~AudioSource() 
		{
			if (m_data)
				delete m_data;
		}

		AudioSourceData* m_data = 0;
	};

	class AudioEngine : public BaseClass
	{
		//void* m_implementationData;
	public:

		virtual AudioSource* Load(const char*) = 0;
		virtual void Play(AudioSource*) = 0;
	};

	AudioSourceData* Audio_Wav(const char*);
}

#endif