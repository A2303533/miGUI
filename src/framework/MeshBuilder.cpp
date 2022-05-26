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

#include "mgf.h"
#include "MeshBuilder.h"
#include "PolygonCreator.h"
#include "Array.h"

using namespace mgf;

void MeshBuilder::_set_hash(v3f* position)
{
	m_vertsMapHash.clear();
	char* ptr = (char*)position->data();
	char bytes[13];
	bytes[0] = ptr[0];
	bytes[1] = ptr[1];
	bytes[2] = ptr[2];
	bytes[3] = ptr[3];
	bytes[4] = ptr[4];
	bytes[5] = ptr[5];
	bytes[6] = ptr[6];
	bytes[7] = ptr[7];
	bytes[8] = ptr[8];
	bytes[9] = ptr[9];
	bytes[10] = ptr[10];
	bytes[11] = ptr[11];
	if (bytes[0] == 0) bytes[0] = 1;
	if (bytes[1] == 0) bytes[1] = 1;
	if (bytes[2] == 0) bytes[2] = 1;
	if (bytes[3] == 0) bytes[3] = 1;
	if (bytes[4] == 0) bytes[4] = 1;
	if (bytes[5] == 0) bytes[5] = 1;
	if (bytes[6] == 0) bytes[6] = 1;
	if (bytes[7] == 0) bytes[7] = 1;
	if (bytes[8] == 0) bytes[8] = 1;
	if (bytes[9] == 0) bytes[9] = 1;
	if (bytes[10] == 0) bytes[10] = 1;
	if (bytes[11] == 0) bytes[11] = 1;
	bytes[12] = 0;
	m_vertsMapHash = bytes;
}

Mesh* MeshBuilder::CreateMesh(uint32_t flags)
{
	if (!m_mesh)
		return 0;
	if (!m_mesh->m_first_polygon)
		return 0;

	v4f color(1.f);

	Mesh* newMesh = new Mesh;
	newMesh->m_indexType = Mesh::MeshIndexType::MeshIndexType_u32;
	newMesh->m_vertexType = Mesh::MeshVertexType::MeshVertexType_Triangle;
	newMesh->m_stride = sizeof(VertexTriangle);

	//newMesh->m_vertices = (uint8_t*)malloc(newMesh->m_stride * 3);
	//newMesh->m_indices = (uint8_t*)malloc(sizeof(uint32_t) * 3);

	//VertexTriangle* vertexModel_ptr = (VertexTriangle*)newMesh->m_vertices;
	//uint32_t* inds_ptr = (uint32_t*)newMesh->m_indices;
	mgf::Array<VertexTriangle> vertArr;
	mgf::Array<uint32_t> indsArr;

	vertArr.reserve(5000);
	indsArr.reserve(5000);

	VertexTriangle currVert;

	uint32_t index = 0;

	auto current_polygon = m_mesh->m_first_polygon;
	auto last_polygon = current_polygon->m_left;
	while (true)
	{

		auto vertex_1 = current_polygon->m_verts.m_head;
		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true)
		{
			currVert.Color = color;
			currVert.Position = vertex_1->m_data.m_vertex->m_position;
			currVert.UV = vertex_1->m_data.m_uv;
			currVert.Normal = vertex_1->m_data.m_normal;
			//++vertexModel_ptr;
			vertArr.push_back(currVert);
			indsArr.push_back(index);

			//*inds_ptr = index;
			++index;
			//++inds_ptr;

			currVert.Color = color;
			currVert.Position = vertex_2->m_data.m_vertex->m_position;
			currVert.UV = vertex_2->m_data.m_uv;
			currVert.Normal = vertex_2->m_data.m_normal;
			//++vertexModel_ptr;
			vertArr.push_back(currVert);
			indsArr.push_back(index);

			//*inds_ptr = index;
			++index;
			//++inds_ptr;

			currVert.Color = color;
			currVert.Position = vertex_3->m_data.m_vertex->m_position;
			currVert.UV = vertex_3->m_data.m_uv;
			currVert.Normal = vertex_3->m_data.m_normal;
			//++vertexModel_ptr;
			vertArr.push_back(currVert);
			indsArr.push_back(index);

			//*inds_ptr = index;
			++index;
			//++inds_ptr;

			newMesh->m_vCount += 3;
			newMesh->m_iCount += 3;

			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_2 == vertex_1)
				break;
		}				

		if (current_polygon == last_polygon)
			break;
		current_polygon = current_polygon->m_right;
	}

	newMesh->m_vertices = (uint8_t*)vertArr.m_data;
	newMesh->m_indices = (uint8_t*)indsArr.m_data;

	vertArr.m_data = 0;
	indsArr.m_data = 0;

	return newMesh;
}

