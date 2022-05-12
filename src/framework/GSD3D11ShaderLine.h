#pragma once
#ifndef MGF_D3D11_SHADER_LINE_H__
#define MGF_D3D11_SHADER_LINE_H__

#include "GSD3D11Shader.h"

namespace mgf
{

	class GSD3D11ShaderLine : public GSD3D11Shader
	{
		ID3D11Device* m_device = 0;
		ID3D11DeviceContext* m_deviceCotext = 0;
	public:
		GSD3D11ShaderLine(ID3D11Device*, ID3D11DeviceContext*);
		virtual ~GSD3D11ShaderLine();

		ID3D11Buffer* m_cb;

		struct cb
		{
			Mat4 VP;
			v4f  P1;
			v4f  P2;
			mgColor  Color;
		}m_cbData;

		bool init();
		virtual void SetConstants(void* material) override;

		void SetData(const v4f& p1, const v4f& p2, const mgColor& c, Mat4*);
	};

}

#endif