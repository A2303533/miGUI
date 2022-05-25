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
#ifndef _MGF_EDGE_H_
#define _MGF_EDGE_H_

#include <stdint.h>

namespace mgf
{
	class Vertex;
	class Polygon;
	class Edge
	{
	public:
		Edge() {}
		~Edge() {}

		uint32_t m_flags = 0;
		enum
		{
			flag_isSelected = 0x1,
			flag_User1 = 0x2,
		};

		bool IsSelected() { return (m_flags & flag_isSelected) == flag_isSelected; }
		void Select() { m_flags |= flag_isSelected; }
		void Deselect() { if (m_flags & flag_isSelected) m_flags ^= flag_isSelected; }

		void CopyData(Edge* other) {
			m_flags = other->m_flags;
		}

		Edge* m_left = 0;
		Edge* m_right = 0;

		Vertex* m_vertex1 = 0;
		Vertex* m_vertex2 = 0;

		Polygon* m_polygon1 = 0;
		Polygon* m_polygon2 = 0;
	};
}
#endif
