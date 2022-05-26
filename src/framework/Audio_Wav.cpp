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

#include "AudioEngine.h"

#include <filesystem>

//struct Wav_header
//{
//	int8_t		ChunkID[4] = { 'R', 'I', 'F', 'F' };
//	uint32_t	ChunkSize = 0;
//	int8_t		WAVEID[4] = { 'W', 'A', 'V', 'E' };
//	int8_t		Subchunk1[4] = { 0, 'f', 'm', 't' };
//};
//
//struct Wav_header_fmt
//{
//	uint32_t Subchunk1Size = 0;
//	uint16_t AudioFormat = 0;
//	uint16_t NumChannels = 0;
//	uint32_t SampleRate = 0;
//	uint32_t ByteRate = 0;
//	uint16_t BlockAlign = 0;
//	uint16_t BitsPerSample = 0;
//};

mgf::AudioSourceData* mgf::Audio_Wav(const char* fn)
{
	mgf::AudioSourceData* rawData = 0;
	if (std::filesystem::exists(fn) && (std::filesystem::file_size(fn) > 0x30))
	{
		FILE* f = fopen(fn, "rb");
		if (f)
		{
			char riff[5] = { 0,0,0,0,0 };
			fread(riff, 4, 1, f);
			if (strcmp(riff, "RIFF") == 0)
			{
				uint32_t RIFFChunkSize = 0;
				fread(&RIFFChunkSize, 4, 1, f);
			
				char wave[5] = { 0,0,0,0,0 };
				fread(wave, 4, 1, f);
				if (strcmp(wave, "WAVE") == 0)
				{
					char fmt[5] = { 0,0,0,0,0 };
					fread(fmt, 4, 1, f);
					if (strcmp(fmt, "fmt ") == 0)
					{
						uint32_t FMTChunkSize = 0;
						fread(&FMTChunkSize, 4, 1, f);
						
						uint16_t format = 0;
						fread(&format, 2, 1, f);

						uint16_t channels = 0;
						fread(&channels, 2, 1, f);

						uint32_t sampleRate = 0;
						fread(&sampleRate, 4, 1, f);

						uint32_t byteRate = 0;
						fread(&byteRate, 4, 1, f);

						uint16_t blockAlign = 0;
						fread(&blockAlign, 2, 1, f);

						uint16_t bitsPerSample = 0;
						fread(&bitsPerSample, 2, 1, f);

						char data[5] = { 0,0,0,0,0 };
						fread(data, 4, 1, f);
						if (strcmp(data, "data") == 0)
						{
							uint32_t dataSize = 0;
							fread(&dataSize, 4, 1, f);
							if (dataSize)
							{
								rawData = new mgf::AudioSourceData;
								rawData->m_data = (uint8_t*)malloc(dataSize);
								rawData->m_dataSize = dataSize;
								fread(rawData->m_data, dataSize, 1, f);
								
								rawData->m_channels = channels;
								rawData->m_avgBytesPerSec = byteRate;
								rawData->m_bitsPerSample = bitsPerSample;
								rawData->m_samplesPerSec = sampleRate;
							}
						}
					}
				}
			}

			fclose(f);
		}
	}
	return rawData;
}