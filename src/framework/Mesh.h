#pragma once
#ifndef __MGF_MESH_H_
#define __MGF_MESH_H_

#include "framework/Vec.h"
#include "framework/String.h"
#include "framework/Aabb.h"

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

	// тип
	// ещё надо будет добавить для анимированных моделей
	enum class MeshVertexType : uint32_t
	{
		Null, // 0
		GUI,
		Triangle,
		AnimatedTriangle,
		Line,
		AnimatedLine,
		Point,
		AnimatedPoint,
	};

	// предпологается что при создании MDL индекс будет устанавливаться автоматически, в зависимости от количества треугольников 
	//  пока не протестировано. условие такое if (modelInds.size() / 3 > 21845) newModel->m_indexType = yyMeshIndexType::uint32_t; 
	enum class MeshIndexType : uint32_t
	{
		_u16,
		_u32
	};


	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		void calculateTangents(mgf::v3f& normal, mgf::v3f& tangent, mgf::v3f& binormal,
			const mgf::v3f& vt1, const mgf::v3f& vt2, const mgf::v3f& vt3, // vertices
			const mgf::v2f& tc1, const mgf::v2f& tc2, const mgf::v2f& tc3); // texture coords

		void GenerateTangents_u16();
		void GenerateTangents_u32();
		void GenerateTangents();

		mgf::Aabb m_aabb;

		uint8_t* m_vertices = nullptr;
		uint8_t* m_indices = nullptr;
		MeshIndexType m_indexType = MeshIndexType::_u16;

		uint32_t m_vCount = 0;
		uint32_t m_iCount = 0;
		uint32_t m_stride = 0;

		MeshVertexType m_vertexType = MeshVertexType::Triangle;

		mgf::StringA m_name;
	};

}
#endif
