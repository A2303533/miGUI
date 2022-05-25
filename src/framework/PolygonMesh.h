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
#ifndef _MGF_POLYGONMESH_H_
#define _MGF_POLYGONMESH_H_

namespace mgf
{
	class Polygon;
	class Edge;
	class Vertex;
	class Skeleton;
	class PolygonMesh
	{
	public:
		PolygonMesh() {}
		~PolygonMesh() {}

		Polygon* m_first_polygon = 0;
		Edge* m_first_edge = 0;
		Vertex* m_first_vertex = 0;
		Skeleton* m_skeleton = 0;

		uint32_t m_vertexCount = 0;
		uint32_t m_edgeCount = 0;
		uint32_t m_polygonCount = 0;
		uint32_t m_uvCount = 0;

		void UpdateCounts();

		// must be for each polygon.
		// array contain first miUVChannel* for each channel
		//miArray<miUVChannelNode*> m_UVChannels;

		void CreateEdges();
		void _delete_edges();
		void _add_edge_to_list(Edge* newEdge);
		void _add_vertex_to_list(Vertex* newVertex);
		void _add_polygon_to_list(Polygon* newPolygon);
		void _remove_edge_from_list(Edge* o);
		void _remove_vertex_from_list(Vertex* o);
		void _remove_polygon_from_list(Polygon* o);
	};
}

#endif
