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
#ifndef _MGF_VEC_H_
#define _MGF_VEC_H_

#include <cmath>
#include <cassert>

namespace mgf
{
	struct v4i
	{
		v4i() :x(0), y(0), z(0), w(0) {}
		v4i(int32_t _x, int32_t _y, int32_t _z, int32_t _w) :x(_x), y(_y), z(_z), w(_w) {}
		v4i(int32_t _v) :x(_v), y(_v), z(_v), w(_v) {}
		void set(int32_t _x, int32_t _y, int32_t _z, int32_t _w) { x = _x; y = _y; z = _z; w = _w; }
		int32_t* data() { return &x; }
		int32_t x, y, z, w;
	};

	struct v3f;
	struct v4f
	{
		v4f() :x(0.f), y(0.f), z(0.f), w(0.f) {}
		v4f(const v3f& v);
		v4f(float _v) :x(_v), y(_v), z(_v), w(_v) {}
		v4f(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
		float x, y, z, w;

		const float* cdata()const { return &x; }
		float* data() { return &x; }
		float operator[](int32_t index)const { assert((index >= 0) && (index < 4)); return (&x)[index]; }
		float& operator[](int32_t index) { assert((index >= 0) && (index < 4)); return (&x)[index]; }

		void set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
		void set(float val) { x = y = z = w = val; }
		void operator+=(const v4f& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
		void operator-=(const v4f& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
		void operator*=(const v4f& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
		void operator/=(const v4f& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }

		v4f operator+(const v4f& v)const { v4f r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; r.w = w + v.w; return r; }
		v4f operator-(const v4f& v)const { v4f r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; r.w = w - v.w; return r; }
		v4f operator*(const v4f& v)const { v4f r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; r.w = w * v.w; return r; }
		v4f operator/(const v4f& v)const { v4f r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; r.w = w / v.w; return r; }

		v4f operator-()const { v4f r; r.x = -x; r.y = -y; r.z = -z; r.w = -w; return r; }

		v4f& operator=(const v3f& v);

		float	dot(const v4f& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z) + (w * V2.w); }
		float	dot()const { return (x * x) + (y * y) + (z * z) + (w * w); }
		void	normalize2()
		{
			float len = std::sqrt(dot());
			if (len > 0)
				len = 1.0f / len;
			x *= len; y *= len; z *= len; w *= len;
		}
		void cross2(const v4f& a, v4f& out)const {
			out.x = (y * a.z) - (z * a.y);
			out.y = (z * a.x) - (x * a.z);
			out.z = (x * a.y) - (y * a.x);
		}
		void cross2(const v4f& a, v3f& out)const;
		float lengthSqrt()const { return(x * x) + (y * y) + (z * z); }
		float length()const { return std::sqrt(lengthSqrt()); }
		float distance(const v4f& from)const { return v4f(x - from.x, y - from.y, z - from.z, 1.f).length(); }
		bool operator==(const v4f& v)const {
			if (x != v.x)return false;
			if (y != v.y)return false;
			if (z != v.z)return false;
			if (w != v.w)return false;
			return true;
		}
	};

	struct v2i
	{
		v2i() :x(0), y(0) {}
		v2i(const v2i& o) :x(o.x), y(o.y) {}
		v2i(int32_t X, int32_t Y) :x(X), y(Y) {}
		int32_t x, y;
		v2i operator/(const v2i& v)const { v2i r; r.x = x / v.x; r.y = y / v.y; return r; }
		v2i operator/(int v)const { v2i r; r.x = x / v; r.y = y / v; return r; }
		v2i operator*(float v)const { v2i r; r.x = (int32_t)((float)x * v); r.y = (int32_t)((float)y / v); return r; }
		v2i operator*(const v2i& v)const { v2i r; r.x = x * v.x; r.y = y * v.y; return r; }
		v2i operator-(const v2i& v)const { v2i r; r.x = x - v.x; r.y = y - v.y; return r; }
		v2i operator+(const v2i& v)const { v2i r; r.x = x + v.x; r.y = y + v.y; return r; }
	};

	struct v2f
	{
		v2f() :x(0.f), y(0.f) {}
		v2f(float _x, float _y) :x(_x), y(_y) {}
		v2f(float val) :x(val), y(val) {}
		float x, y;
		void set(float _x, float _y) { x = _x; y = _y; }
		v2f operator*(const v2f& v)const { v2f r; r.x = x * v.x; r.y = y * v.y; return r; }
		v2f operator-(const v2f& v)const { v2f r; r.x = x - v.x; r.y = y - v.y; return r; }
		v2f operator+(const v2f& v)const { v2f r; r.x = x + v.x; r.y = y + v.y; return r; }
		v2f operator/(const v2f& v)const { v2f r; r.x = x / v.x; r.y = y / v.y; return r; }
		v2f& operator=(const v2i& v) { x = (float)v.x; y = (float)v.y; return *this; }
		void operator+=(const v2f& v) { x += v.x; y += v.y; }
		void operator-=(const v2f& v) { x -= v.x; y -= v.y; }
		void operator*=(const v2f& v) { x *= v.x; y *= v.y; }
		void operator/=(const v2f& v) { x /= v.x; y /= v.y; }
		bool operator==(const v2f& v)const { if (x != v.x)return false; if (y != v.y)return false; return true; }
		float distance(const v2f& from)const { return v2f(x - from.x, y - from.y).length(); }
		float length()const { return std::sqrt(lengthSqrt()); }
		float lengthSqrt()const { return(x * x) + (y * y); }
		float	dot()const { return (x * x) + (y * y); }
		void	normalize2()
		{
			float len = std::sqrt(dot());
			if (len > 0)
				len = 1.0f / len;
			x *= len; y *= len;
		}
		v2f operator*(float v)const { v2f r; r.x = x * v; r.y = y * v; return r; }
	};

	struct v3i
	{
		v3i() :x(0), y(0), z(0) {}
		v3i(const v3i& o) :x(o.x), y(o.y), z(o.z) {}
		v3i(int32_t X, int32_t Y, int32_t Z) :x(X), y(Y), z(Z) {}
		int32_t x, y, z;
		v3i operator/(const v3i& v)const { v3i r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
		v3i operator/(int v)const { v3i r; r.x = x / v; r.y = y / v; r.z = z / v; return r; }
	};

	struct v3f
	{
		v3f() :x(0.f), y(0.f), z(0.f) {}
		v3f(const v4f& v) : x(v.x), y(v.y), z(v.z) {}
		v3f(float _v) :x(_v), y(_v), z(_v) {}
		v3f(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
		float x, y, z;
		void set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
		void set(float val) { x = y = z = val; }
		float* data() { return &x; }
		float distance(const v3f& from)const { return v3f(x - from.x, y - from.y, z - from.z).length(); }
		float distance(const v4f& from)const { return v3f(x - from.x, y - from.y, z - from.z).length(); }
		void operator+=(const v3f& v) { x += v.x; y += v.y; z += v.z; }
		void operator-=(const v3f& v) { x -= v.x; y -= v.y; z -= v.z; }
		void operator*=(const v3f& v) { x *= v.x; y *= v.y; z *= v.z; }
		void operator/=(const v3f& v) { x /= v.x; y /= v.y; z /= v.z; }
		float operator[](int32_t index)const { assert((index >= 0) && (index < 3)); return (&x)[index]; }
		float& operator[](int32_t index) { assert((index >= 0) && (index < 3)); return (&x)[index]; }
		v3f operator+(const v3f& v)const { v3f r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
		v3f operator-(const v3f& v)const { v3f r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; }
		v3f operator*(const v3f& v)const { v3f r; r.x = x * v.x; r.y = y * v.y; r.z = z * v.z; return r; }
		v3f operator/(const v3f& v)const { v3f r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
		v3f operator+(const v4f& v)const { v3f r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
		v3f& operator=(const v4f& v) { x = v.x; y = v.y; z = v.z; return *this; }
		void operator+=(const v4f& v) { x += v.x; y += v.y; z += v.z; }
		bool operator==(const v3f& v)const {
			if (x != v.x)return false;
			if (y != v.y)return false;
			if (z != v.z)return false;
			return true;
		}
		bool operator!=(const v3f& v)const {
			if (x != v.x)return true;
			if (y != v.y)return true;
			if (z != v.z)return true;
			return false;
		}
		v3f cross(const v3f& a)const {
			v3f out;
			out.x = (y * a.z) - (z * a.y);
			out.y = (z * a.x) - (x * a.z);
			out.z = (x * a.y) - (y * a.x);
			return out;
		}
		void cross2(const v3f& a, v3f& out)const {
			out.x = (y * a.z) - (z * a.y);
			out.y = (z * a.x) - (x * a.z);
			out.z = (x * a.y) - (y * a.x);
		}
		void	normalize2()
		{
			float len = std::sqrt(dot());
			if (len > 0)
				len = 1.0f / len;
			x *= len; y *= len; z *= len;
		}
		float	dot(const v3f& V2)const { return (x * V2.x) + (y * V2.y) + (z * V2.z); }
		float	dot()const { return (x * x) + (y * y) + (z * z); }
		v3f operator-()const { v3f r; r.x = -x; r.y = -y; r.z = -z; return r; }
		float length() const { return std::sqrt(length2()); }
		float length2() const { return dot(*this); }

		void add(const v4f&);
	};

}
mgf::v4f operator*(const float& s, const mgf::v4f& v);


#endif