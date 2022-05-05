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
#include "framework/Ray.h"

#include "CRT/float.h"

using namespace mgf;

Ray::Ray()
	:
	m_kz(0),
	m_kx(0),
	m_ky(0),
	m_Sx(0.f),
	m_Sy(0.f),
	m_Sz(0.f)
{}

Ray::~Ray() {}

void Ray::createFrom2DCoords(const v2f& coord, const v4f& rc, const v2f& rc_sz, const Mat4& VPinv)
{
	v2f point;
	point.x = coord.x - rc.x;
	point.y = coord.y - rc.y;

	// координаты курсора от -1 до +1
	float pt_x = (point.x / rc_sz.x) * 2.f - 1.f;
	float pt_y = -(point.y / rc_sz.y) * 2.f + 1.f;

	//                                           0.f - for d3d
	m_origin = math::mul(v4f(pt_x, pt_y, -1.f, 1.f), VPinv);
	m_end = math::mul(v4f(pt_x, pt_y, 1.f, 1.f), VPinv);

	m_origin.w = 1.0f / m_origin.w;
	m_origin.x *= m_origin.w;
	m_origin.y *= m_origin.w;
	m_origin.z *= m_origin.w;

	m_end.w = 1.0f / m_end.w;
	m_end.x *= m_end.w;
	m_end.y *= m_end.w;
	m_end.z *= m_end.w;

	update();
}

float Ray::distanceToLine(const v4f& lineP0, const v4f& lineP1)
{
	v4f u = m_end - m_origin;
	v4f v = lineP1 - lineP0;
	v4f w = m_origin - lineP0;
	u.w = 0.f;
	v.w = 0.f;
	w.w = 0.f;
	float a = u.dot();
	float b = u.dot(v);
	float c = v.dot();
	float d = u.dot(w);
	float e = v.dot(w);
	float D = a * c - b * b;
	float sc, tc;

	if (D < FLT_EPSILON)
	{
		sc = 0.f;
		tc = (b > c ? d / b : e / c);
	}
	else
	{
		sc = (b * e - c * d) / D;
		tc = (a * e - b * d) / D;
	}

	v4f dP = w + (sc * u) - (tc * v);
	dP.w = 0.f;
	return std::sqrt(dP.dot());
}

void Ray::update()
{
	m_direction.x = m_end.x - m_origin.x;
	m_direction.y = m_end.y - m_origin.y;
	m_direction.z = m_end.z - m_origin.z;
	m_direction.w = 0.f;
	m_direction.normalize2();

	m_invDir.x = 1.f / m_direction.x;
	m_invDir.y = 1.f / m_direction.y;
	m_invDir.z = 1.f / m_direction.z;
	m_invDir.w = 1.f / m_direction.w;

	m_kz = max_dim
	(
		v4f
		(
			std::abs(m_direction.x),
			std::abs(m_direction.y),
			std::abs(m_direction.z),
			1.f
		)
	);

	m_kx = m_kz + 1;
	if (m_kx == 3)
		m_kx = 0;

	m_ky = m_kx + 1;
	if (m_ky == 3)
		m_ky = 0;

	float* dir_data = m_direction.data();
	if (dir_data[m_kz])
		std::swap(m_kx, m_ky);

	m_Sx = dir_data[m_kx] / dir_data[m_kz];
	m_Sy = dir_data[m_ky] / dir_data[m_kz];
	m_Sz = 1.f / dir_data[m_kz];
}

void Ray::getIntersectionPoint(float t, v4f& ip)
{
	ip = m_origin + t * m_direction;
}

bool Ray::planeIntersection(const v4f& planePoint, const v4f& planeNormal, float& T) 
{
	float det = (planeNormal.x * m_direction.x) + (planeNormal.y * m_direction.y) + (planeNormal.z * m_direction.z);

	if (std::fabs(det) < FLT_EPSILON) return false;

	v4f v;
	v.x = planePoint.x - m_origin.x;
	v.y = planePoint.y - m_origin.y;
	v.z = planePoint.z - m_origin.z;

	T = (planeNormal.x * v.x) + (planeNormal.y * v.y) + (planeNormal.z * v.z);

	T /= det;
	return true;
}