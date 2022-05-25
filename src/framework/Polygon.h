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
#ifndef _MGF_POLYGON_H_
#define _MGF_POLYGON_H_

#include "Vertex.h"
#include "Trianlge.h"

namespace mgf
{
	class Polygon
	{
	public:

		Polygon() {}
		~Polygon() {}

		uint32_t m_flags = 0;
		enum {
			flag_isSelected = 0x1,
			flag_User1 = 0x2,
		};

		void CopyData(Polygon* other) {
			m_flags = other->m_flags;
		}

		Polygon* m_left = 0;
		Polygon* m_right = 0;

		//miList3<miVertex*, v2f, v3f> m_verts;
		struct _vertex_data {
			_vertex_data() :m_vertex(0), m_flags(0) {}
			_vertex_data(Vertex* v) :m_vertex(v), m_flags(0) {}
			_vertex_data(Vertex* v, const v2f& uv, const v3f& normal, uint32_t flags)
				:
				m_vertex(v),
				m_uv(uv),
				m_normal(normal),
				m_flags(flags)
			{}

			bool operator==(const _vertex_data& other) {
				return m_vertex == other.m_vertex;
			}

			enum { flag_isSelected = 1, };

			Vertex* m_vertex;
			v2f m_uv;
			v3f m_normal;
			uint32_t m_flags;
		};
		List<_vertex_data> m_verts;

		List<Edge*> m_edges;

		bool IsSelected() { return (m_flags & flag_isSelected) == flag_isSelected; }
		void Select() { m_flags |= flag_isSelected; }
		void Deselect() { if (m_flags & flag_isSelected) m_flags ^= flag_isSelected; }

		void SelectUVs();
		void DeselectUVs();
		ListNode<_vertex_data>* FindVertex(Vertex* v);
		bool IsVisible();
		void CalculateNormal();
		v3f GetFaceNormal();
		v3f GetFaceNormalCalculateNew();

		void Flip() {
			m_verts.reverse();
		}

		void FixOrder(float lineLineCollisionLen);
	};
}
#endif
