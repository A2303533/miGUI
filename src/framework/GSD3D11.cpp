#include "mgf.h"

#ifdef MGF_GS_D3D11

#include "GSD3D11.h"

#include "GSD3D11Texture.h"
#include "GSD3D11Mesh.h"
#include "GSD3D11Shader.h"
#include "GSD3D11ShaderScreenQuad.h"

#include "framework/Image.h"
#include "framework/BackendD3D11.h"
#include "framework/Log.h"


using namespace mgf;

PFND3DCOMPILEPROC g_D3DCompile = 0;
PFND3D11CREATEDEVICEPROC g_D3D11CreateDevice = 0;

#define MGFD3D_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

MGFD3D_DEFINE_GUID(MGF_D3D_IID_IDXGIFactory, 0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69);
MGFD3D_DEFINE_GUID(MGF_D3D_IID_IDXGIAdapter, 0x2411e7e1, 0x12ac, 0x4ccf, 0xbd, 0x14, 0x97, 0x98, 0xe8, 0x53, 0x4d, 0xc0);
MGFD3D_DEFINE_GUID(MGF_D3D_IID_IDXGIDevice, 0x54ec77fa, 0x1377, 0x44e6, 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c);
MGFD3D_DEFINE_GUID(MGF_D3D_IID_ID3D11Texture2D, 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c);


GSD3D11::GSD3D11()
{
}

GSD3D11::~GSD3D11()
{
	if (m_shaderScreenQuad) delete m_shaderScreenQuad;

	if (m_whiteTexture) m_whiteTexture->Release();

	if (m_mainTarget) m_mainTarget->Release();
	if (m_mainTargetSurface) m_mainTargetSurface->Release();

	if (m_blendStateAlphaDisabled)              m_blendStateAlphaDisabled->Release();
	if (m_blendStateAlphaEnabledWithATC)        m_blendStateAlphaEnabledWithATC->Release();
	if (m_blendStateAlphaEnabled)               m_blendStateAlphaEnabled->Release();
	if (m_RasterizerWireframeNoBackFaceCulling) m_RasterizerWireframeNoBackFaceCulling->Release();
	if (m_RasterizerWireframe)                  m_RasterizerWireframe->Release();
	if (m_RasterizerSolidNoBackFaceCulling)     m_RasterizerSolidNoBackFaceCulling->Release();
	if (m_RasterizerSolid)                      m_RasterizerSolid->Release();
	if (m_depthStencilView)                     m_depthStencilView->Release();
	if (m_depthStencilStateDisabled)            m_depthStencilStateDisabled->Release();
	if (m_depthStencilStateEnabled)             m_depthStencilStateEnabled->Release();
	if (m_depthStencilBuffer)                   m_depthStencilBuffer->Release();
	if (m_MainTargetView)                       m_MainTargetView->Release();
	if (m_d3d11DevCon)                          m_d3d11DevCon->Release();
	if (m_SwapChain)                            m_SwapChain->Release();

	if (m_d3d11Device)                          m_d3d11Device->Release();

	if (m_dllD3D11) FreeLibrary(m_dllD3D11);
	if (m_dllD3DCompiler) FreeLibrary(m_dllD3DCompiler);
}

