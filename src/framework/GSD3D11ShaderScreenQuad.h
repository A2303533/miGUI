#pragma once
#ifndef MGF_D3D11_SHADER_SCREENQUAD_H__
#define MGF_D3D11_SHADER_SCREENQUAD_H__

#include "GSD3D11Shader.h"

namespace mgf
{

	class GSD3D11ShaderScreenQuad : public GSD3D11Shader
	{
		ID3D11Device* m_device = 0;
	public:
		GSD3D11ShaderScreenQuad(ID3D11Device*);
		virtual ~GSD3D11ShaderScreenQuad();


		bool init();
		virtual void SetConstants(void* material) {}
	};

}

#endif