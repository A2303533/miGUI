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

#include "AudioEngine_waveOut.h"

using namespace mgf;

#ifdef MG_PLATFORM_WINDOWS
#pragma comment(lib, "Winmm.lib")

AudioSource_waveOut::AudioSource_waveOut()
{
	memset(&m_waveformatex, 0, sizeof(WAVEFORMATEX));
	memset(&m_waveHdr, 0, sizeof(WAVEHDR));
}

AudioSource_waveOut::~AudioSource_waveOut()
{
	if (m_waveHdr.dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(m_waveOut, &m_waveHdr, sizeof(WAVEHDR));

	if (m_waveOut)
	{
		waveOutReset(m_waveOut);
		waveOutClose(m_waveOut);
	}
}


// ===============================================================
AudioEngine_waveOut::AudioEngine_waveOut()
{
}

AudioEngine_waveOut::~AudioEngine_waveOut() 
{
}

AudioSource* AudioEngine_waveOut::Load(const char* fn)
{
	AudioSource_waveOut* newSource = 0;

	AudioSourceData* rawData = Audio_Wav(fn);
	if (rawData)
	{
		newSource = new AudioSource_waveOut;
		newSource->m_data = rawData;
		newSource->m_waveformatex.wFormatTag = WAVE_FORMAT_PCM;
		newSource->m_waveformatex.nChannels = rawData->m_channels;
		newSource->m_waveformatex.nSamplesPerSec = rawData->m_samplesPerSec;
		newSource->m_waveformatex.wBitsPerSample = rawData->m_bitsPerSample;
		newSource->m_waveformatex.nBlockAlign = rawData->m_channels * rawData->m_bitsPerSample / 8;
		newSource->m_waveformatex.nAvgBytesPerSec = rawData->m_avgBytesPerSec;
		newSource->m_waveformatex.cbSize = 0;
	}

	return newSource;
}


void CALLBACK AudioEngine_waveOutProc(
	HWAVEOUT  hwo,
	UINT      uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
	switch (uMsg)
	{
	case WOM_CLOSE:
		printf("WOM CLOSE\n");
		break;
	case WOM_DONE:
	{
		printf("WOM DONE\n");
		//WAVEHDR* hdr = (WAVEHDR*)dwParam1;
	}break;
	case WOM_OPEN:
		printf("WOM OPEN\n");
		break;
	}
}

void AudioEngine_waveOut::Play(AudioSource* src)
{
	AudioSource_waveOut* s = (AudioSource_waveOut*)src;
	if (!s->m_waveOut)
	{
		if (waveOutOpen(&s->m_waveOut, WAVE_MAPPER, &s->m_waveformatex, (DWORD)AudioEngine_waveOutProc, 0, CALLBACK_FUNCTION)
			!= MMSYSERR_NOERROR)
			return;
	}

	s->m_waveHdr.lpData = (char*)src->m_data->m_data;
	s->m_waveHdr.dwBufferLength = src->m_data->m_dataSize;
	
	if (s->m_waveHdr.dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(s->m_waveOut, &s->m_waveHdr, sizeof(WAVEHDR));

	waveOutPrepareHeader(s->m_waveOut, &s->m_waveHdr, sizeof(WAVEHDR));
	if (s->m_waveHdr.dwFlags & WHDR_PREPARED)
	{
		int err = waveOutWrite(s->m_waveOut, &s->m_waveHdr, sizeof(WAVEHDR));
	//	if (err != MMSYSERR_NOERROR)
	}


}

#endif

