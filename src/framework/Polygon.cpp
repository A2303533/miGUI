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
#include "Polygon.h"
#include "Pair.h"

void mgf::Polygon::SelectUVs() 
{
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		curV->m_data.m_flags |= Polygon::_vertex_data::flag_isSelected;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
}

void mgf::Polygon::DeselectUVs() 
{
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		if (curV->m_data.m_flags & Polygon::_vertex_data::flag_isSelected)
			curV->m_data.m_flags ^= Polygon::_vertex_data::flag_isSelected;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
}

mgf::ListNode<mgf::Polygon::_vertex_data>* mgf::Polygon::FindVertex(mgf::Vertex* v) {
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		if (curV->m_data.m_vertex == v)
			break;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
	return curV;
}

bool mgf::Polygon::IsVisible() 
{
	auto vertex_1 = m_verts.m_head;
	auto vertex_3 = vertex_1->m_right;
	auto vertex_2 = vertex_3->m_right;
	while (true)
	{
		auto a = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
		auto b = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;

		v3f n;
		a.cross2(b, n);

		float area = 0.5f * sqrt(n.dot());

		if (area > 0.001)
			return true;


		vertex_2 = vertex_2->m_right;
		vertex_3 = vertex_3->m_right;
		if (vertex_3 == vertex_1)
			break;
	}
	return false;
}

void mgf::Polygon::CalculateNormal() 
{
	{
		auto vertex_1 = m_verts.m_head;
		vertex_1->m_data.m_normal.set(0.f);

		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true) {
			vertex_2->m_data.m_normal.set(0.f);
			vertex_3->m_data.m_normal.set(0.f);
			// ===============================
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_2 == vertex_1)
				break;
		}
	}
	{
		auto vertex_1 = m_verts.m_head;
		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true) {
			auto e1 = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
			auto e2 = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
			auto n = e1.cross(e2);
			vertex_1->m_data.m_normal -= n;
			vertex_2->m_data.m_normal -= n;
			vertex_3->m_data.m_normal -= n;

			// ===============================
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_2 == vertex_1)
				break;
		}
	}
	{
		auto vertex_1 = m_verts.m_head;
		vertex_1->m_data.m_normal.normalize2();

		auto vertex_3 = vertex_1->m_right;
		auto vertex_2 = vertex_3->m_right;
		while (true) {
			vertex_2->m_data.m_normal.normalize2();
			vertex_3->m_data.m_normal.normalize2();
			// ===============================
			vertex_2 = vertex_2->m_right;
			vertex_3 = vertex_3->m_right;

			if (vertex_2 == vertex_1)
				break;
		}
	}
}

mgf::v3f mgf::Polygon::GetFaceNormal()
{
	v3f n;
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		n += curV->m_data.m_normal;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
	n.normalize2();
	return n;
}

mgf::v3f mgf::Polygon::GetFaceNormalCalculateNew()
{
	v3f _n;
	auto vertex_1 = m_verts.m_head;
	auto vertex_3 = vertex_1->m_right;
	auto vertex_2 = vertex_3->m_right;
	while (true) {
		auto e1 = vertex_2->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
		auto e2 = vertex_3->m_data.m_vertex->m_position - vertex_1->m_data.m_vertex->m_position;
		auto n = e1.cross(e2);
		_n -= n;

		// ===============================
		vertex_2 = vertex_2->m_right;
		vertex_3 = vertex_3->m_right;

		if (vertex_2 == vertex_1)
			break;
	}
	_n.normalize2();
	return _n;
}

