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
#ifdef MGF_BACKEND_D3D11

#include <d3dcompiler.h>

#include "framework/Context.h"
#include "framework/SystemWindow.h"
#include "framework/BackendD3D11.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"

#include "framework/GSD3D11.h"

extern PFND3DCOMPILEPROC g_D3DCompile;

using namespace mgf;

extern mgf::Backend* g_backend;

void BackendD3D11_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	g_backend->GetTextSize(text, font, sz);
}

mgTexture* BackendD3D11_createTexture(mgImage* img)
{
	return g_backend->CreateTexture(img);
}

void BackendD3D11_destroyTexture(mgTexture* t)
{
	g_backend->DestroyTexture(t);
}

void BackendD3D11_beginDraw()
{
	g_backend->BeginDraw();
}

void BackendD3D11_endDraw()
{
	g_backend->EndDraw();
}

void BackendD3D11_drawRectangle(
	int reason,
	void* object,
	mgRect* rct,
	mgColor* color1,
	mgColor* color2,
	mgTexture* texture, /*optional*/
	mgVec4* UVRegion)
{
	g_backend->DrawRectangle(reason, object, rct, color1, color2, texture, UVRegion);
}

void BackendD3D11_drawText(
	int reason,
	void* object,
	mgPoint* position,
	const wchar_t* text,
	int textLen,
	mgColor* color,
	mgFont* font)
{
	g_backend->DrawText(reason, object, position, text, textLen, color, font);
}

void BackendD3D11_drawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	g_backend->DrawLine(reason, object, position, where, color, size);
}

mgRect BackendD3D11_setClipRect(mgRect* r)
{
	return g_backend->SetClipRect(r);
}

BackendD3D11::BackendD3D11(BackendD3D11Params params)
{
	m_params = params;
	m_getTextSize = BackendD3D11_getTextSize;
	g_backend = this;
	m_gpu = new mgVideoDriverAPI;
	((mgVideoDriverAPI*)m_gpu)->createTexture = BackendD3D11_createTexture;
	((mgVideoDriverAPI*)m_gpu)->destroyTexture = BackendD3D11_destroyTexture;
	((mgVideoDriverAPI*)m_gpu)->beginDraw = BackendD3D11_beginDraw;
	((mgVideoDriverAPI*)m_gpu)->endDraw = BackendD3D11_endDraw;
	((mgVideoDriverAPI*)m_gpu)->drawRectangle = BackendD3D11_drawRectangle;
	((mgVideoDriverAPI*)m_gpu)->drawText = BackendD3D11_drawText;
	((mgVideoDriverAPI*)m_gpu)->setClipRect = BackendD3D11_setClipRect;
	
	//blackImage = new Image;
	//blackImage->Create(20, 20, mgColor(0xff020202));
	{
		mgf::Image whiteImage;
		whiteImage.Create(2, 2, mgColor(0xffFFFFFF));
		m_whiteTexture = CreateTexture(whiteImage.GetMGImage());
	}
	/*blackBitmap = new Gdiplus::Bitmap(blackImage->GetWidth(), blackImage->GetHeight(), blackImage->GetPitch(), 
		PixelFormat32bppARGB, blackImage->GetData());*/

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_params.m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled))) 
		throw "MGF D3D11: Can't create Direct3D 11 depth stencil state";

	D3D11_RASTERIZER_DESC	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.ScissorEnable = true;
	if (FAILED(m_params.m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling)))
		throw "MGF D3D11: Can not create rasterizer state";

	D3D11_BLEND_DESC  bd;
	memset(&bd, 0, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(m_params.m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled)))
		throw "MGF D3D11: Can't create blend state";

	_createShader();
}

