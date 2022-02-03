#include "miGUI.h"
#include "miGUILoader.h"

#include <time.h>
#include <cwchar>

#include <d3d11.h>
#include <d3dcompiler.h>

mgPoint ClientResize(HWND hWnd, int nWidth, int nHeight)
{
    mgPoint size;
    RECT rcClient, rcWind;
    POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWind);
    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
    MoveWindow(hWnd, rcWind.left, rcWind.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
    size.x = nWidth + ptDiff.x;
    size.y = nHeight + ptDiff.y;
    return size;
}

struct Mat4 {
    mgVec4 m_data[4];
};

Mat4 g_proj;
Mat4 UpdateGUIProjection(float x, float y)
{
    Mat4 m;

    m.m_data[0] = mgVec4(2.0f / x, 0.0f, 0.0f, 0.0f);
    m.m_data[1] = mgVec4(0.0f, 2.0f / -y, 0.0f, 0.0f);
    m.m_data[2] = mgVec4(0.0f, 0.0f, 0.5f, 0.0f);
    m.m_data[3] = mgVec4(-1.f, 1.f, 0.5f, 1.0f);

    return m;
}

class MyD3D11Texture {
public:
    MyD3D11Texture() {}
    ~MyD3D11Texture() {
        if (m_texture) m_texture->Release();
        if (m_textureResView) m_textureResView->Release();
        if (m_samplerState) m_samplerState->Release();
    }

    ID3D11Texture2D* m_texture = 0;
    ID3D11ShaderResourceView* m_textureResView = 0;
    ID3D11SamplerState* m_samplerState = 0;
};

class MyD3D11{
    ID3D11Texture2D* m_depthStencilBuffer = 0;
    ID3D11DepthStencilView* m_depthStencilView = 0;
    ID3D11RenderTargetView* m_MainTargetView = 0;
    D3D11_DEPTH_STENCIL_VIEW_DESC	m_depthStencilViewDesc;
    ID3D11DepthStencilState* m_depthStencilStateEnabled = 0;
    ID3D11DepthStencilState* m_depthStencilStateDisabled = 0;
    ID3D11RasterizerState* m_RasterizerSolidNoBackFaceCulling = 0;
    ID3D11BlendState* m_blendStateAlphaEnabled = 0;
    ID3D11BlendState* m_blendStateAlphaDisabled = 0;

    struct cbVertex {
        Mat4 m_ProjMtx;
        mgVec4 m_Corners;
        mgColor m_Color1;
        mgColor m_Color2;
        mgVec4 m_UVs;
    }
    m_cbVertex_impl;
    ID3D11Buffer* m_cbVertex = 0;
    ID3D11VertexShader* m_vShader = 0;
    ID3D11PixelShader* m_pShader = 0;
    ID3D11GeometryShader* m_gShader = 0;
    ID3D11InputLayout* m_vLayout = 0;