void MeshBuilder::Clear()
{
	if (!m_mesh)
		return;
	m_mesh->_delete_edges();
	if (m_mesh->m_first_polygon)
	{
		auto p = m_mesh->m_first_polygon;
		auto last = p->m_left;
		while (true) {
			auto next = p->m_right;
			
			delete p;

			if (p == last)
				break;
			p = next;
		}
	}


	if (m_mesh->m_first_vertex)
	{
		auto v = m_mesh->m_first_vertex;
		auto last = v->m_left;
		while (true) {
			auto next = v->m_right;
			delete v;
			if (v == last)
				break;
			v = next;
		}
	}
	delete m_mesh;
	m_mesh = 0;
}

void MeshBuilder::AddPolygon(PolygonCreator* pc, bool genNormals) {
	auto polygonVertexCount = pc->Size();
	if (polygonVertexCount < 3)
		return;

	if (!m_mesh)
		m_mesh = new PolygonMesh;

	Polygon* newPolygon = new Polygon;

	if (!m_mesh->m_first_polygon)
	{
		m_mesh->m_first_polygon = newPolygon;
		m_mesh->m_first_polygon->m_right = m_mesh->m_first_polygon;
		m_mesh->m_first_polygon->m_left = m_mesh->m_first_polygon;
	}
	else
	{
		auto last = m_mesh->m_first_polygon->m_left;
		last->m_right = newPolygon;
		newPolygon->m_left = last;
		newPolygon->m_right = m_mesh->m_first_polygon;
		m_mesh->m_first_polygon->m_left = newPolygon;
	}

	auto positions = pc->GetPositions();
	auto normals = pc->GetNormals();
	auto tCoords = pc->GetTCoords();

	for (int i = 0; i < polygonVertexCount; ++i)
	{
		Vertex* newVertex = 0;
		m_aabb.add(positions[i].m_first);

		if (positions[i].m_second & PolygonCreator::flag_weld)
		{
			_set_hash(&positions[i].m_first);

			auto find_result = m_weldMap.find(m_vertsMapHash);
			if (find_result == m_weldMap.end())
			{
				newVertex = new Vertex;
				newVertex->m_position = positions[i].m_first;

				m_weldMap[m_vertsMapHash] = newVertex;
				m_mesh->_add_vertex_to_list(newVertex);
			}
			else
			{
				newVertex = find_result->second;
			}
		}
		else
		{
			newVertex = new Vertex;
			newVertex->m_position = positions[i].m_first;

			m_mesh->_add_vertex_to_list(newVertex);
		}

		if (positions[i].m_second & PolygonCreator::flag_selected)
			newVertex->m_flags |= Vertex::flag_isSelected;

		// add newPolygon to vertex and newVertex to polygon
		// only once
		if (newVertex->m_polygons.find(newPolygon) == 0)
		{
			newVertex->m_polygons.push_back(newPolygon);
			Polygon::_vertex_data newVD;
			newVD.m_vertex = newVertex;
			newVD.m_uv = tCoords[i];
			newVD.m_normal = normals[i];
			newPolygon->m_verts.push_back(newVD);
		}
	}

	if (genNormals)
		newPolygon->CalculateNormal();
}