BackendD3D11::~BackendD3D11()
{
	//if (blackImage)
		//delete blackImage;
	if (m_whiteTexture)
		this->DestroyTexture(m_whiteTexture);

	if (m_cbVertex) m_cbVertex->Release();
	if (m_vLayout) m_vLayout->Release();
	if (m_gShader) m_gShader->Release();
	if (m_vShader) m_vShader->Release();
	if (m_pShader) m_pShader->Release();

	if (m_depthStencilStateDisabled) m_depthStencilStateDisabled->Release();
	if (m_RasterizerSolidNoBackFaceCulling) m_RasterizerSolidNoBackFaceCulling->Release();
	if (m_blendStateAlphaEnabled) m_blendStateAlphaEnabled->Release();

	if (m_defaultFont)
		m_defaultFont->Release();

	if (m_defaultIcons)
	{
		/*if(m_defaultIcons->implementation)
			delete ((Gdiplus::Bitmap*)m_defaultIcons->implementation);*/
		delete m_defaultIcons;
	}

	delete m_gpu;
}

bool BackendD3D11::_createShader() 
{
	const char* text =
		"cbuffer cbVertex{\n"
		"	float4x4 ProjMtx;\n"
		"	float4 Corners;\n"
		"	float4 Color1;\n"
		"	float4 Color2;\n"
		"	float4 UVs;\n"
		"};\n"

		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float4 color : COLOR;\n"
		"};\n"

		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float2 uv : TEXCOORD0;\n"
		"	float4 color : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"

		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos   = float4(0.f,0.f,0.f,0.f);\n"
		"	output.uv = float2(0.f,0.f);\n"
		"	output.color = float4(0.f,0.f,0.f,0.f);\n"
		"	return output;\n"
		"}\n"

		"[maxvertexcount(4)]\n"
		"void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream ){\n"
		"   float4 v1 = mul(ProjMtx, float4(Corners.x, Corners.y, 0.f, 1.f) );\n"
		"   float4 v2 = mul(ProjMtx, float4(Corners.z, Corners.w, 0.f, 1.f) );\n"

		"	VSOut Out;\n"

		"	Out.pos   = float4(v2.x, v1.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.z,UVs.y);\n"
		"	Out.color = Color1;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v2.x, v2.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.z,UVs.w);\n"
		"	Out.color = Color2;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v1.x, v1.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.x,UVs.y);\n"
		"	Out.color = Color1;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v1.x, v2.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.x,UVs.w);\n"
		"	Out.color = Color2;\n"
		"	TriStream.Append(Out);\n"

		"	TriStream.RestartStrip();\n"

		"	return;\n"
		"}\n"

		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    float4 textureColor = tex2d_1.Sample(tex2D_sampler_1, input.uv);\n"
		"    output.color = float4(input.color.xyz, 1.f) * textureColor;\n"
		"    output.color.w = textureColor.w;\n"
		"    return output;\n"
		"}\n";
	if (!D3D11_createShaders("vs_4_0", "ps_4_0", text, text, "VSMain", "PSMain", &this->m_vShader, &this->m_pShader, &this->m_vLayout))
		return false;

	if (!D3D11_createGeometryShaders("gs_4_0", text, "GSMain", &m_gShader))
		return false;

	if (!D3D11_createConstantBuffer(sizeof(cbVertex), &m_cbVertex))
		return false;

	return true;
}

bool BackendD3D11::D3D11_createConstantBuffer(int byteSize, ID3D11Buffer** cb) {
	D3D11_BUFFER_DESC mbd;
	memset(&mbd, 0, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = byteSize;
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_params.m_d3d11Device->CreateBuffer(&mbd, 0, cb))) {
		printf("Can't create constant buffer\n");
		return false;
	}
	return true;
}

bool BackendD3D11::D3D11_createGeometryShaders(const char* target, const char* shaderText, const char* entryPoint, ID3D11GeometryShader** gs) {
	ID3D10Blob* m_GsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;
	HRESULT hr = g_D3DCompile(shaderText, strlen(shaderText), 0, 0, 0, entryPoint, target, 0, 0, &m_GsBlob, &m_errorBlob);
	if (FAILED(hr)) {
		char* message = (char*)m_errorBlob->GetBufferPointer();
		printf("Geometry shader compile error: %s\n", message);
		return false;
	}

	hr = m_params.m_d3d11Device->CreateGeometryShader(m_GsBlob->GetBufferPointer(), m_GsBlob->GetBufferSize(), 0, gs);
	if (FAILED(hr)) {
		printf("Can't create geometry shader. Error code [%u]\n", hr);
		return false;
	}
	return true;
}

