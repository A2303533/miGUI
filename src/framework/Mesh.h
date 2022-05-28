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
#ifndef __MGF_MESH_H_
#define __MGF_MESH_H_

#include "framework/Vec.h"
#include "framework/String.h"
#include "framework/Aabb.h"

#include "framework/MeshBuilder.h"

namespace mgf
{
	struct VertexGUI
	{
		mgf::v2f Position;
		mgf::v2f UV;
	};

	struct VertexPoint
	{
		mgf::v3f Position;
		mgf::v4f Color;
	};

	struct VertexAnimatedPoint
	{
		mgf::v3f Position;
		mgf::v4f Color;
		mgf::v4f Weights; // потом надо переделать на байты чтобы уменьшить вес
		mgf::v4i Bones;
	};

	struct VertexTriangle
	{
		mgf::v3f Position;
		mgf::v2f UV;
		mgf::v3f Normal;
		mgf::v3f Binormal;
		mgf::v3f Tangent;
		mgf::v4f Color;
	};

	struct VertexAnimatedTriangle
	{
		mgf::v3f Position;
		mgf::v2f UV;
		mgf::v3f Normal;
		mgf::v3f Binormal;
		mgf::v3f Tangent;
		mgf::v4f Color;

		mgf::v4f Weights; // потом надо переделать на байты чтобы уменьшить вес
		mgf::v4i Bones;
	};

	struct VertexLine
	{
		mgf::v3f Position;
		mgf::v3f Normal;
		mgf::v4f Color;
	};

	struct VertexAnimatedLine
	{
		mgf::v3f Position;
		mgf::v3f Normal;
		mgf::v4f Color;

		mgf::v4f Weights; // потом надо переделать на байты чтобы уменьшить вес
		mgf::v4i Bones;
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		// тип
		// ещё надо будет добавить для анимированных моделей
		enum MeshVertexType
		{
			MeshVertexType_Null, // 0
			MeshVertexType_GUI,
			MeshVertexType_Triangle,
			MeshVertexType_AnimatedTriangle,
			MeshVertexType_Line,
			MeshVertexType_AnimatedLine,
			MeshVertexType_Point,
			MeshVertexType_AnimatedPoint,
		};

		// предпологается что при создании MDL индекс будет устанавливаться автоматически, в зависимости от количества треугольников 
		//  пока не протестировано. условие такое if (modelInds.size() / 3 > 21845) newModel->m_indexType = yyMeshIndexType::uint32_t; 
		enum MeshIndexType
		{
			MeshIndexType_u16,
			MeshIndexType_u32
		};

		void calculateTangents(mgf::v3f& normal, mgf::v3f& tangent, mgf::v3f& binormal,
			const mgf::v3f& vt1, const mgf::v3f& vt2, const mgf::v3f& vt3, // vertices
			const mgf::v2f& tc1, const mgf::v2f& tc2, const mgf::v2f& tc3); // texture coords

		void GenerateTangents_u16();
		void GenerateTangents_u32();
		void GenerateTangents();

		mgf::Aabb m_aabb;

		uint8_t* m_vertices;
		uint8_t* m_indices;
		MeshIndexType m_indexType;

		uint32_t m_vCount;
		uint32_t m_iCount;
		uint32_t m_stride;

		MeshVertexType m_vertexType;

		mgf::StringA m_name;
	};

	class Material;
	class MeshBuilder;

	/// <summary>
	/// Interface for loading 3D files.
	/// Implement OnLoad, call LoadMesh and catch mesh in meshBuilder.
	/// </summary>
	class MeshLoader : public BaseClass
	{
	protected:
		uint32_t m_currUID = 0;
	public:
		MeshLoader() {}
		virtual ~MeshLoader() {}

		/// <summary>
		/// One file can contain many objects.
		/// mat will be deleted!
		/// </summary>
		virtual void OnLoad(mgf::MeshBuilder* meshBuilder, mgf::Material* mat) = 0;

		/// <summary>
		/// Loading 3D object in this way is hard, but I don't want to load many
		/// objects as one. One Mesh object contain only one buffer.
		/// uid is to know what file i load.
		/// For user may be better to create class like MyMesh with std::vector<Mesh*>
		/// </summary>
		virtual void LoadMesh(const char* filename, uint32_t uid);
		virtual void LoadMesh(const wchar_t* filename, uint32_t uid);
	};

	void Mesh_OBJ(const char* fn, MeshLoader*);
}
#endif
