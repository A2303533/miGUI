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
#ifdef MGF_GS_D3D11

#include "GSD3D11.h"
#include <d3dcompiler.h>
#include "GSD3D11Shader.h"
#include "GSD3D11ShaderScreenQuad.h"
#include "Log.h"

using namespace mgf;

GSD3D11ShaderScreenQuad::GSD3D11ShaderScreenQuad(ID3D11Device* d)
	:
	m_device(d)
{
}

GSD3D11ShaderScreenQuad::~GSD3D11ShaderScreenQuad()
{
}

bool GSD3D11ShaderScreenQuad::init()
{
	const char * text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float2 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float2 uv : TEXCOORD0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos   = float4(input.position.x, input.position.y, 0.5f, 1.f);\n"
		"	output.uv    = input.uv;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n" 
		"    PSOut output;\n"
		"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv);\n"
		"    output.color.w = 1.f;\n"
		"    return output;\n"
		"}\n";
	if (!GSD3D11_createShaders(
		m_device,
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		Mesh::MeshVertexType_GUI,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		mgf::LogWriteError("%s\n", "D3D11: Can't create shader Screen Quad");
		return false;
	}

	return true;
}

#endif

