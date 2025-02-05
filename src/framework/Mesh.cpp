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

// windows.h include winsock.h that can conflict with filesystem
//#define WIN32_LEAN_AND_MEAN // add in project parameters

#include "framework/mgf.h"
#include "Mesh.h"

#include <filesystem>

using namespace mgf;

Mesh::Mesh() 
	:
	m_vertices(0),
	m_indices(0),
	m_indexType(MeshIndexType::MeshIndexType_u16),
	m_vCount(0),
	m_iCount(0),
	m_stride(0),
	m_vertexType(MeshVertexType::MeshVertexType_Triangle)
{}

Mesh::~Mesh()
{
	if (m_vertices) free(m_vertices);
	if (m_indices) free(m_indices);
}

void Mesh::calculateTangents(v3f& normal, v3f& tangent, v3f& binormal,
	const v3f& vt1, const v3f& vt2, const v3f& vt3, // vertices
	const v2f& tc1, const v2f& tc2, const v2f& tc3) // texture coords
{
	v3f e1 = vt2 - vt1;
	v3f e2 = vt3 - vt1;
	normal = e1.cross(e2);
	normal.normalize2();
	float deltaX1 = tc1.x - tc2.x;
	float deltaX2 = tc3.x - tc1.x;
	binormal = (e1 * deltaX2) - (e2 * deltaX1);
	binormal.normalize2();
	float deltaY1 = tc1.y - tc2.y;
	float deltaY2 = tc3.y - tc1.y;
	tangent = (e1 * deltaY2) - (e2 * deltaY1);
	tangent.normalize2();
	v3f txb = tangent.cross(binormal);
	if (txb.dot(normal) < 0.0f)
	{
		tangent *= -1.0f;
		binormal *= -1.0f;
	}
}


void Mesh::GenerateTangents_u16()
{
	uint16_t* idx = (uint16_t*)m_indices;
	VertexTriangle* v = (VertexTriangle*)m_vertices;

	v3f localNormal;
	for (uint32_t i = 0; i < m_iCount; i += 3)
	{
		calculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV);
		//if (recalculateNormals)
		//v[idx[i+0]].Normal=localNormal;

		calculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV);
		//if (recalculateNormals)
		//v[idx[i+1]].Normal=localNormal;

		calculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV);
		//if (recalculateNormals)
		//v[idx[i+2]].Normal=localNormal;
	}
}
void Mesh::GenerateTangents_u32()
{
	uint32_t* idx = (uint32_t*)m_indices;
	VertexTriangle* v = (VertexTriangle*)m_vertices;

	v3f localNormal;
	for (uint32_t i = 0; i < m_iCount; i += 3)
	{
		calculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV);
		//if (recalculateNormals)
		//v[idx[i+0]].Normal=localNormal;

		calculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV);
		//if (recalculateNormals)
		//v[idx[i+1]].Normal=localNormal;

		calculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV);
		//if (recalculateNormals)
		//v[idx[i+2]].Normal=localNormal;
	}
}
void Mesh::GenerateTangents()
{
	if (m_vertexType != MeshVertexType::MeshVertexType_Triangle)
		return;

	switch (m_indexType)
	{
	case MeshIndexType::MeshIndexType_u16:
	default:
		GenerateTangents_u16();
		break;
	case MeshIndexType::MeshIndexType_u32:
		GenerateTangents_u32();
		break;
	}
}

void MeshLoader::LoadMesh(const char* filename, uint32_t uid)
{
	m_currUID = uid;
	std::filesystem::path p = filename;
	LoadMesh(p.c_str(), uid);
}

void MeshLoader::LoadMesh(const wchar_t* filename, uint32_t uid)
{
	m_currUID = uid;
	std::filesystem::path p = filename;
	std::filesystem::path ext = p.extension();

	if (ext == ".obj")
		Mesh_OBJ((const char*)p.u8string().c_str(), this);
}