bool BackendD3D11::D3D11_createShaders(const char* vertexTarget, const char* pixelTarget, const char* vertexShader,
	const char* pixelShader, const char* vertexEntryPoint, const char* pixelEntryPoint,
	ID3D11VertexShader** vs, ID3D11PixelShader** ps, ID3D11InputLayout** il)
{
	ID3D10Blob* m_VsBlob = nullptr;
	ID3D10Blob* m_PsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;

	HRESULT hr = g_D3DCompile(vertexShader, strlen(vertexShader), 0, 0, 0, vertexEntryPoint, vertexTarget, 0, 0, &m_VsBlob, &m_errorBlob);

	if (FAILED(hr)) {
		char* message = (char*)m_errorBlob->GetBufferPointer();
		printf("Vertex shader compile error: %s\n", message);
		return false;
	}

	hr = g_D3DCompile(pixelShader, strlen(pixelShader), 0, 0, 0, pixelEntryPoint, pixelTarget, 0, 0, &m_PsBlob, &m_errorBlob);

	if (FAILED(hr)) {
		char* message = (char*)m_errorBlob->GetBufferPointer();
		printf("Pixel shader compile error: %s\n", message);
		return false;
	}

	hr = m_params.m_d3d11Device->CreateVertexShader(m_VsBlob->GetBufferPointer(), m_VsBlob->GetBufferSize(), 0, vs);
	if (FAILED(hr)) {
		printf("Can't create vertex shader. Error code [%u]\n", hr);
		return false;
	}

	hr = m_params.m_d3d11Device->CreatePixelShader(m_PsBlob->GetBufferPointer(), m_PsBlob->GetBufferSize(), 0, ps);
	if (FAILED(hr)) {
		printf("Can't create pixel shader. Error code [%u]\n", hr);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexLayout[8];
	int vertexLayoutSize = 0;
	/*
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	*/

	int ind = 0;
	ind = 0;
	vertexLayout[ind].SemanticName = "POSITION";
	vertexLayout[ind].SemanticIndex = 0;
	vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexLayout[ind].InputSlot = 0;
	vertexLayout[ind].AlignedByteOffset = 0;
	vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[ind].InstanceDataStepRate = 0;

	ind++;
	vertexLayout[ind].SemanticName = "COLOR";
	vertexLayout[ind].SemanticIndex = 0;
	vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexLayout[ind].InputSlot = 0;
	vertexLayout[ind].AlignedByteOffset = 12;
	vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[ind].InstanceDataStepRate = 0;
	vertexLayoutSize = ind + 1;

	hr = m_params.m_d3d11Device->CreateInputLayout(vertexLayout, vertexLayoutSize, m_VsBlob->GetBufferPointer(), m_VsBlob->GetBufferSize(), il);
	if (FAILED(hr)) {
		printf("Can't create input layout. Error code [%u]\n", hr);
		return false;
	}

	if (m_VsBlob)    m_VsBlob->Release();
	if (m_PsBlob)    m_PsBlob->Release();
	if (m_errorBlob) m_errorBlob->Release();

	return true;
}

mgTexture_s* BackendD3D11::GetDefaultIcons()
{

	if (m_defaultIcons)
		return m_defaultIcons;

	//m_defaultIcons = new mgTexture_s;
	//m_defaultIcons->implementation = new Gdiplus::Bitmap(L"../data/icons.png");
	mgf::Image* img = mgf::GetFramework()->LoadImage("../data/icons.png");
	img->Convert(mgImageType_r8g8b8a8);
	m_defaultIcons = CreateTexture(img->GetMGImage());
	delete img;
	
	return m_defaultIcons;
}

void* BackendD3D11::GetVideoDriverAPI()
{
	return m_gpu;
}

void BackendD3D11::BeginDraw()
{
	m_params.m_d3d11DevCon->IASetInputLayout(m_vLayout);
	m_params.m_d3d11DevCon->VSSetShader(m_vShader, 0, 0);
	m_params.m_d3d11DevCon->GSSetShader(m_gShader, 0, 0);
	m_params.m_d3d11DevCon->PSSetShader(m_pShader, 0, 0);
	
	m_params.m_d3d11DevCon->OMGetDepthStencilState(&m_oldDepthStencilState, &m_oldDepthStencilRef);
	m_params.m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);

	m_params.m_d3d11DevCon->RSGetState(&m_oldRasterizerState);
	m_params.m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

	m_params.m_d3d11DevCon->OMGetBlendState(&m_oldBlendState, m_oldBlendStateBlendFactor, &m_oldBlendStateSampleMask);
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_params.m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);
}

