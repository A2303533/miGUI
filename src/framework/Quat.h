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
#ifndef _MGF_QUAT_H_
#define _MGF_QUAT_H_

#include <cmath>
#include <cassert>

#include "Vec.h"
#include "Quat.h"

namespace mgf
{
	class Quat
	{
	public:

		float x, y, z, w;

		Quat()
		{
			identity();
		}

		Quat(const Quat& o) :
			x(o.x),
			y(o.y),
			z(o.z),
			w(o.w)
		{
		}

		Quat(float i1, float i2, float i3, float real)
			:
			x(i1),
			y(i2),
			z(i3),
			w(real)
		{
		}

		Quat(const v3f& _axis, const float& _angle)
		{
			setRotation(_axis, _angle);
		}

		Quat(float Euler_x, float Euler_y, float Euler_z)
		{
			set(Euler_x, Euler_y, Euler_z);
		}

		Quat(const v4f& v) { set(v); }

		float* data() { return &x; }
		v4f	get() const { return v4f(x, y, z, w); }
		float	getReal() const { return w; }

		void set(float i1, float i2, float i3, float real)
		{
			x = i1;
			y = i2;
			z = i3;
			w = real;
		}

		void set(const v4f& v) { set(v.x, v.y, v.z); }

		void set(float Euler_x, float Euler_y, float Euler_z)
		{
			Euler_x *= 0.5f;
			Euler_y *= 0.5f;
			Euler_z *= 0.5f;
			float c1 = cos(Euler_x);
			float c2 = cos(Euler_y);
			float c3 = cos(Euler_z);
			float s1 = sin(Euler_x);
			float s2 = sin(Euler_y);
			float s3 = sin(Euler_z);
			w = (c1 * c2 * c3) + (s1 * s2 * s3);
			x = (s1 * c2 * c3) - (c1 * s2 * s3);
			y = (c1 * s2 * c3) + (s1 * c2 * s3);
			z = (c1 * c2 * s3) - (s1 * s2 * c3);
		}

		void setRotation(const v3f& axis, const float& _angle)
		{
			float d = axis.length();
			float s = sin(_angle * 0.5f) / d;
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
			w = cos(_angle * 0.5f);
		}

		void setReal(float r)
		{
			w = r;
		}

		void identity()
		{
			x = 0.f;
			y = 0.f;
			z = 0.f;
			w = 1.f;
		}

		Quat& operator=(const Quat& o)
		{
			x = o.x;
			y = o.y;
			z = o.z;
			w = o.w;
			return *this;
		}

		Quat operator*(const Quat& q)const
		{
			return Quat(
				w * q.x + x * q.w + y * q.z - z * q.y,
				w * q.y + y * q.w + z * q.x - x * q.z,
				w * q.z + z * q.w + x * q.y - y * q.x,
				w * q.w - x * q.x - y * q.y - z * q.z);
		}

		void operator*=(const Quat& q)
		{
			x = w * q.x + x * q.w + y * q.z - z * q.y;
			y = w * q.y + y * q.w + z * q.x - x * q.z;
			z = w * q.z + z * q.w + x * q.y - y * q.x;
			w = w * q.w - x * q.x - y * q.y - z * q.z;
		}

		bool operator!=(const Quat& q)const
		{
			if (x != q.x) return true;
			if (y != q.y) return true;
			if (z != q.z) return true;
			if (w != q.w) return true;
			return false;
		}

		bool operator==(const Quat& q)const
		{
			if (x != q.x) return false;
			if (y != q.y) return false;
			if (z != q.z) return false;
			if (w != q.w) return false;
			return true;
		}

		Quat operator+(const Quat& o) const
		{
			return Quat(
				x + o.x,
				y + o.y,
				z + o.z,
				w + o.w);
		}

		Quat operator-(const Quat& o) const
		{
			return Quat(
				x - o.x,
				y - o.y,
				z - o.z,
				w - o.w);
		}

		Quat operator-()
		{
			x = -x;
			y = -y;
			z = -z;
			return Quat(x, y, z, w);
		}

		float operator[](int32_t index) const
		{
			assert((index >= 0) && (index < 4));
			return (&x)[index];
		}

		float& operator[](int32_t index)
		{
			assert((index >= 0) && (index < 4));
			return (&x)[index];
		}

		float	getLength() { return std::sqrt(x * x + y * y + z * z + w * w); }
		float length2() const { return dot(*this); }

		Quat operator*(float s) const
		{
			return Quat(s * x, s * y, s * z, s * w);
		}

		Quat lerp(Quat q1, Quat q2, float time)
		{
			const float scale = 1.0f - time;
			return (*this = (q1 * scale) + (q2 * time));
		}

		float dot(const Quat& q2) const
		{
			return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
		}

		Quat slerp(Quat q1, Quat q2, float time, float threshold)
		{
			float angle = q1.dot(q2);
			// make sure we use the short rotation
			if (angle < 0.0f)
			{
				q1 = q1 * -1.0f;
				angle *= -1.0f;
			}

			if (angle <= (1 - threshold)) // spherical interpolation
			{
				const float theta = acosf(angle);
				const float invsintheta = 1.f / (sinf(theta));
				const float scale = sinf(theta * (1.0f - time)) * invsintheta;
				const float invscale = sinf(theta * time) * invsintheta;
				return (*this = (q1 * scale) + (q2 * invscale));
			}
			else // linear interploation
				return lerp(q1, q2, time);
		}

		Quat& normalize()
		{
			float len = this->getLength();
			if (len) {
				float len2 = 1.f / len;
				x *= len2;
				y *= len2;
				z *= len2;
				w *= len2;
			}
			return *this;
		}

		void invert()
		{
			x = -x;
			y = -y;
			z = -z;
		}

	};

}


#endif