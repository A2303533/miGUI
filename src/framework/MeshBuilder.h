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
#ifndef _MGF_MESHBUILDER_H_
#define _MGF_MESHBUILDER_H_

#include <map>
#include <string>
#include "Mesh.h"
#include "Polygon.h"
#include "PolygonMesh.h"
#include "Aabb.h"

namespace mgf
{
	class PolygonCreator;
	class Mesh;

	/// <summary>
	/// Creating 3D meshes using polygons.
	/// Much more easy to load 3D, but not too fast.
	/// Easy to create mesh using code.
	/// </summary>
	class MeshBuilder
	{
	public:
		MeshBuilder() {}
		~MeshBuilder()
		{
			Clear();
		}

		void Clear();

		PolygonMesh* m_mesh;

		std::map<std::string, Vertex*> m_weldMap;
		std::string m_vertsMapHash;

		//v4f m_position;
		Aabb m_aabb;

		bool m_isBegin;
		void Begin() 
		{
			if (m_isBegin) return;
			m_isBegin = true;
			m_weldMap.clear();
		}

		void End() 
		{
			m_isBegin = false;
			CreateEdges();
		}

		void AddPolygon(PolygonCreator* pc, bool genNormals);
		
		void CreateEdges()
		{
			m_mesh->CreateEdges();
		}

		enum
		{
			// If not many indices.
			flag_use16bit = 0x1
		};

		Mesh* CreateMesh(uint32_t flags);
	private:
		void _set_hash(v3f* position);
	};
}
#endif