bool GSD3D11::Init(SystemWindow* window, BackendD3D11Params* outParams)
{
	m_dllD3DCompiler = LoadLibraryA("d3dcompiler_47.dll");
	if (!m_dllD3DCompiler)
	{
		LogWriteError("Can't load d3dcompiler_47.dll");
		return false;
	}

	g_D3DCompile = (PFND3DCOMPILEPROC)GetProcAddress(m_dllD3DCompiler, "D3DCompile");
	if (!g_D3DCompile)
	{
		LogWriteError("Can't get procedure address (D3DCompile) from d3dcompiler_47.dll");
		return false;
	}

	m_dllD3D11 = LoadLibraryA("D3D11.dll");
	if (!m_dllD3D11)
	{
		LogWriteError("Can't load D3D11.dll");
		return false;
	}

	g_D3D11CreateDevice = (PFND3D11CREATEDEVICEPROC)GetProcAddress(m_dllD3D11, "D3D11CreateDevice");
	if (!g_D3D11CreateDevice)
	{
		LogWriteError("Can't get procedure address (D3D11CreateDevice) from D3D11.dll");
		return false;
	}

	mgPoint windowSize = window->GetSize();

	HWND hWnd = (HWND)window->GetOSData()->handle;

	m_activeWindow = window;
	m_mainTargetSize.x = (float)windowSize.x;
	m_mainTargetSize.y = (float)windowSize.y;

	DXGI_MODE_DESC	bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Width = windowSize.x;
	bufferDesc.Height = windowSize.y;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;

	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	auto hr = g_D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_d3d11Device,
		&featureLevel,
		&m_d3d11DevCon);

	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory1* dxgiFactory = nullptr;
	hr = m_d3d11Device->QueryInterface(MGF_D3D_IID_IDXGIDevice, (void**)&dxgiDevice);
	if (FAILED(hr))
	{
		LogWriteError("Can't QueryInterface : IID_IDXGIDevice");
		return false;
	}

	hr = dxgiDevice->GetParent(MGF_D3D_IID_IDXGIAdapter, (void**)&dxgiAdapter);
	if (FAILED(hr))
	{
		LogWriteError("Can't get DXGI adapter");
		return false;
	}

	hr = dxgiAdapter->GetParent(MGF_D3D_IID_IDXGIFactory, (void**)&dxgiFactory);
	if (FAILED(hr))
	{
		LogWriteError("Can't get DXGI factory");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC	swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true/*m_params.m_fullScreen ? false : true*/;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	hr = dxgiFactory->CreateSwapChain(m_d3d11Device, &swapChainDesc, &m_SwapChain);
	if (FAILED(hr))
	{
		LogWriteError("Can't create Swap Chain");
		return false;
	}

	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	_createBackBuffer(windowSize.x, windowSize.y);

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
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

	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStateEnabled)))
	{
		LogWriteError("Can't create Direct3D 11 depth stencil state");
		return false;
	}

	m_d3d11DevCon->OMSetDepthStencilState(this->m_depthStencilStateEnabled, 0);

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled)))
	{
		LogWriteError("Can't create Direct3D 11 depth stencil state");
		return false;
	}

	D3D11_RASTERIZER_DESC	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolid)))
	{
		LogWriteError("Can not create rasterizer state");
		return false;
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling);
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframeNoBackFaceCulling);
	rasterDesc.CullMode = D3D11_CULL_BACK;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframe);

	m_d3d11DevCon->RSSetState(m_RasterizerSolid);

	D3D11_BLEND_DESC  bd;
	memset(&bd, 0, sizeof(bd));
	bd.AlphaToCoverageEnable = 0;
	bd.RenderTarget[0].BlendEnable = TRUE;

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled)))
	{
		LogWriteError("Can't create Direct3D 11 blend state");
		return false;
	}

	bd.AlphaToCoverageEnable = 1;
	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabledWithATC)))
	{
		LogWriteError("Can't create Direct3D 11 blend state");
		return false;
	}

	bd.RenderTarget[0].BlendEnable = FALSE;
	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaDisabled)))
	{
		LogWriteError("Can't create Direct3D 11 blend state");
		return false;
	}

	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	//	if (atc)
		//	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabledWithATC, blendFactor, 0xffffffff);
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);

	D3D11_RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = (LONG)windowSize.x;
	sr.bottom = (LONG)windowSize.y;
	m_d3d11DevCon->RSSetScissorRects(1, &sr);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)windowSize.x;
	viewport.Height = (float)windowSize.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3d11DevCon->RSSetViewports(1, &viewport);

	UpdateGUIProjection();

	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0);

	{
		GSTextureInfo inf;
		Image img;
		img.Create(10, 10, 0xFFFFFFFF);
		inf.m_imagePtr = img.GetMGImage();
		inf.m_width = img.GetWidth();
		inf.m_height = img.GetHeight();
		m_whiteTexture = (GSD3D11Texture*)this->CreateTexture(&inf);
	}

	if (outParams)
	{
		outParams->m_d3d11Device = m_d3d11Device;
		outParams->m_d3d11DevCon = m_d3d11DevCon;
	}

	m_shaderScreenQuad = new GSD3D11ShaderScreenQuad(m_d3d11Device);
	m_shaderScreenQuad->init();

	m_mainTargetSize.set((float)windowSize.x, (float)windowSize.y);
	_updateMainTarget();

	return true;
}