void mgf::Polygon::FixOrder(float lineLineCollisionLen) 
{
	// only if 4 or more vertices
	uint32_t num = 0;
	auto curV = m_verts.m_head;
	auto lastV = curV->m_left;
	while (true)
	{
		++num;
		if (curV == lastV)
			break;
		curV = curV->m_right;
	}
	if (num < 4)
		return;

	for (uint32_t i = 0; i < num * 4; ++i)
	{
		curV = m_verts.m_head;
		lastV = curV->m_left;
		while (true)
		{
			mgf::Pair<mgf::ListNode<mgf::Polygon::_vertex_data>*, ListNode<_vertex_data>*> current;
			mgf::Pair<mgf::ListNode<mgf::Polygon::_vertex_data>*, ListNode<_vertex_data>*> prev;

			current.m_first = curV;
			current.m_second = current.m_first->m_right;

			prev.m_second = current.m_first;
			prev.m_first = prev.m_second->m_left;

			auto tgCurr = current.m_second->m_right;
			auto tgLast = tgCurr->m_left;
			while (true)
			{
				Pair<ListNode<_vertex_data>*, ListNode<_vertex_data>*> target;
				target.m_first = tgCurr;
				target.m_second = tgCurr->m_right;

				if (target.m_first == prev.m_first)
					break;

				/*miRay r;
				r.m_origin = current.m_first->m_data.m_vertex->m_position;
				r.m_end = current.m_second->m_data.m_vertex->m_position;

				f32 d = r.distanceToLine(target.m_first->m_data.m_vertex->m_position, target.m_second->m_data.m_vertex->m_position);
				if (d < lineLineCollisionLen)*/
				v3f normal;
				Triangle tri1;
				Triangle tri2;
				{
					Triangle tri3;
					auto vertex = current.m_first;

					tri3.v1 = vertex->m_data.m_vertex->m_position;
					vertex = vertex->m_right;

					tri3.v2 = vertex->m_data.m_vertex->m_position;
					vertex = vertex->m_right;

					tri3.v3 = vertex->m_data.m_vertex->m_position;

					tri3.update();
					tri3.e1.cross2(tri3.e2, normal);
					normal.normalize2();
				}

				tri1.v1 = current.m_first->m_data.m_vertex->m_position;
				tri1.v2 = current.m_first->m_right->m_data.m_vertex->m_position;
				tri1.v3 = current.m_first->m_data.m_vertex->m_position;

				tri2.v1 = current.m_first->m_data.m_vertex->m_position;
				tri2.v2 = current.m_first->m_right->m_data.m_vertex->m_position;
				tri2.v3 = current.m_first->m_right->m_data.m_vertex->m_position;

				//ip = m_origin + t * m_direction;
				tri1.v1 = tri1.v1 + 1.f * normal;
				tri1.v2 = tri1.v2 + 1.f * normal;
				tri1.v3 = tri1.v3 + 1.f * -normal;

				tri2.v1 = tri2.v1 + 1.f * -normal;
				tri2.v2 = tri2.v2 + 1.f * -normal;
				tri2.v3 = tri2.v3 + 1.f * normal;

				//{ // scale
				//	f32 sz = 100.f;
				//	v4f center;
				//	tri.center(center);

				//	tri.v1 = ((tri.v1 - center) * sz) + center;
				//	tri.v2 = ((tri.v2 - center) * sz) + center;
				//	tri.v3 = ((tri.v3 - center) * sz) + center;
				//}
				tri1.update();
				tri2.update();

				Ray r;
				r.m_origin = target.m_first->m_data.m_vertex->m_position;
				r.m_end = target.m_second->m_data.m_vertex->m_position;
				r.update();

				float T = 0.f;
				float U = 0.f;
				float V = 0.f;
				float W = 0.f;

				bool good = tri1.rayTest_MT(r, true, T, U, V, W);

				if (!good)
					good = tri2.rayTest_MT(r, true, T, U, V, W);

				if (good)
				{
					auto tmp = target.m_first->m_data.m_vertex;
					target.m_first->m_data.m_vertex = current.m_second->m_data.m_vertex;
					current.m_second->m_data.m_vertex = tmp;
					goto end;
				}

				if (tgCurr == tgLast)
					break;

				tgCurr = tgCurr->m_right;
			}


			if (curV == lastV)
				break;
			curV = curV->m_right;
		}
	end:;
	}
}