void BackendD3D11::EndDraw()
{
	m_params.m_d3d11DevCon->OMSetDepthStencilState(m_oldDepthStencilState, m_oldDepthStencilRef);
	m_params.m_d3d11DevCon->RSSetState(m_oldRasterizerState);
	m_params.m_d3d11DevCon->OMSetBlendState(m_oldBlendState, m_oldBlendStateBlendFactor, m_oldBlendStateSampleMask);
}

mgTexture* BackendD3D11::CreateTexture(mgImage* img)
{
	if (img->type == mgImageType_unknown)
		return 0;

	BackendD3D11_texture* d3d11Texture = new BackendD3D11_texture;
	D3D11_FILTER filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = img->width;
	desc.Height = img->height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = img->data;
	initData.SysMemPitch = img->width * 4;
	initData.SysMemSlicePitch = img->dataSize;
	auto hr = m_params.m_d3d11Device->CreateTexture2D(&desc, &initData, &d3d11Texture->m_texture);
	if (FAILED(hr))
	{
		printf("Can't create 2D texture\n");
		delete d3d11Texture;
		return 0;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = m_params.m_d3d11Device->CreateShaderResourceView(d3d11Texture->m_texture,
		&SRVDesc, &d3d11Texture->m_textureResView);
	if (FAILED(hr))
	{
		printf("Can't create shader resource view\n");
		delete d3d11Texture;
		return 0;
	}

	D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_COMPARISON_FUNC cmpFunc = D3D11_COMPARISON_NEVER;

	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = filter;
		samplerDesc.MipLODBias = 0.0f;

		samplerDesc.AddressU = addressMode;
		samplerDesc.AddressV = addressMode;
		samplerDesc.AddressW = addressMode;

		samplerDesc.ComparisonFunc = cmpFunc;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		samplerDesc.MaxAnisotropy = 1;

		if (FAILED(m_params.m_d3d11Device->CreateSamplerState(&samplerDesc, &d3d11Texture->m_samplerState)))
		{
			printf("Can't create sampler state\n");
			delete d3d11Texture;
			return 0;
		}
	}

	mgTexture_s* newT = new mgTexture_s;
	newT->implementation = d3d11Texture;
	return newT;
}

void BackendD3D11::DestroyTexture(mgTexture* t)
{
	BackendD3D11_texture* hwdata = (BackendD3D11_texture*)t->implementation;
	delete (hwdata);
	delete t;
}