void GSD3D11::UpdateGUIProjection()
{
	mgPoint sz = m_activeWindow->GetSize();

	mgf::Mat4 * m = &m_matrices[GS::MatrixType_Projection];

	/*m->m_data[0] = mgVec4(2.0f / x, 0.0f, 0.0f, 0.0f);
	m->m_data[1] = mgVec4(0.0f, 2.0f / -y, 0.0f, 0.0f);
	m->m_data[2] = mgVec4(0.0f, 0.0f, 0.5f, 0.0f);
	m->m_data[3] = mgVec4(-1.f, 1.f, 0.5f, 1.0f);*/

	m->m_data[0].x = 2.f / sz.x;
	m->m_data[1].y = 2.f / -sz.y;
	m->m_data[2].z = m->m_data[3].z = 0.5f;
	m->m_data[3].x = -1.f;
	m->m_data[3].y = m->m_data[3].w = 1.f;

	m->m_data[0].y = 
		m->m_data[0].z = 
		m->m_data[0].w = 
		m->m_data[1].x = 
		m->m_data[1].z = 
		m->m_data[1].w = 
		m->m_data[2].x =
		m->m_data[2].y =
		m->m_data[2].w =
		0.f;
}

void GSD3D11::UseVSync(bool v)
{
	m_vsync = v;
}

bool GSD3D11::UseDepth(bool v)
{
	v ? m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0)
		: m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);

	auto o = m_old_depth;
	m_old_depth = v;
	return o;
}

bool GSD3D11::UseBlend(bool v)
{
	const float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
	if (v)
		m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	else
		m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaDisabled, blend_factor, 0xffffffff);

	auto o = m_old_blend;
	m_old_blend = v;
	return o;
}

void GSD3D11::GetDepthRange(mgf::v2f* d)
{
	d->x = 0.f;
	d->y = 1.f;
}

void GSD3D11::SetClearColor(float* rgba)
{
	m_clearColor.r = rgba[0];
	m_clearColor.g = rgba[1];
	m_clearColor.b = rgba[2];
	m_clearColor.a = rgba[3];
}

void GSD3D11::BeginDraw()
{
	//m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);
	//m_currentTargetView = m_MainTargetView;

	m_d3d11DevCon->OMSetRenderTargets(1, &m_mainTarget->m_RTV, m_depthStencilView);
	SetViewport(0, 0, m_mainTargetSize.x, m_mainTargetSize.y, 0, 0);
	m_currentTargetView = m_mainTarget->m_RTV;
}