    bool D3D11_createConstantBuffer(int byteSize, ID3D11Buffer** cb) {
        D3D11_BUFFER_DESC mbd;
        memset(&mbd, 0, sizeof(mbd));
        mbd.Usage = D3D11_USAGE_DYNAMIC;
        mbd.ByteWidth = byteSize;
        mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(m_d3d11Device->CreateBuffer(&mbd, 0, cb))){
            printf("Can't create constant buffer\n");
            return false;
        }
        return true;
    }

    bool D3D11_createGeometryShaders(const char* target,const char* shaderText,const char* entryPoint,ID3D11GeometryShader** gs) {
        ID3D10Blob* m_GsBlob = nullptr;
        ID3D10Blob* m_errorBlob = nullptr;
        HRESULT hr = D3DCompile(shaderText,strlen(shaderText),0, 0, 0,entryPoint,target,0,0,&m_GsBlob,&m_errorBlob);
        if (FAILED(hr)){
            char* message = (char*)m_errorBlob->GetBufferPointer();
            printf("Geometry shader compile error: %s\n", message);
            return false;
        }

        hr = m_d3d11Device->CreateGeometryShader(m_GsBlob->GetBufferPointer(),m_GsBlob->GetBufferSize(),0,gs);
        if (FAILED(hr)){
            printf("Can't create geometry shader. Error code [%u]\n", hr);
            return false;
        }
        return true;
    }

    bool D3D11_createShaders(const char* vertexTarget,const char* pixelTarget,const char* vertexShader,
        const char* pixelShader,const char* vertexEntryPoint,const char* pixelEntryPoint,
        ID3D11VertexShader** vs,ID3D11PixelShader** ps,ID3D11InputLayout** il)
    {
        ID3D10Blob* m_VsBlob = nullptr;
        ID3D10Blob* m_PsBlob = nullptr;
        ID3D10Blob* m_errorBlob = nullptr;

        HRESULT hr = D3DCompile(vertexShader,strlen(vertexShader),0, 0, 0,vertexEntryPoint,vertexTarget,0,0,&m_VsBlob,&m_errorBlob);

        if (FAILED(hr)){
            char* message = (char*)m_errorBlob->GetBufferPointer();
            printf("Vertex shader compile error: %s\n", message);
            return false;
        }

        hr = D3DCompile(pixelShader,strlen(pixelShader),0, 0, 0,pixelEntryPoint,pixelTarget,0,0,&m_PsBlob,&m_errorBlob);

        if (FAILED(hr)){
            char* message = (char*)m_errorBlob->GetBufferPointer();
            printf("Pixel shader compile error: %s\n", message);
            return false;
        }

        hr = m_d3d11Device->CreateVertexShader(m_VsBlob->GetBufferPointer(),m_VsBlob->GetBufferSize(),0,vs);
        if (FAILED(hr)){
            printf("Can't create vertex shader. Error code [%u]\n", hr);
            return false;
        }

        hr = m_d3d11Device->CreatePixelShader(m_PsBlob->GetBufferPointer(),m_PsBlob->GetBufferSize(),0,ps);
        if (FAILED(hr)){
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

        hr = m_d3d11Device->CreateInputLayout(vertexLayout,vertexLayoutSize,m_VsBlob->GetBufferPointer(),m_VsBlob->GetBufferSize(),il);
        if (FAILED(hr)){
            printf("Can't create input layout. Error code [%u]\n", hr);
            return false;
        }

        if (m_VsBlob)    m_VsBlob->Release();
        if (m_PsBlob)    m_PsBlob->Release();
        if (m_errorBlob) m_errorBlob->Release();

        return true;
    }

    bool _createShader(){
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
            "    output.color.w = input.color.w * textureColor.w;\n"
            "    return output;\n"
            "}\n";
        if (!D3D11_createShaders("vs_4_0","ps_4_0",text,text,"VSMain","PSMain",&this->m_vShader,&this->m_pShader,&this->m_vLayout))
            return false;

        if (!D3D11_createGeometryShaders("gs_4_0", text, "GSMain", &m_gShader))
            return false;

        if (!D3D11_createConstantBuffer(sizeof(cbVertex), &m_cbVertex))
            return false;

        return true;
    }


    bool _createBackBuffer(int x, int y){
        if (m_depthStencilBuffer){
            m_depthStencilBuffer->Release();
            m_depthStencilBuffer = 0;
        }
        if (m_depthStencilView){
            m_depthStencilView->Release();
            m_depthStencilView = 0;
        }
        if (m_MainTargetView){
            m_MainTargetView->Release();
            m_MainTargetView = 0;
        }

        ID3D11Texture2D* BackBuffer = 0;
        if (FAILED(m_SwapChain->GetBuffer(0,IID_ID3D11Texture2D,(void**)&BackBuffer)))
            return false;

        if (FAILED(this->m_d3d11Device->CreateRenderTargetView(BackBuffer, 0, &m_MainTargetView))){
            if (BackBuffer) BackBuffer->Release();
            return false;
        }

        if (BackBuffer) 
            BackBuffer->Release();
        D3D11_TEXTURE2D_DESC	DSD;
        ZeroMemory(&DSD, sizeof(DSD));
        DSD.Width = (UINT)x;
        DSD.Height = (UINT)y;
        DSD.MipLevels = 1;
        DSD.ArraySize = 1;
        DSD.Format = DXGI_FORMAT_D32_FLOAT;
        DSD.SampleDesc.Count = 1;
        DSD.SampleDesc.Quality = 0;
        DSD.Usage = D3D11_USAGE_DEFAULT;
        DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        if (FAILED(m_d3d11Device->CreateTexture2D(&DSD, 0, &m_depthStencilBuffer))){
            printf("Can't create Direct3D 11 depth stencil buffer\n");
            return false;
        }
        ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
        m_depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
        m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        m_depthStencilViewDesc.Texture2D.MipSlice = 0;
        if (FAILED(m_d3d11Device->CreateDepthStencilView(m_depthStencilBuffer, &m_depthStencilViewDesc, &m_depthStencilView))){
            printf("Can't create Direct3D 11 depth stencil view\n");
            return false;
        }
        m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);
        return true;
    }