void BackendD3D11::_drawRectangle()
{
	/*m_cbVertex_impl.m_Corners.x = m_drrect_rect.left;
	m_cbVertex_impl.m_Corners.y = m_drrect_rect.top;
	m_cbVertex_impl.m_Corners.z = m_drrect_rect.right;
	m_cbVertex_impl.m_Corners.w = m_drrect_rect.bottom;*/
	//m_cbVertex_impl.m_Color1 = *m_drrect_color1;
	//m_cbVertex_impl.m_Color2 = *m_drrect_color2;
	//m_cbVertex_impl.m_UVs = m_drrect_tcoords;
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D11_BUFFER_DESC d;
		m_params.m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbVertex->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbVertex_impl, d.ByteWidth);
		m_params.m_d3d11DevCon->Unmap(m_cbVertex, 0);
		m_params.m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
		m_params.m_d3d11DevCon->GSSetConstantBuffers(0, 1, &m_cbVertex);
	}
	m_params.m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_params.m_d3d11DevCon->Draw(1, 0);
}
void BackendD3D11::DrawRectangle(int reason, void* object, mgRect* rct, mgColor* color1, mgColor* color2,
	mgTexture* texture, mgVec4* UVRegion)
{
	m_cbVertex_impl.m_Color1 = *color1;
	m_cbVertex_impl.m_Color2 = *color2;

	/*m_drrect_rect.left = (float)(rct->left);
	m_drrect_rect.top = (float)(rct->top);
	m_drrect_rect.right = (float)(rct->right);
	m_drrect_rect.bottom = (float)(rct->bottom);*/
	m_cbVertex_impl.m_Corners.x = (float)(rct->left);
	m_cbVertex_impl.m_Corners.y = (float)(rct->top);
	m_cbVertex_impl.m_Corners.z = (float)(rct->right);
	m_cbVertex_impl.m_Corners.w = (float)(rct->bottom);

	/*m_drrect_tcoords.x = 0.f;
	m_drrect_tcoords.y = 0.f;
	m_drrect_tcoords.z = 1.f;
	m_drrect_tcoords.w = 1.f;*/
	m_cbVertex_impl.m_UVs.x = 0.f;
	m_cbVertex_impl.m_UVs.y = 0.f;
	m_cbVertex_impl.m_UVs.z = 1.f;
	m_cbVertex_impl.m_UVs.w = 1.f;

	m_activeTexture = (BackendD3D11_texture*)m_whiteTexture->implementation;

	if (texture)
	{
		BackendD3D11_texture* timpl = (BackendD3D11_texture*)texture->implementation;

		m_activeTexture = timpl;

		if (UVRegion)
		{
			//m_drrect_tcoords = *UVRegion;
			m_cbVertex_impl.m_UVs = *UVRegion;
		}
		else
		{
			m_cbVertex_impl.m_UVs = m_context->m_gui_context->currentIcon->uv;
		}
	}

	m_params.m_d3d11DevCon->PSSetShaderResources(0, 1, &m_activeTexture->m_textureResView);
	m_params.m_d3d11DevCon->PSSetSamplers(0, 1, &m_activeTexture->m_samplerState);

	bool draw = true;

	switch (reason)
	{
	case mgDrawRectangleReason_listItemBG1:
	case mgDrawRectangleReason_listItemBG2:
		break;
	default:
		if (reason == mgDrawRectangleReason_popupSeparator)
		{
			m_drrect_rect.top += 2;
			m_drrect_rect.bottom -= 2;
			m_drrect_rect.left += 2;
			m_drrect_rect.right -= 2;
		}

		_drawRectangle();

		if (reason == mgDrawRectangleReason_popupBG)
		{
			mgColor c;
			c.setAsIntegerRGB(0x666666);
			/*glBegin(GL_LINES);
			glColor4fv(&c.r);
			glVertex3f(m_drrect_rect.left, m_drrect_rect.top + 1, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.top + 1, 0.0f);
			glVertex3f(m_drrect_rect.left, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.left + 1, m_drrect_rect.top, 0.0f);
			glVertex3f(m_drrect_rect.left + 1, m_drrect_rect.bottom, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.top, 0.0f);
			glVertex3f(m_drrect_rect.right, m_drrect_rect.bottom, 0.0f);
			glEnd();*/
		}
		break;
	}
	//glScissor(0, 0, m_window->m_size.x, m_window->m_size.y);
}

void BackendD3D11::DrawText(int reason, void* object, mgPoint* position, const wchar_t* text, int textLen,
	mgColor* color, mgFont* font)
{
	m_cbVertex_impl.m_Color1 = *color;
	m_cbVertex_impl.m_Color2 = *color;

	mgPoint _position = *position;
	for (int i = 0; i < textLen; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			_position.x += glyph->underhang;
			mgVec4 corners;
			corners.x = (float)_position.x;
			corners.y = (float)_position.y;

			corners.z = corners.x + glyph->width;
			corners.w = corners.y + glyph->height;

			BackendD3D11_texture* texture = (BackendD3D11_texture*)((mgFontBitmap*)font->implementation)[glyph->textureSlot].gpuTexture->implementation;

			/*m_drrect_rect.left = corners.x;
			m_drrect_rect.top = corners.y;
			m_drrect_rect.right = corners.z;
			m_drrect_rect.bottom = corners.w;*/
			m_cbVertex_impl.m_Corners = corners;

			m_params.m_d3d11DevCon->PSSetShaderResources(0, 1, &texture->m_textureResView);
			m_params.m_d3d11DevCon->PSSetSamplers(0, 1, &texture->m_samplerState);
			//m_drrect_tcoords = glyph->UV;
			m_cbVertex_impl.m_UVs = glyph->UV;
			_drawRectangle();

			_position.x += glyph->width + glyph->overhang + font->characterSpacing;
			if (character == L' ')
				_position.x += font->spaceSize;
			if (character == L'\t')
				_position.x += font->tabSize;
		}
	}
}

