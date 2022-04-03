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

#include "framework/mgf.h"
#include "framework/Aabb.h"

using namespace mgf;

Aabb::Aabb()
	:
	m_min(v4f(FLT_MAX)),
	m_max(v4f(-FLT_MAX))
{}

Aabb::Aabb(const v4f& min, const v4f& max) :m_min(min), m_max(max) { }

Aabb::~Aabb() {}

/*
Transforming Axis-Aligned Bounding Boxes
by Jim Arvo
from "Graphics Gems", Academic Press, 1990
*/
void Aabb::transform(Aabb* original, Mat4* matrix, v4f* position) {
	float  a, b;
	float  Amin[3], Amax[3];
	float  Bmin[3], Bmax[3];
	int    i, j;

	/*Copy box A into a min array and a max array for easy reference.*/
	Amin[0] = original->m_min.x;  Amax[0] = original->m_max.x;
	Amin[1] = original->m_min.y;  Amax[1] = original->m_max.y;
	Amin[2] = original->m_min.z;  Amax[2] = original->m_max.z;

	/* Take care of translation by beginning at T. */
	Bmin[0] = Bmax[0] = position->x;
	Bmin[1] = Bmax[1] = position->y;
	Bmin[2] = Bmax[2] = position->z;

	/* Now find the extreme points by considering the product of the */
	/* min and max with each component of M.  */
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// mimi: I swapped j and i
			a = (float)(matrix->m_data[j][i] * Amin[j]);
			b = (float)(matrix->m_data[j][i] * Amax[j]);
			if (a < b)
			{
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else
			{
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}
	}
	/* Copy the result into the new box. */
	m_min.x = Bmin[0];  m_max.x = Bmax[0];
	m_min.y = Bmin[1];  m_max.y = Bmax[1];
	m_min.z = Bmin[2];  m_max.z = Bmax[2];
}

void Aabb::add(const v4f& point)
{
	if (point.x < m_min.x) m_min.x = point.x;
	if (point.y < m_min.y) m_min.y = point.y;
	if (point.z < m_min.z) m_min.z = point.z;

	if (point.x > m_max.x) m_max.x = point.x;
	if (point.y > m_max.y) m_max.y = point.y;
	if (point.z > m_max.z) m_max.z = point.z;
}

void Aabb::add(const v3f& point)
{
	if (point.x < m_min.x) m_min.x = point.x;
	if (point.y < m_min.y) m_min.y = point.y;
	if (point.z < m_min.z) m_min.z = point.z;

	if (point.x > m_max.x) m_max.x = point.x;
	if (point.y > m_max.y) m_max.y = point.y;
	if (point.z > m_max.z) m_max.z = point.z;
}

void Aabb::add(const Aabb& box)
{
	if (box.m_min.x < m_min.x) m_min.x = box.m_min.x;
	if (box.m_min.y < m_min.y) m_min.y = box.m_min.y;
	if (box.m_min.z < m_min.z) m_min.z = box.m_min.z;

	if (box.m_max.x > m_max.x) m_max.x = box.m_max.x;
	if (box.m_max.y > m_max.y) m_max.y = box.m_max.y;
	if (box.m_max.z > m_max.z) m_max.z = box.m_max.z;
}

bool Aabb::rayTest(const Ray& r)
{
	float t1 = (m_min.x - r.m_origin.x) * r.m_invDir.x;
	float t2 = (m_max.x - r.m_origin.x) * r.m_invDir.x;
	float t3 = (m_min.y - r.m_origin.y) * r.m_invDir.y;
	float t4 = (m_max.y - r.m_origin.y) * r.m_invDir.y;
	float t5 = (m_min.z - r.m_origin.z) * r.m_invDir.z;
	float t6 = (m_max.z - r.m_origin.z) * r.m_invDir.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax) return false;

	return true;
}

void Aabb::center(v4f& v) const
{
	v = v4f(m_min + m_max);
	v *= 0.5f;
}

float Aabb::radius() 
{
	return m_min.distance(m_max) * 0.5f;
}

void Aabb::extent(v4f& v) 
{
	v = v4f(m_max - m_min); 
}

bool Aabb::isEmpty() const
{
	return ((m_min == v4f(FLT_MAX)) && (m_max == v4f(-FLT_MAX)))
		|| (m_min == m_max);
}

void Aabb::reset() 
{ 
	m_min = v4f(FLT_MAX);
	m_max = v4f(-FLT_MAX);
}