#pragma once
#ifndef __MGF_GSD3D11TEXTURE_H_
#define __MGF_GSD3D11TEXTURE_H_

#include "GSTexture.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11RenderTargetView;

namespace mgf
{
	class GSD3D11Texture : public GSTexture
	{
	public:
		GSD3D11Texture(GSTextureInfo*, ID3D11Device*, ID3D11DeviceContext*);
		virtual ~GSD3D11Texture();

		ID3D11Texture2D* m_texture = 0;
		ID3D11ShaderResourceView* m_textureResView = 0;
		ID3D11SamplerState* m_samplerState = 0;
		ID3D11RenderTargetView* m_RTV = 0;
	};

}

#endif
