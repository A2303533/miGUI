﻿/*
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

#include "mgf.h"
#ifdef MGF_GS_D3D11

#include "GSD3D11.h"
#include <d3dcompiler.h>
#include "GSD3D11Shader.h"
#include "Log.h"

extern PFND3DCOMPILEPROC g_D3DCompile;


using namespace mgf;

bool mgf::GSD3D11_createConstantBuffer(ID3D11Device* d, uint32_t byteSize, ID3D11Buffer** cb)
{
	D3D11_BUFFER_DESC mbd;
	memset(&mbd, 0, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = byteSize;
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	HRESULT hr = d->CreateBuffer(&mbd, 0, cb);
	if (FAILED(hr))
	{
		LogWriteError("Can't create constant buffer. Error code [%u]\n", hr);
		return false;
	}
	return true;
}

bool mgf::GSD3D11_createGeometryShaders(
	ID3D11Device* d,
	const char* target,
	const char* shaderText,
	const char* entryPoint,
	ID3D11GeometryShader** gs) {
	ID3D10Blob* m_GsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;
	HRESULT hr = g_D3DCompile(
		shaderText,
		strlen(shaderText),
		0, 0, 0,
		entryPoint,
		target,
		0,
		0,
		&m_GsBlob,
		&m_errorBlob
	);
	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		LogWriteError("Geometry shader compile error: %s\n", message);
		return false;
	}

	hr = d->CreateGeometryShader(
		m_GsBlob->GetBufferPointer(),
		m_GsBlob->GetBufferSize(),
		0,
		gs);
	if (FAILED(hr))
	{
		LogWriteError("Can't create geometry shader. Error code [%u]\n", hr);
		return false;
	}

	return true;
}

bool mgf::GSD3D11_createShaders(
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
	ID3D11InputLayout** il)
{
	ID3D10Blob* m_VsBlob = nullptr;
	ID3D10Blob* m_PsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;

	HRESULT hr = g_D3DCompile(
		vertexShader,
		strlen(vertexShader),
		0, 0, 0,
		vertexEntryPoint,
		vertexTarget,
		0,
		0,
		&m_VsBlob,
		&m_errorBlob
	);

	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		LogWriteError("Vertex shader compile error: %s\n", message);
		return false;
	}

	hr = g_D3DCompile(
		pixelShader,
		strlen(pixelShader),
		0, 0, 0,
		pixelEntryPoint,
		pixelTarget,
		0,
		0,
		&m_PsBlob,
		&m_errorBlob
	);

	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		LogWriteError("Pixel shader compile error: %s\n", message);
		return false;
	}

	hr = d->CreateVertexShader(
		m_VsBlob->GetBufferPointer(),
		m_VsBlob->GetBufferSize(),
		0,
		vs);
	if (FAILED(hr))
	{
		LogWriteError("Can't create vertex shader. Error code [%u]\n", hr);
		return false;
	}

	hr = d->CreatePixelShader(
		m_PsBlob->GetBufferPointer(),
		m_PsBlob->GetBufferSize(),
		0,
		ps);
	if (FAILED(hr))
	{
		LogWriteError("Can't create pixel shader. Error code [%u]\n", hr);
		return false;
	}

	if (vertexType != Mesh::MeshVertexType_Null)
	{
		D3D11_INPUT_ELEMENT_DESC vertexLayout[8];
		uint32_t vertexLayoutSize = 0;
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
		switch (vertexType)
		{
		case Mesh::MeshVertexType_GUI:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 8;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case Mesh::MeshVertexType_Triangle:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case Mesh::MeshVertexType_AnimatedTriangle:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 72;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 88;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		case Mesh::MeshVertexType_Line:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 24;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case Mesh::MeshVertexType_AnimatedLine:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 24;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 40;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case Mesh::MeshVertexType_Point:
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
			break;
		case Mesh::MeshVertexType_AnimatedPoint:
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

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 28;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		default:
			LogWriteError("Unsupportex vertex type\n");
			return false;
		}
		vertexLayoutSize = ind + 1;

		hr = d->CreateInputLayout(
			vertexLayout,
			vertexLayoutSize,
			m_VsBlob->GetBufferPointer(),
			m_VsBlob->GetBufferSize(),
			il);
		if (FAILED(hr))
		{
			LogWriteError("Can't create input layout. Error code [%u]\n", hr);
			return false;
		}
	}


	if (m_VsBlob)    m_VsBlob->Release();
	if (m_PsBlob)    m_PsBlob->Release();
	if (m_errorBlob) m_errorBlob->Release();

	return true;
}
#endif