void GSD3D11::ClearDepth()
{
	m_d3d11DevCon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GSD3D11::ClearColor()
{
	m_d3d11DevCon->ClearRenderTargetView(m_currentTargetView, &m_clearColor.r);
}

void GSD3D11::ClearAll()
{
	m_d3d11DevCon->ClearRenderTargetView(m_currentTargetView, &m_clearColor.r);
	m_d3d11DevCon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GSD3D11::SetActiveShader(GSD3D11Shader* shader) {
	//	if (shader != m_activeShader)
	{
		m_activeShader = shader;
		m_d3d11DevCon->IASetInputLayout(shader->m_vLayout);
		m_d3d11DevCon->VSSetShader(shader->m_vShader, 0, 0);
		m_d3d11DevCon->GSSetShader(shader->m_gShader, 0, 0);
		m_d3d11DevCon->PSSetShader(shader->m_pShader, 0, 0);
	}
}

void GSD3D11::EndDraw()
{
	auto wsz = m_activeWindow->GetSize();

	m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);
	m_currentTargetView = m_MainTargetView;
	SetViewport(0, 0, (float)wsz.x, (float)wsz.y, 0, 0);
	SetScissorRect(mgf::v4f(0.f, 0.f, (float)wsz.x, (float)wsz.y), 0, 0);
	ClearColor();

	SetActiveShader(m_shaderScreenQuad);

	m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto old_depth = UseDepth(false);

	UseBlend(false);
	m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

	m_d3d11DevCon->PSSetShaderResources(0, 1, &m_mainTarget->m_textureResView);
	m_d3d11DevCon->PSSetSamplers(0, 1, &m_mainTarget->m_samplerState);
	uint32_t offset = 0u;
	m_d3d11DevCon->IASetVertexBuffers(0, 1, &m_mainTargetSurface->m_vBuffer, &m_mainTargetSurface->m_stride, &offset);
	m_d3d11DevCon->IASetIndexBuffer(m_mainTargetSurface->m_iBuffer, m_mainTargetSurface->m_indexType, 0);
	m_d3d11DevCon->DrawIndexed(m_mainTargetSurface->m_iCount, 0, 0);
	UseDepth(old_depth);
}

void GSD3D11::SwapBuffers()
{
	m_vsync ? m_SwapChain->Present(1, 0)
		: m_SwapChain->Present(0, 0);
}

void GSD3D11::UpdateMainRenderTarget(const mgf::v2f& bufferSize)
{
	m_mainTargetSize = bufferSize;
	_updateMainTarget();
}

GSTexture* GSD3D11::CreateTexture(GSTextureInfo* ti)
{
	return new GSD3D11Texture(ti, m_d3d11Device, m_d3d11DevCon);
}

GSTexture* GSD3D11::CreateRenderTargetTexture(uint32_t w, uint32_t h, GSTextureInfo* opt)
{
	GSTextureInfo ti;
	if (opt)
		ti = *opt;

	ti.m_textureType = GSTextureInfo::Type_RTT;
	ti.m_width = w;
	ti.m_height = h;

	return new GSD3D11Texture(&ti, m_d3d11Device, m_d3d11DevCon);
}

GSMesh* GSD3D11::CreateMesh(GSMeshInfo* mi)
{
	return new GSD3D11Mesh(mi, m_d3d11Device, m_d3d11DevCon);
}

void GSD3D11::SetRenderTarget(GSTexture* rtt)
{
	if (rtt)
	{
		GSD3D11Texture* t = (GSD3D11Texture*)rtt;
		m_d3d11DevCon->OMSetRenderTargets(1, &t->m_RTV, m_depthStencilView);
		m_currentTargetView = t->m_RTV;
	}
	else
	{
		m_d3d11DevCon->OMSetRenderTargets(1, &m_mainTarget->m_RTV, m_depthStencilView);
		m_currentTargetView = m_mainTarget->m_RTV;
	}
}

void GSD3D11::SetViewport(float x, float y, float width, float height, mgf::SystemWindow* window, mgf::v4f* old)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	m_d3d11DevCon->RSSetViewports(1, &viewport);
	
	if (old)
		old->set(m_old_viewport.x, m_old_viewport.y, m_old_viewport.z, m_old_viewport.w);
	m_old_viewport.set(x, y, width, height);
}

void GSD3D11::SetScissorRect(const mgf::v4f& rect, mgf::SystemWindow* window, mgf::v4f* old)
{
	D3D11_RECT r;
	r.left = (LONG)rect.x;
	r.top = (LONG)rect.y;
	r.right = (LONG)rect.z;
	r.bottom = (LONG)rect.w;
	m_d3d11DevCon->RSSetScissorRects(1, &r);

	if (old)
		old->set(m_old_scissor.x, m_old_scissor.y, m_old_scissor.z, m_old_scissor.w);
	m_old_scissor = rect;
}

const wchar_t* GSD3D11::GetVideoDriverName()
{
	return L"Direct3D 11";
}

void GSD3D11::SetTexture(uint32_t slot, GSTexture* res)
{
	if (res)
		m_currentTextures[(uint32_t)slot] = (GSD3D11Texture*)res;
	else
		m_currentTextures[slot] = nullptr;
}

void GSD3D11::SetMesh(GSMesh*  res)
{
	if (res)
		m_currentMesh = (GSD3D11Mesh*)res;
	else
		m_currentMesh = nullptr;
}

void GSD3D11::Draw()
{
	if (!m_currentMesh)
		return;
}

