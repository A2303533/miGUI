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

#pragma once
#ifndef __MGF_GSD3D11SHADER_H_
#define __MGF_GSD3D11SHADER_H_

#include "Mesh.h"
#include "GSShader.h"

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
		Mesh::MeshVertexType vertexType,
		ID3D11VertexShader** vs,
		ID3D11PixelShader** ps,
		ID3D11InputLayout** il);
	bool GSD3D11_createConstantBuffer(ID3D11Device* d, uint32_t byteSize, ID3D11Buffer**);
	bool GSD3D11_createGeometryShaders(ID3D11Device* d, const char* target,
		const char* shaderText,
		const char* entryPoint,
		ID3D11GeometryShader** gs);

	class GSD3D11Shader : public GSShader
	{
	public:
		GSD3D11Shader() {}
		virtual ~GSD3D11Shader() {}

		ID3D11VertexShader* m_vShader = 0;
		ID3D11PixelShader* m_pShader = 0;
		ID3D11GeometryShader* m_gShader = 0;
		ID3D11InputLayout* m_vLayout = 0;
	};

}

#endif
