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
#include "Trianlge.h"
#include "Ray.h"

namespace mgf
{
	extern float g_Infinity;
}

using namespace mgf;

void Triangle::update()
{
	e1 = v4f(v2.x - v1.x,
		v2.y - v1.y,
		v2.z - v1.z,
		0.f);
	e2 = v4f(v3.x - v1.x,
		v3.y - v1.y,
		v3.z - v1.z,
		0.f);
	//	e1.cross(e2, faceNormal);
}

void Triangle::center(v4f& out)
{
	out = (v1 + v2 + v3) * 0.3333333f;
}

bool Triangle::rayTest_MT(const Ray& ray, bool withBackFace, float& T, float& U, float& V, float& W)
{
	v4f  pvec;
	ray.m_direction.cross2(e2, pvec);
	float det = e1.dot(pvec);

	if (withBackFace)
	{
		if (std::fabs(det) < FLT_EPSILON)
			return false;
	}
	else
	{
		if (det < FLT_EPSILON && det > -FLT_EPSILON)
			return false;
	}

	v4f tvec(
		ray.m_origin.x - v1.x,
		ray.m_origin.y - v1.y,
		ray.m_origin.z - v1.z,
		0.f);

	float inv_det = 1.f / det;
	U = tvec.dot(pvec) * inv_det;

	if (U < 0.f || U > 1.f)
		return false;

	v4f  qvec;
	tvec.cross2(e1, qvec);
	V = ray.m_direction.dot(qvec) * inv_det;

	if (V < 0.f || U + V > 1.f)
		return false;

	T = e2.dot(qvec) * inv_det;

	if (T < FLT_EPSILON) return false;

	W = 1.f - U - V;
	return true;
}

bool Triangle::rayTest_Watertight(const Ray& ray, bool withBackFace, float& T, float& U, float& V, float& W)
{
	v1.w = 1.f;
	v2.w = 1.f;
	v3.w = 1.f;
	const auto A = v2 - ray.m_origin;
	const auto B = v3 - ray.m_origin;
	const auto C = v1 - ray.m_origin;

	const float Ax = A[ray.m_kx] - (ray.m_Sx * A[ray.m_kz]);
	const float Ay = A[ray.m_ky] - (ray.m_Sy * A[ray.m_kz]);
	const float Bx = B[ray.m_kx] - (ray.m_Sx * B[ray.m_kz]);
	const float By = B[ray.m_ky] - (ray.m_Sy * B[ray.m_kz]);
	const float Cx = C[ray.m_kx] - (ray.m_Sx * C[ray.m_kz]);
	const float Cy = C[ray.m_ky] - (ray.m_Sy * C[ray.m_kz]);

	U = (Cx * By) - (Cy * Bx);
	V = (Ax * Cy) - (Ay * Cx);
	W = (Bx * Ay) - (By * Ax);

	if (U == 0.f || V == 0.f || W == 0.f)
	{
		double CxBy = (double)Cx * (double)By;
		double CyBx = (double)Cy * (double)Bx;
		U = (float)(CxBy - CyBx);

		double AxCy = (double)Ax * (double)Cy;
		double AyCx = (double)Ay * (double)Cx;
		V = (float)(AxCy - AyCx);

		double BxAy = (double)Bx * (double)Ay;
		double ByAx = (double)By * (double)Ax;
		W = (float)(BxAy - ByAx);
	}

	if (withBackFace)
	{
		if ((U < 0.f || V < 0.f || W < 0.f) &&
			(U > 0.f || V > 0.f || W > 0.f))
			return false;
	}
	else
	{
		if (U < 0.f || V < 0.f || W < 0.f)
			return false;
	}

	float det = U + V + W;

	if (det == 0.f)
		return false;

	const float Az = ray.m_Sz * A[ray.m_kz];
	const float Bz = ray.m_Sz * B[ray.m_kz];
	const float Cz = ray.m_Sz * C[ray.m_kz];
	const float Ts = (U * Az) + (V * Bz) + (W * Cz);

	if (!withBackFace) // CULL
	{
		if (Ts < 0.f || Ts > g_Infinity * det)
			return false;
	}
	else
	{
		if (det < 0.f && (Ts >= 0.f || Ts < g_Infinity * det))
			return false;
		else if (det > 0.f && (Ts <= 0.f || Ts > g_Infinity * det))
			return false;
	}

	const float invDet = 1.f / det;
	U = U * invDet;
	V = V * invDet;
	W = W * invDet;
	T = Ts * invDet;
	if (T < FLT_EPSILON)
		return false;
	return true;
}
