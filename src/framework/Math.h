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

#ifndef _MGF_MATH_H_
#define _MGF_MATH_H_

#include <math.h>
#include <float.h>

#include "Vec.h"
#include "Quat.h"
#include "Mat.h"

namespace mgf
{
	namespace math
	{

		/*
			   0 : 0
			22.5 : 0.3926991
			  45 : 0.7853982
			67.5 : 1.1780972
			  90 : 1.5707963
		   112.5 : 1.9634954
			 135 : 2.3561945
		   157.5 : 2.7488936
			 180 : 3.1415927
		   202.5 : 3.5342917
			 225 : 3.9269908
		   247.5 : 4.3196899
			 270 : 4.7123890
		   292.5 : 5.1050881
			 315 : 5.4977871
		   337.5 : 5.8904862
			 360 : 0
		*/
		const float PI = static_cast<float>(3.14159265358979323846);
		const float PIHalf = static_cast<float>(3.14159265358979323846 * 0.5);
		const float PIPlusHalf = static_cast<float>(3.14159265358979323846 + PIHalf);
		const float PIPI = 6.2831853f;
		const v4f v4fFltMax = v4f(FLT_MAX);
		const v4f v4fFltMaxNeg = v4f(-FLT_MAX);

		namespace _internal
		{
			/*
			https://stackoverflow.com/questions/6235847/how-to-generate-nan-infinity-and-infinity-in-ansi-c
			unsigned int p = 0x7F800000; // 0xFF << 23
			unsigned int n = 0xFF800000; // 0xFF8 << 20
			unsigned int pnan = 0x7F800001; // or anything above this up to 0x7FFFFFFF
			unsigned int nnan = 0xFF800001; // or anything above this up to 0xFFFFFFFF

			float positiveInfinity = *(float *)&p;
			float negativeInfinity = *(float *)&n;
			float positiveNaN = *(float *)&pnan;
			float negativeNaN = *(float *)&nnan;
			*/
			inline
				float get_infinity()
			{
				unsigned int p = 0x7F800000;
				float inf = *(float*)&p;
				return inf;
			}
		}

		inline
			bool pointInRect(float x, float y, const v4f& rect)
		{
			if (x >= rect.x) { if (x <= rect.z) { if (y >= rect.y) { if (y <= rect.w) { return true; } } } }
			return false;
		}

		inline
			float degToRad(float degrees)
		{
			return degrees * (PI / 180.f);
		}

		inline
			float radToDeg(float radians)
		{
			return radians * (180.f / PI);
		}

		inline
			float clamp(float x, float min, float max)
		{
			if (x < min)
				return min;
			else if (x > max)
				return max;
			return x;
		}

		template<typename T>
		T lerp(const T& a, const T& b, const float t)
		{
			return (T)(a * (1.f - t)) + (b * t);
		}

		// assimp
		inline
			Quat slerp(const Quat& pStart, const Quat& pEnd, const float pFactor)
		{
			Quat pOut;
			float cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

			// adjust signs (if necessary)
			Quat end = pEnd;
			if (cosom < static_cast<float>(0.0))
			{
				cosom = -cosom;
				end.x = -end.x;   // Reverse all signs
				end.y = -end.y;
				end.z = -end.z;
				end.w = -end.w;
			}

			// Calculate coefficients
			float sclp, sclq;
			if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
			{
				// Standard case (slerp)
				float omega, sinom;
				omega = std::acos(cosom); // extract theta from dot product's cos theta
				sinom = std::sin(omega);
				sclp = std::sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
				sclq = std::sin(pFactor * omega) / sinom;
			}
			else
			{
				// Very close, do linear interp (because it's faster)
				sclp = static_cast<float>(1.0) - pFactor;
				sclq = pFactor;
			}

			pOut.x = sclp * pStart.x + sclq * end.x;
			pOut.y = sclp * pStart.y + sclq * end.y;
			pOut.z = sclp * pStart.z + sclq * end.z;
			pOut.w = sclp * pStart.w + sclq * end.w;
			return pOut;
		}

		inline
			float get_0_1(float base, float val)
		{
			return val * (1.f / base);
		}

		inline
			v2f worldToScreen(const Mat4& VP, const v4f& point3D, const v2f& viewportSize, const v2f& offset)
		{
			v4f point = point3D;
			point.w = 1.f;

			point = math::mul(point, VP);

			return v2f
			(
				offset.x + (viewportSize.x * 0.5f + point.x * viewportSize.x * 0.5f / point.w)
				,
				offset.y + (viewportSize.y - (viewportSize.y * 0.5f + point.y * viewportSize.y * 0.5f / point.w))
			);
		}

		inline
			v2f screenToClient(const v2f& screen_coord, const v4f& client_rect)
		{
			return v2f(
				screen_coord.x - client_rect.x,
				screen_coord.y - client_rect.y
			);
		}
	}

}
#endif