bool GSD3D11::_updateMainTarget()
{
	if (m_mainTarget) m_mainTarget->Release();
	if (m_mainTargetSurface) m_mainTargetSurface->Release();

	m_mainTarget = (GSD3D11Texture*)this->CreateRenderTargetTexture((uint32_t)m_mainTargetSize.x, (uint32_t)m_mainTargetSize.y, 0);

	auto mesh = new Mesh;
	mesh->m_iCount = 6;
	mesh->m_vCount = 4;
	mesh->m_stride = sizeof(VertexGUI);
	mesh->m_vertexType = Mesh::MeshVertexType_GUI;
	mesh->m_vertices = (uint8_t*)malloc(mesh->m_vCount * mesh->m_stride);
	mesh->m_indices = (uint8_t*)malloc(mesh->m_iCount * sizeof(uint16_t));
	uint16_t* inds = (uint16_t*)mesh->m_indices;

	VertexGUI* vertex = (VertexGUI*)mesh->m_vertices;
	vertex->Position.set(-1.0f, 1.0f);
	vertex->UV.set(0.f, 0.f);
	vertex++;
	vertex->Position.set(-1.0f, -1.0f);
	vertex->UV.set(0.f, 1.f);
	vertex++;
	vertex->Position.set(1.0f, -1.0f);
	vertex->UV.set(1.f, 1.f);
	vertex++;
	vertex->Position.set(1.0f, 1.0f);
	vertex->UV.set(1.f, 0.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	/*m_mainTargetSurface = miCreate<miD3D11Model>();
	m_mainTargetSurface->GetMeshInfo()->m_meshPtr = mesh;
	m_mainTargetSurface->Reload();*/
	GSMeshInfo mi;
	mi.m_meshPtr = mesh;
	m_mainTargetSurface = (GSD3D11Mesh*)CreateMesh(&mi);
	delete mesh;

	m_d3d11DevCon->OMSetRenderTargets(0, 0, 0);

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_MainTargetView)
	{
		m_MainTargetView->Release();
		m_MainTargetView = 0;
	}

	auto wsz = m_activeWindow->GetSize();

	//m_mainTargetSize.set(wsz.x, wsz.y);
	m_SwapChain->ResizeBuffers(0, (UINT)wsz.x, (UINT)wsz.y, DXGI_FORMAT_UNKNOWN, 0);
	_createBackBuffer(wsz.x, wsz.y);
	UpdateGUIProjection();

	return true;
}

bool GSD3D11::_createBackBuffer(uint32_t x, uint32_t y)
{
	if (x < 1) x = 1;
	if (y < 1) y = 1;

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_MainTargetView)
	{
		m_MainTargetView->Release();
		m_MainTargetView = 0;
	}

	ID3D11Texture2D* BackBuffer = 0;
	if (FAILED(m_SwapChain->GetBuffer(
		0,
		MGF_D3D_IID_ID3D11Texture2D,
		(void**)&BackBuffer)))
	{
		return false;
	}
	if (FAILED(this->m_d3d11Device->CreateRenderTargetView(
		BackBuffer, 0, &m_MainTargetView)))
	{
		if (BackBuffer) BackBuffer->Release();
		return false;
	}
	if (BackBuffer) BackBuffer->Release();
	D3D11_TEXTURE2D_DESC	DSD;
	ZeroMemory(&DSD, sizeof(DSD));
	DSD.Width = x;
	DSD.Height = y;
	DSD.MipLevels = 1;
	DSD.ArraySize = 1;
	DSD.Format = DXGI_FORMAT_D32_FLOAT;
	DSD.SampleDesc.Count = 1;
	DSD.SampleDesc.Quality = 0;
	DSD.Usage = D3D11_USAGE_DEFAULT;
	DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSD.CPUAccessFlags = 0;
	DSD.MiscFlags = 0;
	if (FAILED(m_d3d11Device->CreateTexture2D(&DSD, 0, &m_depthStencilBuffer)))
		return false;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC	m_depthStencilViewDesc;
	ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));

	m_depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_d3d11Device->CreateDepthStencilView(m_depthStencilBuffer, &m_depthStencilViewDesc, &m_depthStencilView)))
		return false;
	m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);
	return true;
}

void GSD3D11::GetTextureCopyForImage(GSTexture* t, Image* i)
{
	D3D11_TEXTURE2D_DESC desc;
	GSD3D11Texture* texture = (GSD3D11Texture*)t;
	texture->m_texture->GetDesc(&desc);

	desc.BindFlags = 0;
	desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* staging = 0;

	HRESULT hr = m_d3d11Device->CreateTexture2D(&desc, nullptr, &staging);
	if (FAILED(hr))
		return;

	m_d3d11DevCon->CopyResource(staging, texture->m_texture);
	
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = m_d3d11DevCon->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hr))
		return;

	uint32_t rowPitch = i->GetPitch();
	uint8_t* dptr = i->GetData();
	auto sptr = static_cast<const uint8_t*>(mapped.pData);
	size_t msize = std::min<size_t>(rowPitch, mapped.RowPitch);
	if (sptr)
	{
		for (size_t h = 0; h < desc.Height; ++h)
		{
			memcpy(dptr, sptr, msize);
			sptr += mapped.RowPitch;
			dptr += rowPitch;
		}
	}

	m_d3d11DevCon->Unmap(staging, 0);
	staging->Release();
	return;
}

#endif
