#pragma once
#ifndef __MGF_GSD3D11_H_
#define __MGF_GSD3D11_H_

#ifdef MGF_GS_D3D11

#include "framework/GS.h"
#include <d3d11.h>

typedef HRESULT(WINAPI*PFND3DCOMPILEPROC)(
	LPCVOID pSrcData,
	SIZE_T SrcDataSize,
	LPCSTR pSourceName,
	CONST D3D_SHADER_MACRO* pDefines,
	ID3DInclude* pInclude,
	LPCSTR pEntrypoint,
	LPCSTR pTarget,
	UINT Flags1,
	UINT Flags2,
	ID3DBlob** ppCode,
	ID3DBlob** ppErrorMsgs);
 
//HRESULT D3D11CreateDevice
typedef HRESULT(WINAPI*PFND3D11CREATEDEVICEPROC)(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE         DriverType,
	HMODULE                 Software,
	UINT                    Flags,
	const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT                    FeatureLevels,
	UINT                    SDKVersion,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext);

namespace mgf
{
	class GSD3D11Texture;
	class GSD3D11Mesh;
	class GSD3D11Shader;
	class GSD3D11ShaderScreenQuad;
	class GSD3D11ShaderLine;

	struct BackendD3D11Params;

	constexpr uint32_t GSVideoDriverMaxTextures = 16;

	class GSD3D11 : public GS
	{
		v2f m_mainTargetSize;

		IDXGISwapChain* m_SwapChain = 0;
		ID3D11Device* m_d3d11Device = 0;
		ID3D11DeviceContext* m_d3d11DevCon = 0;
		ID3D11RenderTargetView* m_MainTargetView = 0;
		ID3D11RenderTargetView* m_currentTargetView = 0;
		ID3D11Texture2D* m_depthStencilBuffer = 0;
		ID3D11DepthStencilState* m_depthStencilStateEnabled = 0;
		ID3D11DepthStencilState* m_depthStencilStateDisabled = 0;
		ID3D11DepthStencilView* m_depthStencilView = 0;
		ID3D11RasterizerState* m_RasterizerSolid = 0;
		ID3D11RasterizerState* m_RasterizerSolidNoBackFaceCulling = 0;
		ID3D11RasterizerState* m_RasterizerWireframeNoBackFaceCulling = 0;
		ID3D11RasterizerState* m_RasterizerWireframe = 0;
		ID3D11BlendState* m_blendStateAlphaEnabled = 0;
		ID3D11BlendState* m_blendStateAlphaEnabledWithATC = 0;
		ID3D11BlendState* m_blendStateAlphaDisabled = 0;

		GSD3D11Texture* m_mainTarget = 0;
		GSD3D11Mesh* m_mainTargetSurface = 0;
		bool _createBackBuffer(uint32_t x, uint32_t y);
		bool _updateMainTarget();

		HMODULE m_dllD3DCompiler = 0;
		HMODULE m_dllD3D11 = 0;

		bool m_vsync = true;
		bool m_old_depth = true;
		bool m_old_blend = true;
		mgColor m_clearColor;

		GSD3D11Texture* m_whiteTexture = 0;
		GSD3D11Shader* m_activeShader = 0;
		GSD3D11ShaderScreenQuad* m_shaderScreenQuad = 0;
		GSD3D11ShaderLine* m_shaderLine = 0;

		v4f m_old_viewport;
		v4f m_old_scissor;

		GSD3D11Texture* m_currentTextures[GSVideoDriverMaxTextures];
		GSD3D11Mesh* m_currentMesh = nullptr;
	public:
		GSD3D11();
		virtual ~GSD3D11();

		void SetActiveShader(GSD3D11Shader*);

		bool Init(SystemWindow* window, BackendD3D11Params* outParams);

		virtual void UseVSync(bool) override;
		virtual bool UseDepth(bool) override; // return old
		virtual bool UseBlend(bool) override; // return old
		virtual void GetDepthRange(v2f*) override;
		virtual void SetClearColor(float* rgba) override;
		virtual void BeginDraw() override;
		virtual void ClearDepth() override;
		virtual void ClearColor() override;
		virtual void ClearAll() override;    // ClearDepth и ClearColor
		virtual void EndDraw() override;
		virtual void SwapBuffers() override;       // Present\swapbuffers
		virtual void UpdateMainRenderTarget(const v2f& bufferSize) override;
		virtual GSTexture* CreateTexture(GSTextureInfo*) override;
		virtual GSTexture* CreateRenderTargetTexture(uint32_t w, uint32_t h, GSTextureInfo* optional) override;
		virtual GSMesh* CreateMesh(GSMeshInfo*) override;
		virtual void SetRenderTarget(GSTexture*) override;
		virtual void SetViewport(float x, float y, float width, float height, SystemWindow* window, v4f* old) override;
		virtual void SetScissorRect(const v4f& rect, SystemWindow* window, v4f* old) override;
		virtual const wchar_t* GetVideoDriverName() override;

		virtual void SetTexture(uint32_t slot, GSTexture*) override;
		virtual void SetMesh(GSMesh*) override;
		virtual void Draw() override;

		virtual void DrawLine2D(const v3f& p1, const v3f& p2, const mgColor& c) override;
		virtual void DrawLine3D(const v4f& p1, const v4f& p2, const mgColor& c) override;

		virtual void GetTextureCopyForImage(GSTexture* t, Image* i) override;

		void UpdateGUIProjection();
	};

}
#endif

#endif