void BackendD3D11::DrawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	/*Gdiplus::Graphics graphics(m_window->m_hdcMem);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);*/
}

mgRect BackendD3D11::SetClipRect(mgRect* r)
{
	mgRect old = m_clipRect;
	m_clipRect = *r;
	//int y = m_window->m_size.y - m_clipRect.top;
	//int w = m_clipRect.bottom - m_clipRect.top;
	D3D11_RECT _r;
	_r.left = r->left;
	_r.top = r->top;
	_r.right = r->right;
	_r.bottom = r->bottom;
	m_params.m_d3d11DevCon->RSSetScissorRects(1, &_r);
	return old;
}


void BackendD3D11::_createBackbuffer(mgf::SystemWindow* impl)
{
}

void BackendD3D11::InitWindow(mgf::SystemWindow* w)
{
	if (m_window->m_hdcMem)
		return;
//	_createBackbuffer(m_window);
}

void BackendD3D11::SetActiveWindow(mgf::SystemWindow* w)
{
	m_window = w;
}

void BackendD3D11::SetActiveContext(mgf::Context* c)
{
	m_context = c;
}

void BackendD3D11::UpdateBackbuffer()
{
	/*if (m_window->m_hdcMem)
		DeleteDC(m_window->m_hdcMem);
	if (m_window->m_hbmMem)
		DeleteObject(m_window->m_hbmMem);
	
	glViewport(0, 0, m_window->m_size.x, m_window->m_size.y);*/
	
	m_cbVertex_impl.m_ProjMtx.m_data[0] = mgf::v4f(2.0f / m_window->m_size.x, 0.0f, 0.0f, 0.0f);
	m_cbVertex_impl.m_ProjMtx.m_data[1] = mgf::v4f(0.0f, 2.0f / -m_window->m_size.y, 0.0f, 0.0f);
	m_cbVertex_impl.m_ProjMtx.m_data[2] = mgf::v4f(0.0f, 0.0f, 0.5f, 0.0f);
	m_cbVertex_impl.m_ProjMtx.m_data[3] = mgf::v4f(-1.f, 1.f, 0.5f, 1.0f);
	//_createBackbuffer(m_window);
}

void BackendD3D11::GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	sz->x = 0;
	sz->y = 0;
	int c = wcslen(text);
	if (!c)
		return;
	for (int i = 0; i < c; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			if (glyph->height > sz->y)
				sz->y = glyph->height;

			sz->x += glyph->width + glyph->overhang + glyph->underhang + font->characterSpacing;
			if (character == L' ')
				sz->x += font->spaceSize;
			if (character == L'\t')
				sz->x += font->tabSize;
		}
	}
}

Font* BackendD3D11::CreateFont(const wchar_t* file, int size, bool bold, bool italic)
{
	StringW str = file;
	StringA stra = str.to_utf8();

	int flags = 0;

	if (bold)
		flags |= MG_FNTFL_BOLD;
	if (italic)
		flags |= MG_FNTFL_ITALIC;

	FontImpl* newFont = new FontImpl;
	newFont->m_context = this->m_context;

	newFont->m_font = mgCreateFont(this->m_context->m_gui_context, stra.data(), flags, size, 0);

	newFont->m_backend = this;
	return newFont;
}

void BackendD3D11::DestroyFont(Font* f)
{
	if (!f)
		return;
	FontImpl* fi = (FontImpl*)f;
	if (fi->m_font)
	{
		if (fi->m_font->implementation)
			DeleteObject(fi->m_font->implementation);
		free(fi->m_font);
	}
}

Font* BackendD3D11::GetDefaultFont()
{
	if(!m_defaultFont)
		m_defaultFont = this->CreateFont(L"Arial", 10, 1, 0);

	return m_defaultFont;
}

#endif
