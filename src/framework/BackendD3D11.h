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

#ifndef _MG_BCK_D3D11_H_
#define _MG_BCK_D3D11_H_

#ifdef MGF_BACKEND_D3D11

#include "framework/Mat.h"

#ifdef MG_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#endif

namespace mgf
{
	class SystemWindow;
	class Context;
	
	struct BackendD3D11Params
	{
		ID3D11Device* m_d3d11Device = 0;
		ID3D11DeviceContext* m_d3d11DevCon = 0;
	};

	struct BackendD3D11_texture
	{
		BackendD3D11_texture() {}
		~BackendD3D11_texture() 
		{
			if (m_texture) m_texture->Release();
			if (m_textureResView) m_textureResView->Release();
			if (m_samplerState) m_samplerState->Release();
		}
		ID3D11Texture2D* m_texture = 0;
		ID3D11ShaderResourceView* m_textureResView = 0;
		ID3D11SamplerState* m_samplerState = 0;
	};

	// You need to init and call clearcolor and swapbuffers by yourself
	class BackendD3D11 : public Backend
	{
		friend class Context;

		mgf::SystemWindow* m_currWindow = 0;

		mgf::Context* m_context = 0;

		void* m_gpu = 0;

		mgTexture_s* m_defaultIcons = 0;
		Font* m_defaultFont = 0;

		mgRect m_clipRect;

		void _createBackbuffer(mgf::SystemWindow* impl);

		//Image* blackImage = 0; //for shadows
		BackendD3D11Params m_params;

		mgVec4 m_drrect_tcoords;
		mgColor* m_drrect_color1 = 0;
		mgColor* m_drrect_color2 = 0;
		mgRectf m_drrect_rect;
		void _drawRectangle();

		bool D3D11_createConstantBuffer(int byteSize, ID3D11Buffer** cb);
		bool D3D11_createGeometryShaders(const char* target, const char* shaderText, const char* entryPoint, ID3D11GeometryShader** gs);
		bool D3D11_createShaders(const char* vertexTarget, const char* pixelTarget, const char* vertexShader,
			const char* pixelShader, const char* vertexEntryPoint, const char* pixelEntryPoint,
			ID3D11VertexShader** vs, ID3D11PixelShader** ps, ID3D11InputLayout** il);
		bool _createShader();
		struct cbVertex {
			mgf::Mat4 m_ProjMtx;
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


		mgTexture_s* m_whiteTexture = 0;
		BackendD3D11_texture* m_activeTexture = 0;
		ID3D11DepthStencilState* m_oldDepthStencilState = 0;
		UINT m_oldDepthStencilRef = 0;
		ID3D11DepthStencilState* m_depthStencilStateDisabled = 0;
		ID3D11RasterizerState* m_oldRasterizerState = 0;
		ID3D11RasterizerState* m_RasterizerSolidNoBackFaceCulling = 0;
		ID3D11BlendState* m_oldBlendState = 0;
		float m_oldBlendStateBlendFactor[4];
		UINT m_oldBlendStateSampleMask = 0;
		ID3D11BlendState* m_blendStateAlphaEnabled = 0;

	public:
		BackendD3D11(BackendD3D11Params);
		virtual ~BackendD3D11();
		virtual void* GetVideoDriverAPI() override;

		virtual void InitWindow(mgf::SystemWindow*) override;
		virtual mgf::SystemWindow* SetCurrentWindow(mgf::SystemWindow*) override;
		virtual void SetActiveContext(mgf::Context*) override;

		virtual void UpdateBackbuffer() override;

		virtual void BeginDraw(int reason) override;
		virtual void EndDraw() override;

		virtual mgTexture* CreateTexture(mgImage* img)  override;
		virtual void DestroyTexture(mgTexture*)  override;
		virtual void DrawRectangle(int reason,void* object,mgRect* rct,mgColor* color1,mgColor* color2,
			mgTexture* texture,  mgVec4* UVRegion ) override;
		virtual void DrawText(int reason,void* object,mgPoint* position,const wchar_t* text,int textLen,
			mgColor* color, mgFont* font) override;
		virtual void DrawLine( int reason, void* object, mgPoint* position, mgPoint* where,
			mgColor* color, int size) override;
		virtual mgRect SetClipRect(mgRect* r) override;
		virtual void GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz) override;
		virtual mgTexture_s* GetDefaultIcons() override;
		virtual Font* CreateFont(const wchar_t* file, int size, bool bold, bool italic) override;
		virtual void DestroyFont(Font*) override;
		virtual Font* GetDefaultFont() override;
	};

}

#endif

#endif