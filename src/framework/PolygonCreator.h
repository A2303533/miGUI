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
#ifndef _MGF_POLYGONCREATOR_H_
#define _MGF_POLYGONCREATOR_H_

#include "Pair.h"

namespace mgf
{

	// Use it when you create new model.
	// Don't forget to call Clear() before\after every polygon
	class PolygonCreator
	{
		int m_size;
		int m_allocated;
		Pair<v3f, uint8_t>* m_positions;
		v3f* m_normals;
		v2f* m_tcoords;

		void _reallocate(int size) {
			Pair<v3f, uint8_t>* newPositions = new Pair<v3f, uint8_t>[size];
			v3f* newNormals = new v3f[size];
			v2f* newtCoords = new v2f[size];

			for (int i = 0; i < m_size; ++i)
			{
				newPositions[i] = m_positions[i];
				newNormals[i] = m_normals[i];
				newtCoords[i] = m_tcoords[i];
			}

			delete[] m_positions;
			delete[] m_normals;
			delete[] m_tcoords;

			m_positions = newPositions;
			m_normals = newNormals;
			m_tcoords = newtCoords;

			m_allocated = size;
		}

	public:
		PolygonCreator() :m_size(0), m_allocated(0), m_positions(0), m_normals(0), m_tcoords(0) {}
		~PolygonCreator() {
			if (m_positions) delete[] m_positions;
			if (m_normals) delete[] m_normals;
			if (m_tcoords) delete[] m_tcoords;
		}

		void Clear() { m_size = 0; }
		int Size() { return m_size; }

		enum
		{
			flag_weld = 0x1,
			flag_selected = 0x2
		};

		void Add(const v3f& position, bool weld, bool selected, const v3f& normal, const v2f& tCoords) {
			if (m_size == m_allocated)
			{
				_reallocate(m_allocated + (int)std::ceil(((2.f + (float)m_allocated) * 0.5f)));
			}
			uint8_t flags = 0;
			if (weld)
				flags |= flag_weld;
			if (selected)
				flags |= flag_selected;

			m_positions[m_size] = Pair<v3f, uint8_t>(position, flags);
			m_normals[m_size] = normal;
			m_tcoords[m_size] = tCoords;
			++m_size;
		}

		Pair<v3f, uint8_t>* GetPositions() { return m_positions; }
		v3f* GetNormals() { return m_normals; }
		v2f* GetTCoords() { return m_tcoords; }
	};

}
#endif