public:
    MyD3D11(int x, int y){
        m_windowSize.x = x;
        m_windowSize.y = y;
    }
    ~MyD3D11()
    {
        if (m_whiteTexture) delete m_whiteTexture;
        if (m_cbVertex) m_cbVertex->Release();
        if (m_vLayout) m_vLayout->Release();
        if (m_gShader) m_gShader->Release();
        if (m_vShader) m_vShader->Release();
        if (m_pShader) m_pShader->Release();

        if (m_blendStateAlphaDisabled)              m_blendStateAlphaDisabled->Release();
        if (m_blendStateAlphaEnabled)               m_blendStateAlphaEnabled->Release();
        if (m_RasterizerSolidNoBackFaceCulling)     m_RasterizerSolidNoBackFaceCulling->Release();
        if (m_depthStencilView)                     m_depthStencilView->Release();
        if (m_depthStencilStateDisabled)            m_depthStencilStateDisabled->Release();
        if (m_depthStencilStateEnabled)             m_depthStencilStateEnabled->Release();
        if (m_depthStencilBuffer)                   m_depthStencilBuffer->Release();
        if (m_MainTargetView)                       m_MainTargetView->Release();
        if (m_d3d11DevCon)                          m_d3d11DevCon->Release();
        if (m_SwapChain)                            m_SwapChain->Release();

        if (m_d3d11Device)                          m_d3d11Device->Release();
    }

    bool init(HWND hwnd)
    {
        DXGI_MODE_DESC	bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Width = m_windowSize.x;
        bufferDesc.Height = m_windowSize.y;
        bufferDesc.RefreshRate.Numerator = 60;
        bufferDesc.RefreshRate.Denominator = 1;
        bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        auto hr = D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,D3D11_CREATE_DEVICE_SINGLETHREADED,
            nullptr,0,D3D11_SDK_VERSION,&m_d3d11Device,&featureLevel,&m_d3d11DevCon);

        if (FAILED(hr)){
            printf("Can't create Direct3D 11 Device : code %u\n", hr);
            return false;
        }

        IDXGIDevice* dxgiDevice = nullptr;
        IDXGIAdapter* dxgiAdapter = nullptr;
        IDXGIFactory1* dxgiFactory = nullptr;
        hr = m_d3d11Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
        if (FAILED(hr)){
            printf("Can't QueryInterface : IID_IDXGIDevice, code %u\n", hr);
            return false;
        }

        hr = dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
        if (FAILED(hr)){
            printf("Can't get DXGI adapter, code %u\n", hr);
            return false;
        }

        hr = dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
        if (FAILED(hr)){
            printf("Can't get DXGI factory, code %u\n", hr);
            return false;
        }

        DXGI_SWAP_CHAIN_DESC	swapChainDesc;
        memset(&swapChainDesc, 0, sizeof(swapChainDesc));
        swapChainDesc.BufferDesc = bufferDesc;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hwnd;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.Windowed = true/*m_params.m_fullScreen ? false : true*/;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        hr = dxgiFactory->CreateSwapChain(m_d3d11Device, &swapChainDesc, &m_SwapChain);
        if (FAILED(hr)){
            printf("Can't create Swap Chain : code %u\n", hr);
            return false;
        }

        dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        dxgiDevice->Release();
        dxgiAdapter->Release();
        dxgiFactory->Release();
        
        _createBackBuffer(m_windowSize.x, m_windowSize.y);

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

        if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStateEnabled))){
            printf("Can't create Direct3D 11 depth stencil state\n");
            return false;
        }

        m_d3d11DevCon->OMSetDepthStencilState(this->m_depthStencilStateEnabled, 0);

        depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.StencilEnable = false;
        depthStencilDesc.DepthEnable = false;
        if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled))){
            printf("Can't create Direct3D 11 depth stencil state\n");
            return false;
        }

        D3D11_RASTERIZER_DESC	rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.ScissorEnable = true;

        if (FAILED(m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling))){
            printf("Can not create rasterizer state\n");
            return false;
        }

        m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

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

        if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled))){
            printf("Can't create Direct3D 11 blend state\n");
            return false;
        }
        bd.RenderTarget[0].BlendEnable = FALSE;
        if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaDisabled)))
        {
            printf("Can't create Direct3D 11 blend state\n");
            return false;
        }

        float blendFactor[4];
        blendFactor[0] = 0.0f;
        blendFactor[1] = 0.0f;
        blendFactor[2] = 0.0f;
        blendFactor[3] = 0.0f;
        m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);

        D3D11_RECT sr;
        sr.left = 0;
        sr.top = 0;
        sr.right = m_windowSize.x;
        sr.bottom = m_windowSize.y;
        m_d3d11DevCon->RSSetScissorRects(1, &sr);

        D3D11_VIEWPORT viewport;
        viewport.Width = m_windowSize.x;
        viewport.Height = m_windowSize.y;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        m_d3d11DevCon->RSSetViewports(1, &viewport);

        this->_createShader();
        this->UpdateBackBuffer(m_windowSize.x, m_windowSize.y);

        m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0);
        
        m_clearColor[0] = 1.f;
        m_clearColor[1] = 1.f;
        m_clearColor[2] = 1.f;
        m_clearColor[3] = 1.f;

        m_whiteTexture = this->CreateWhiteTexture();
        
        return true;
    }
    
    void SwapBuffers()
    {
        m_SwapChain->Present(1, 0);
    }

    bool m_old_depth = true;
    bool UseDepth(bool v){
        v ? m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0)
            : m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);

        auto o = m_old_depth;
        m_old_depth = v;
        return o;
    }

    bool m_old_blend = true;
    bool UseBlend(bool v){
        const float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
        if (v)
            m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
        else
            m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaDisabled, blend_factor, 0xffffffff);

        auto o = m_old_blend;
        m_old_blend = v;
        return o;
    }

    void SetViewport(float x, float y, float width, float height)
    {
        D3D11_VIEWPORT viewport;
        viewport.Width = width;
        viewport.Height = height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = x;
        viewport.TopLeftY = y;
        m_d3d11DevCon->RSSetViewports(1, &viewport);
    }

    void ClearDepth(){
        m_d3d11DevCon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    void ClearColor(){
        m_d3d11DevCon->ClearRenderTargetView(m_MainTargetView, m_clearColor);
    }
    void ClearAll(){
        m_d3d11DevCon->ClearRenderTargetView(m_MainTargetView, m_clearColor);
        m_d3d11DevCon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    MyD3D11Texture* CreateWhiteTexture()
    {
        MyD3D11Texture* t = 0;
        mgImage img;
        img.height = 2;
        img.width = 2;
        img.dataSize = (img.width * 4) * img.height;
        img.data = (unsigned char*)malloc(img.dataSize);
        memset(img.data, 255, img.dataSize);
        t = CreateTexture(&img);
        free(img.data);
        return t;
    }

    void DestroyTexture(MyD3D11Texture* t)
    {
        delete t;
    }

    MyD3D11Texture* CreateTexture(mgImage* img)
    {
        MyD3D11Texture* t = new MyD3D11Texture;

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
        auto hr = m_d3d11Device->CreateTexture2D(&desc, &initData, &t->m_texture);
        if (FAILED(hr))
        {
            printf("Can't create 2D texture\n");
            delete t;
            return 0;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        ZeroMemory(&SRVDesc, sizeof(SRVDesc));
        SRVDesc.Format = desc.Format;
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MostDetailedMip = 0;
        SRVDesc.Texture2D.MipLevels = 1;

        hr = m_d3d11Device->CreateShaderResourceView(t->m_texture,
            &SRVDesc, &t->m_textureResView);
        if (FAILED(hr))
        {
            printf("Can't create shader resource view\n");
            delete t;
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
        
            if (FAILED(m_d3d11Device->CreateSamplerState(&samplerDesc, &t->m_samplerState)))
            {
                printf("Can't create sampler state\n");
                delete t;
                return 0;
            }
        }
        return t;
    }

    void DrawRectangle(
        const mgVec4& corners,
        const mgColor& color1,
        const mgColor& color2,
        Mat4* proj,
        MyD3D11Texture* texture,
        mgVec4* UVs)
    {
        m_cbVertex_impl.m_ProjMtx = *proj;
        m_cbVertex_impl.m_Corners = corners;
        m_cbVertex_impl.m_Color1 = color1;
        m_cbVertex_impl.m_Color2 = color2;
        m_cbVertex_impl.m_UVs.x = 0.f; 
        m_cbVertex_impl.m_UVs.y = 0.f;
        m_cbVertex_impl.m_UVs.z = 1.f;
        m_cbVertex_impl.m_UVs.w = 1.f;

        if (UVs)
            m_cbVertex_impl.m_UVs = *UVs;

        m_d3d11DevCon->IASetInputLayout(m_vLayout);
        m_d3d11DevCon->VSSetShader(m_vShader, 0, 0);
        m_d3d11DevCon->GSSetShader(m_gShader, 0, 0);
        m_d3d11DevCon->PSSetShader(m_pShader, 0, 0);

        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            D3D11_BUFFER_DESC d;
            m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            m_cbVertex->GetDesc(&d);
            memcpy(mappedResource.pData, &m_cbVertex_impl, d.ByteWidth);
            m_d3d11DevCon->Unmap(m_cbVertex, 0);
            m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
            m_d3d11DevCon->GSSetConstantBuffers(0, 1, &m_cbVertex);
        }

        m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        if (!texture)
            texture = m_whiteTexture;

        m_d3d11DevCon->PSSetShaderResources(0, 1, &((MyD3D11Texture*)texture)->m_textureResView);
        m_d3d11DevCon->PSSetSamplers(0, 1, &((MyD3D11Texture*)texture)->m_samplerState);

        auto old_depth = UseDepth(false);
        m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

        m_d3d11DevCon->Draw(1, 0);

        UseDepth(old_depth);
    }

    void UpdateBackBuffer(int x, int y) {
        m_windowSize.x = x;
        m_windowSize.y = y;
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

        m_SwapChain->ResizeBuffers(0, m_windowSize.x, m_windowSize.y, DXGI_FORMAT_UNKNOWN, 0);
        _createBackBuffer(m_windowSize.x, m_windowSize.y);

        SetViewport(0.f, 0.f, (float)(m_windowSize.x), (float)(m_windowSize.y));
        
        SetClipRect(0, 0, m_windowSize.x, m_windowSize.y);
    }

    void SetClipRect(LONG l, LONG t, LONG r, LONG b)
    {
        D3D11_RECT _r;
        _r.left = l;
        _r.top = t;
        _r.right = r;
        _r.bottom = b;
        m_d3d11DevCon->RSSetScissorRects(1, &_r);
    }

    mgPoint m_windowSize;

    MyD3D11Texture* m_whiteTexture = 0;

    float m_clearColor[4];
    IDXGISwapChain* m_SwapChain = 0;
    ID3D11Device* m_d3d11Device = 0;
    ID3D11DeviceContext* m_d3d11DevCon = 0;
};
MyD3D11* g_d3d11 = 0;

mgContext* g_gui_context = 0;
mgInputContext g_input;
HRAWINPUT g_rawInputData[0xff];

mgFont* g_font = 0;

HKL KEYBOARD_INPUT_HKL = 0;
unsigned int KEYBOARD_INPUT_CODEPAGE = 0;

void gui_beginDraw()
{
}

void gui_endDraw()
{
}

mgTexture gui_createTexture(mgImage* img)
{
    return (mgTexture)g_d3d11->CreateTexture(img);
}

void gui_destroyTexture(mgTexture t)
{
    g_d3d11->DestroyTexture((MyD3D11Texture*)t);
}

mgRect gui_setClipRect(mgRect* r)
{
    static mgRect old;
    g_d3d11->SetClipRect(r->left, r->top, r->right, r->bottom);
    mgRect ret = old;
    old = *r;
    return ret;
}

void gui_drawRectangle(
    int reason,
    mgRect* rct,
    mgColor* color1,
    mgColor* color2,
    mgElement* element, /*current element, can be null*/
    mgTexture texture, /*optional*/
    mgVec4* UVRegion)
{
    mgVec4 corners;
    corners.x = rct->left;
    corners.y = rct->top;
    corners.z = rct->right;
    corners.w = rct->bottom;
    g_d3d11->DrawRectangle(corners, *color1, *color2, &g_proj, 0, 0);
}

void gui_drawText(
    int reason,
    mgPoint* position,
    const wchar_t* text,
    int textLen,
    mgColor* color,
    mgFont* font)
{
    mgPoint _position = *position;
    
    for (int i = 0; i < textLen; ++i)
    {
        wchar_t character = text[i];
        auto glyph = font->glyphMap[character];
        if (glyph)
        {
            _position.x += glyph->underhang;

            mgVec4 corners;
            corners.x = _position.x;
            corners.y = _position.y;

            corners.z = corners.x + glyph->width;
            corners.w = corners.y + glyph->height;
            
            MyD3D11Texture* texture = (MyD3D11Texture*)((mgFontBitmap*)font->implementation)[glyph->textureSlot].gpuTexture;
            g_d3d11->DrawRectangle(corners, *color, *color, &g_proj, texture, &glyph->UV);

            _position.x += glyph->width + glyph->overhang + font->characterSpacing;
            if (character == L' ')
                _position.x += font->spaceSize;
            if (character == L'\t')
                _position.x += font->tabSize;
        }
    }
}

void gui_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
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

mgFont* gui_createFont(const char* fn, unsigned int flags, int size)
{
    return 0;
}

void draw_gui()
{
    if (!g_gui_context)
        return;

    g_gui_context->gpu->beginDraw();
    mgRect rect;
    mgRectSet(&rect, 0, 0, 220, 180);
    gui_setClipRect(&rect);
    
    mgColor color1;
    mgColor color2;
    mgColorSetAsIntegerRGB(&color1, 0xFFFF9999);
    mgColorSetAsIntegerRGB(&color2, 0xFF9999FF);

    g_gui_context->gpu->drawRectangle(0, &rect, &color1, &color2, 0, 0, 0);

    mgPoint textPosition;
    mgPointSet(&textPosition, 10, 10);
    mgColor textColor;
    mgColorSetAsIntegerRGB(&textColor, 0xFF005000);
    wchar_t textBuffer[200];
    
    swprintf(textBuffer, 200, L"mousePosition: %i %i", g_input.mousePosition.x, g_input.mousePosition.y);
    mgPointSet(&textPosition, 10, 10);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    
    swprintf_s(textBuffer, L"mouseMoveDelta: %i %i", g_input.mouseMoveDelta.x, g_input.mouseMoveDelta.y);
    mgPointSet(&textPosition, 10, 20);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    
    swprintf_s(textBuffer, L"mouseWheelDelta: %.1f", g_input.mouseWheelDelta);
    mgPointSet(&textPosition, 10, 30);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    
    swprintf_s(textBuffer, L"mouseButtons: 0x%x 0x%x", g_input.mouseButtonFlags1, g_input.mouseButtonFlags2);
    mgPointSet(&textPosition, 10, 40);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);

    swprintf_s(textBuffer, L"key char: %c 0x%x", g_input.character, g_input.character);
    mgPointSet(&textPosition, 10, 50);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);

    if (mgIsKeyHit(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Hit ENTER");
        mgPointSet(&textPosition, 10, 60);
        gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    }

    if (mgIsKeyHold(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Hold ENTER");
        mgPointSet(&textPosition, 10, 70);
        gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    }

    if (mgIsKeyRelease(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Release ENTER");
        mgPointSet(&textPosition, 10, 80);
        gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);
    }

    swprintf_s(textBuffer, L"KB modifier 0x%x\n", g_input.keyboardModifier);
    mgPointSet(&textPosition, 10, 90);
    gui_drawText(0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_font);

    /*draw elements*/
    mgDraw(g_gui_context);

    g_gui_context->gpu->endDraw();
}


void rect_onMouseEnter(struct mgElement_s* e){
    mgElementText* text = (mgElementText*)e->userData;
    text->text = L"Mouse enter";
    text->textLen = wcslen(text->text);
    mgSetCursor(g_gui_context, g_gui_context->defaultCursors[mgCursorType_Hand], mgCursorType_Arrow);
}
void rect_onMouseLeave(struct mgElement_s* e){
    mgElementText* text = (mgElementText*)e->userData;
    text->text = L"Mouse leave";
    text->textLen = wcslen(text->text);
    mgSetCursor(g_gui_context, g_gui_context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
}
void rect_onClickLMB(struct mgElement_s* e) {
    mgElementText* text = (mgElementText*)e->userData;
    text->color.setAsIntegerBGR(0xFF0000);
}
void rect_onReleaseLMB(struct mgElement_s* e) {
    mgElementText* text = (mgElementText*)e->userData;
    text->color.setAsIntegerBGR(0xFFFFFF);
}

static unsigned int LocaleIdToCodepage(unsigned int lcid);

int g_run = 1;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int main()
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(0);
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = L"migui";
    wcex.hIconSm = 0;
    RegisterClassExW(&wcex);

    HWND hwnd = CreateWindowExW(0, wcex.lpszClassName, L"migui", WS_OVERLAPPEDWINDOW,
        0, 0, 800, 600, 0, 0, wcex.hInstance, 0);
    if (!hwnd)
        return FALSE;

    MoveWindow(hwnd, 0, 0, 800, 600, FALSE);

    ShowWindow(hwnd, SW_NORMAL);
    UpdateWindow(hwnd);

    RAWINPUTDEVICE device;
    device.usUsagePage = 0x01;
    device.usUsage = 0x02;
    device.dwFlags = 0;
    device.hwndTarget = 0;
    RegisterRawInputDevices(&device, 1, sizeof device);

    MyD3D11 d3d11(800, 600);
    if (!d3d11.init(hwnd))
        return 0;

   


    MG_LIB_HANDLE gui_lib = mgLoad();
    if (!gui_lib)
    {
        MessageBoxA(0, "Can't load migui.dll", "Error", MB_OK);
        return 0;
    }

    mgVideoDriverAPI gui_gpu;
    gui_gpu.createTexture = gui_createTexture;
    gui_gpu.destroyTexture = gui_destroyTexture;
    gui_gpu.beginDraw = gui_beginDraw;
    gui_gpu.endDraw = gui_endDraw;
    gui_gpu.drawRectangle = gui_drawRectangle;
    gui_gpu.drawText = gui_drawText;
    gui_gpu.setClipRect = gui_setClipRect;

    memset(&g_input, 0, sizeof(g_input));
    g_gui_context = mgCreateContext(&gui_gpu, &g_input);
    g_gui_context->getTextSize = gui_getTextSize;
    
    g_d3d11 = &d3d11;
    ClientResize(hwnd, 800, 600); // instead this vvv
    // g_gui_context->windowSize.x = rc.right - rc.left;
    // g_gui_context->windowSize.y = rc.bottom - rc.top;

    // destroy g_font at the end using mgDestroyFont
    g_font = mgCreateFont(g_gui_context, "Noto Sans", 0, 10, "Noto Sans"); //gui_createFont("Segoe", MG_FNTFL_BOLD, 14);
    {
        /*mgPoint pos, sz;
        mgColor c1, c2;
        pos.x = 300;
        pos.y = 0;
        sz.x = 100;
        sz.y = 20;
        c1.setAsIntegerRGB(0xFF0000);
        c2.setAsIntegerRGB(0x0000FF);
        mgElement* rectangle = mgCreateRectangle(g_gui_context, &pos, &sz, &c1, &c2);
        mgElement* text = mgCreateText(g_gui_context, &pos, L"Text", g_font);
        ((mgElementText*)text->implementation)->color.setAsIntegerBGR(0xFFFFFF);
        rectangle->userData = text->implementation;
        rectangle->onMouseEnter = rect_onMouseEnter;
        rectangle->onMouseLeave = rect_onMouseLeave;
        rectangle->onClickLMB = rect_onClickLMB;
        rectangle->onReleaseLMB = rect_onReleaseLMB;
     
        pos.y += 30;
        mgElement* button = mgCreateButton(g_gui_context, &pos, &sz, L"Button", g_font);*/
        mgWindow* guiWindow = mgCreateWindow(g_gui_context, 10, 10, 300, 180);
        guiWindow->titlebarFont = g_font;
        mgSetWindowTitle(guiWindow, L"Window");
    }

    MSG msg;
    
    g_proj = UpdateGUIProjection(800.f, 600.f);


    Sleep(100);
    while (g_run)
    {
        mgStartFrame(g_gui_context);

        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        mgUpdate(g_gui_context);

        d3d11.ClearAll();

        draw_gui();

      //  d3d11.DrawRectangle(mgVec4(0.f, 0.f, 100.f, 100.f), mgColor(1.f, 0.f, 0.f, 1.f), mgColor(0.f, 0.f, 1.f, 1.f), &g_proj, 0, 0);

        d3d11.SwapBuffers();
    }
    
    if (g_font)
        mgDestroyFont(g_gui_context, g_font);

    mgDestroyContext(g_gui_context);
    mgUnload(gui_lib);

    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_SIZE:
    {
        if (g_d3d11)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            
            ClientResize(hWnd, rc.right - rc.left, rc.bottom - rc.top);
            g_d3d11->UpdateBackBuffer(rc.right - rc.left, rc.bottom - rc.top);
            g_proj = UpdateGUIProjection((float)(rc.right - rc.left), (float)(rc.bottom - rc.top));

            if (g_gui_context)
            {
                g_gui_context->needRebuild = 1;
                g_gui_context->windowSize.x = rc.right - rc.left;
                g_gui_context->windowSize.y = rc.bottom - rc.top;
            }
        }
    }return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_INPUT:
    {
        HRAWINPUT hRawInput = (HRAWINPUT)lParam;
        UINT dataSize;
        GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

        if (dataSize == 0 || dataSize > 0xff)
            break;

        void* dataBuf = &g_rawInputData[0];
        GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

        const RAWINPUT* raw = (const RAWINPUT*)dataBuf;
        if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            HANDLE deviceHandle = raw->header.hDevice;

            const RAWMOUSE& mouseData = raw->data.mouse;

            USHORT flags = mouseData.usButtonFlags;
            short wheelDelta = (short)mouseData.usButtonData;
            LONG x = mouseData.lLastX, y = mouseData.lLastY;

            /*wprintf(
                L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
                deviceHandle, flags, wheelDelta, x, y);*/

            if (wheelDelta)
                g_input.mouseWheelDelta = (float)wheelDelta / (float)WHEEL_DELTA;

            POINT cursorPoint;
            GetCursorPos(&cursorPoint);
            ScreenToClient(hWnd, &cursorPoint);
            g_input.mousePosition.x = cursorPoint.x;
            g_input.mousePosition.y = cursorPoint.y;

            if (flags)
            {
                if ((flags & 0x1) == 0x1)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_LMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x2) == 0x2)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_LMBUP;
                    if((g_input.mouseButtonFlags2 & MG_MBFL_LMBHOLD) == MG_MBFL_LMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x4) == 0x4)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_RMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_RMBHOLD;
                }
                if ((flags & 0x8) == 0x8)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_RMBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_RMBHOLD) == MG_MBFL_RMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_RMBHOLD;
                }

                if ((flags & 0x10) == 0x10)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_MMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_MMBHOLD;
                }
                if ((flags & 0x20) == 0x20)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_MMBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_MMBHOLD) == MG_MBFL_MMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_MMBHOLD;
                }

                if ((flags & 0x100) == 0x100)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X1MBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_X1MBHOLD;
                }
                if ((flags & 0x200) == 0x200)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X1MBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_X1MBHOLD) == MG_MBFL_X1MBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_X1MBHOLD;
                }

                if ((flags & 0x40) == 0x40)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X2MBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_X2MBHOLD;
                }
                if ((flags & 0x80) == 0x80)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X2MBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_X2MBHOLD) == MG_MBFL_X2MBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_X2MBHOLD;
                }

            }
        }
    }break;
    case WM_DESTROY:
        g_run = 0;
        PostQuitMessage(0);
        break;
    case WM_SETCURSOR: {
        auto id = LOWORD(lParam);
        switch (id)
        {
        default:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_Arrow], mgCursorType_Arrow);
            return TRUE;
        case HTLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTTOP:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTBOTTOM:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTTOPLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTTOPRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTHELP:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_Help], mgCursorType_Help);
            return TRUE;
        }
    }break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        bool isPress = true;

        unsigned char key = (unsigned char)wParam;

        if (message == WM_SYSKEYUP) isPress = false;
        if (message == WM_KEYUP) isPress = false;

        const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

        if (key == MG_KEY_SHIFT)
        { // shift -> lshift rshift
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255u), MY_MAPVK_VSC_TO_VK_EX));
        }
        if (key == MG_KEY_CTRL)
        { // ctrl -> lctrl rctrl
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
            if (lParam & 0x1000000)
                key = static_cast<unsigned char>(163);
        }

        if (key == MG_KEY_ALT)
        { // alt -> lalt ralt
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
            if (lParam & 0x1000000)
                key = static_cast<unsigned char>(165);
            //printf("alt = %i\n",(int)ev.keyboardEvent.key);
        }

        static unsigned char keys[256u];
        GetKeyboardState(keys);
        WORD chars[2];

        if (ToAsciiEx((UINT)wParam, HIWORD(lParam), keys, chars, 0, KEYBOARD_INPUT_HKL) == 1)
        {
            MultiByteToWideChar(KEYBOARD_INPUT_CODEPAGE, MB_PRECOMPOSED, (LPCSTR)chars,
                sizeof(chars), (WCHAR*)&g_input.character, 1);
        }

        if (isPress)
        {
            if (key < 256)
            {
                g_input.keyFlags[key] |= MG_KEYFL_HOLD;
                g_input.keyFlags[key] |= MG_KEYFL_HIT;
            }
        }
        else
        {
            if (key < 256)
            {
                if((g_input.keyFlags[key] & MG_KEYFL_HOLD) == MG_KEYFL_HOLD)
                    g_input.keyFlags[key] ^= MG_KEYFL_HOLD;
                
                g_input.keyFlags[key] |= MG_KEYFL_RELEASE;
            }
        }

        if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        else
        {
            return 0;
        }
    }break;

    case WM_INPUTLANGCHANGE:
        KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
        KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
            (wParam & 0xFFF0) == SC_MONITORPOWER ||
            (wParam & 0xFFF0) == SC_KEYMENU
            )
        {
            return 0;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// IrrLicht
// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid)
{
    switch (lcid)
    {
    case 1098:  // Telugu
    case 1095:  // Gujarati
    case 1094:  // Punjabi
    case 1103:  // Sanskrit
    case 1111:  // Konkani
    case 1114:  // Syriac
    case 1099:  // Kannada
    case 1102:  // Marathi
    case 1125:  // Divehi
    case 1067:  // Armenian
    case 1081:  // Hindi
    case 1079:  // Georgian
    case 1097:  // Tamil
        return 0;
    case 1054:  // Thai
        return 874;
    case 1041:  // Japanese
        return 932;
    case 2052:  // Chinese (PRC)
    case 4100:  // Chinese (Singapore)
        return 936;
    case 1042:  // Korean
        return 949;
    case 5124:  // Chinese (Macau S.A.R.)
    case 3076:  // Chinese (Hong Kong S.A.R.)
    case 1028:  // Chinese (Taiwan)
        return 950;
    case 1048:  // Romanian
    case 1060:  // Slovenian
    case 1038:  // Hungarian
    case 1051:  // Slovak
    case 1045:  // Polish
    case 1052:  // Albanian
    case 2074:  // Serbian (Latin)
    case 1050:  // Croatian
    case 1029:  // Czech
        return 1250;
    case 1104:  // Mongolian (Cyrillic)
    case 1071:  // FYRO Macedonian
    case 2115:  // Uzbek (Cyrillic)
    case 1058:  // Ukrainian
    case 2092:  // Azeri (Cyrillic)
    case 1092:  // Tatar
    case 1087:  // Kazakh
    case 1059:  // Belarusian
    case 1088:  // Kyrgyz (Cyrillic)
    case 1026:  // Bulgarian
    case 3098:  // Serbian (Cyrillic)
    case 1049:  // Russian
        return 1251;
    case 8201:  // English (Jamaica)
    case 3084:  // French (Canada)
    case 1036:  // French (France)
    case 5132:  // French (Luxembourg)
    case 5129:  // English (New Zealand)
    case 6153:  // English (Ireland)
    case 1043:  // Dutch (Netherlands)
    case 9225:  // English (Caribbean)
    case 4108:  // French (Switzerland)
    case 4105:  // English (Canada)
    case 1110:  // Galician
    case 10249:  // English (Belize)
    case 3079:  // German (Austria)
    case 6156:  // French (Monaco)
    case 12297:  // English (Zimbabwe)
    case 1069:  // Basque
    case 2067:  // Dutch (Belgium)
    case 2060:  // French (Belgium)
    case 1035:  // Finnish
    case 1080:  // Faroese
    case 1031:  // German (Germany)
    case 3081:  // English (Australia)
    case 1033:  // English (United States)
    case 2057:  // English (United Kingdom)
    case 1027:  // Catalan
    case 11273:  // English (Trinidad)
    case 7177:  // English (South Africa)
    case 1030:  // Danish
    case 13321:  // English (Philippines)
    case 15370:  // Spanish (Paraguay)
    case 9226:  // Spanish (Colombia)
    case 5130:  // Spanish (Costa Rica)
    case 7178:  // Spanish (Dominican Republic)
    case 12298:  // Spanish (Ecuador)
    case 17418:  // Spanish (El Salvador)
    case 4106:  // Spanish (Guatemala)
    case 18442:  // Spanish (Honduras)
    case 3082:  // Spanish (International Sort)
    case 13322:  // Spanish (Chile)
    case 19466:  // Spanish (Nicaragua)
    case 2058:  // Spanish (Mexico)
    case 10250:  // Spanish (Peru)
    case 20490:  // Spanish (Puerto Rico)
    case 1034:  // Spanish (Traditional Sort)
    case 14346:  // Spanish (Uruguay)
    case 8202:  // Spanish (Venezuela)
    case 1089:  // Swahili
    case 1053:  // Swedish
    case 2077:  // Swedish (Finland)
    case 5127:  // German (Liechtenstein)
    case 1078:  // Afrikaans
    case 6154:  // Spanish (Panama)
    case 4103:  // German (Luxembourg)
    case 16394:  // Spanish (Bolivia)
    case 2055:  // German (Switzerland)
    case 1039:  // Icelandic
    case 1057:  // Indonesian
    case 1040:  // Italian (Italy)
    case 2064:  // Italian (Switzerland)
    case 2068:  // Norwegian (Nynorsk)
    case 11274:  // Spanish (Argentina)
    case 1046:  // Portuguese (Brazil)
    case 1044:  // Norwegian (Bokmal)
    case 1086:  // Malay (Malaysia)
    case 2110:  // Malay (Brunei Darussalam)
    case 2070:  // Portuguese (Portugal)
        return 1252;
    case 1032:  // Greek
        return 1253;
    case 1091:  // Uzbek (Latin)
    case 1068:  // Azeri (Latin)
    case 1055:  // Turkish
        return 1254;
    case 1037:  // Hebrew
        return 1255;
    case 5121:  // Arabic (Algeria)
    case 15361:  // Arabic (Bahrain)
    case 9217:  // Arabic (Yemen)
    case 3073:  // Arabic (Egypt)
    case 2049:  // Arabic (Iraq)
    case 11265:  // Arabic (Jordan)
    case 13313:  // Arabic (Kuwait)
    case 12289:  // Arabic (Lebanon)
    case 4097:  // Arabic (Libya)
    case 6145:  // Arabic (Morocco)
    case 8193:  // Arabic (Oman)
    case 16385:  // Arabic (Qatar)
    case 1025:  // Arabic (Saudi Arabia)
    case 10241:  // Arabic (Syria)
    case 14337:  // Arabic (U.A.E.)
    case 1065:  // Farsi
    case 1056:  // Urdu
    case 7169:  // Arabic (Tunisia)
        return 1256;
    case 1061:  // Estonian
    case 1062:  // Latvian
    case 1063:  // Lithuanian
        return 1257;
    case 1066:  // Vietnamese
        return 1258;
    }
    return 65001;   // utf-8
}