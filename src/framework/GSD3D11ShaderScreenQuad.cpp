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
		MeshVertexType::GUI,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		return false;
	}

	return true;
}

#endif

