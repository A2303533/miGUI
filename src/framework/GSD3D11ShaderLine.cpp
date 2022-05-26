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
#include "GSD3D11ShaderLine.h"
#include "Log.h"

using namespace mgf;

GSD3D11ShaderLine::GSD3D11ShaderLine(ID3D11Device* d, ID3D11DeviceContext* dc)
	:
	m_device(d),
	m_deviceCotext(dc)
{
}

GSD3D11ShaderLine::~GSD3D11ShaderLine()
{
}

bool GSD3D11ShaderLine::init()
{
	const char* text =
		"struct VSIn{\n"
		"	uint vertexID : SV_VertexID;\n"
		"};\n"
		"cbuffer cbVertex  : register(b0) {\n"
		"	float4x4 VP;\n"
		"	float4 P1;\n"
		"	float4 P2;\n"
		"	float4 Color;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"   float4 color : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"

		"VSOut VSMain(VSIn input){\n"
		"float4 vertices[2] = {\n"
		"float4( P1.xyz, 1.0),\n"
		"float4( P2.xyz, 1.0)\n"
		"};\n"
		"   VSOut output;\n"
		"	output.pos   = mul(VP, vertices[input.vertexID]);\n"
		"	output.color = Color;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    output.color = input.color;\n"
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
		Mesh::MeshVertexType_Null,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		mgf::LogWriteError("%s\n", "D3D11: Can't create shader Line");
		return false;
	}

	if (!GSD3D11_createConstantBuffer(m_device, sizeof(cb), &m_cb))
	{
		mgf::LogWriteError("%s\n", "D3D11: Can't create constant buffer for shader Line");
		return false;
	}

	return true;
}

void GSD3D11ShaderLine::SetConstants(void*)
{
	m_deviceCotext->VSSetConstantBuffers(0, 1, &m_cb);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_deviceCotext->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbData, d.ByteWidth);
	m_deviceCotext->Unmap(m_cb, 0);
}

void GSD3D11ShaderLine::SetData(const v4f& p1, const v4f& p2, const mgColor& c, Mat4* projMat)
{
	m_cbData.P1 = p1;
	m_cbData.P2 = p2;
	m_cbData.Color = c;
	m_cbData.VP = *projMat; // g_d3d11->m_matrixViewProjection;
}

#endif


