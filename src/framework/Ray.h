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
#ifndef __RAY_H_
#define __RAY_H_

#include "framework/Vec.h"
#include "framework/Mat.h"

namespace mgf
{
	inline int max_dim(const v4f& v)
	{
		return (v.x > v.y) ? ((v.x > v.z)
			? 0 : 2) : ((v.y > v.z) ? 1 : 2);
	}

	//const float g_tMax = Infinity;

	class Ray
	{
	public:
		Ray();
		~Ray();

		void createFrom2DCoords(const v2f& coord, const v4f& rc, const v2f& rc_sz, const Mat4& VPinv);
		float distanceToLine(const v4f& lineP0, const v4f& lineP1);
		void update();
		void getIntersectionPoint(float t, v4f& ip);
		bool planeIntersection(const v4f& planePoint, const v4f& planeNormal, float& T);

		v4f m_origin;
		v4f m_end;
		v4f m_direction;
		v4f m_invDir;

		int32_t m_kz;
		int32_t m_kx;
		int32_t m_ky;

		float m_Sx;
		float m_Sy;
		float m_Sz;
	};
}

#endif
