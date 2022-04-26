#pragma once
#ifndef __MGF_GSD3D11SHADER_H_
#define __MGF_GSD3D11SHADER_H_

#include "Mesh.h"

namespace mgf
{

	bool GSD3D11_createShaders(
		ID3D11Device* d,
		const char* vertexTarget,
		const char* pixelTarget,
		const char* vertexShader,
		const char* pixelShader,
		const char* vertexEntryPoint,
		const char* pixelEntryPoint,
		MeshVertexType vertexType,
		ID3D11VertexShader** vs,
		ID3D11PixelShader** ps,
		ID3D11InputLayout** il);
	bool GSD3D11_createConstantBuffer(ID3D11Device* d, uint32_t byteSize, ID3D11Buffer**);
	bool GSD3D11_createGeometryShaders(ID3D11Device* d, const char* target,
		const char* shaderText,
		const char* entryPoint,
		ID3D11GeometryShader** gs);

	class GSD3D11Shader
	{
	public:
		GSD3D11Shader() {}
		virtual ~GSD3D11Shader() {}
		virtual void SetConstants(void* material) = 0;

		ID3D11VertexShader* m_vShader = 0;
		ID3D11PixelShader* m_pShader = 0;
		ID3D11GeometryShader* m_gShader = 0;
		ID3D11InputLayout* m_vLayout = 0;
	};

}

#endif
