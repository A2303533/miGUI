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

#ifdef MG_PLATFORM_WINDOWS
#include <windows.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#endif

struct AudioEngineImplementationData
{
#ifdef MG_PLATFORM_WINDOWS
	IMMDevice* m_device = 0;
	HANDLE      _ShutdownEvent = 0;
#endif
};


mgf::AudioEngine::AudioEngine()
{
#ifdef MG_PLATFORM_WINDOWS
	AudioEngineImplementationData* iData = new AudioEngineImplementationData;
	m_implementationData = iData;

	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
		throw "AudioEngine: Unable to instantiate device enumerator";

	ERole deviceRole = eMultimedia;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &iData->m_device);
	if (FAILED(hr))
		throw "AudioEngine: Unable to get default device";
	if (deviceCollection)
		deviceCollection->Release();
	if (deviceEnumerator)
		deviceEnumerator->Release();

	iData->_ShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
	if (iData->_ShutdownEvent == NULL)
		throw "AudioEngine: Unable to create shutdown event";
#endif
}

mgf::AudioEngine::~AudioEngine()
{
	AudioEngineImplementationData* iData = (AudioEngineImplementationData*)m_implementationData;

#ifdef MG_PLATFORM_WINDOWS
	if (iData->m_device)
		iData->m_device->Release();
#endif
	delete ((AudioEngineImplementationData*)m_implementationData);
}
