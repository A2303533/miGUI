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
#ifndef _MGF_MAT_H_
#define _MGF_MAT_H_

#include <cmath>
#include <cassert>

#include "Vec.h"
#include "Quat.h"

namespace mgf
{
	class Mat4
	{
	public:
		v4f m_data[4u]; //< components

		Mat4()
		{
			identity();
		}

		Mat4(const Mat4& m)
		{
			*this = m;
		}

		Mat4(const Quat& q)
		{
			identity();
			setRotation(q);
		}

		Mat4(float v)
		{
			fill(v);
		}

		Mat4(const v4f& x, const v4f& y, const v4f& z, const v4f& w) {
			m_data[0u] = x;
			m_data[1u] = y;
			m_data[2u] = z;
			m_data[3u] = w;
		}

		void		fill(float v) {
			m_data[0u].set(v);
			m_data[1u].set(v);
			m_data[2u].set(v);
			m_data[3u].set(v);
		}

		void		zero() {
			fill(0.f);
		}


		void		identity() {
			auto* p = this->getPtr();
			p[0u] = 1.f;
			p[1u] = 0.f;
			p[2u] = 0.f;
			p[3u] = 0.f;

			p[4u] = 0.f;
			p[5u] = 1.f;
			p[6u] = 0.f;
			p[7u] = 0.f;

			p[8u] = 0.f;
			p[9u] = 0.f;
			p[10u] = 1.f;
			p[11u] = 0.f;

			p[12u] = 0.f;
			p[13u] = 0.f;
			p[14u] = 0.f;
			p[15u] = 1.f;

		}

		void setValue(float xx, float xy, float xz,
			float yx, float yy, float yz,
			float zx, float zy, float zz)
		{
			m_data[0].set(xx, xy, xz, 0.f);
			m_data[1].set(yx, yy, yz, 0.f);
			m_data[2].set(zx, zy, zz, 0.f);
		}

		void setRotation(const Quat& q)
		{
			float d = q.length2();
			float s = 2.0f / d;
			float xs = q.x * s, ys = q.y * s, zs = q.z * s;
			float wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
			float xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
			float yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
			setValue(
				1.0f - (yy + zz), xy - wz, xz + wy,
				xy + wz, 1.0f - (xx + zz), yz - wx,
				xz - wy, yz + wx, 1.0f - (xx + yy));
		}

		void setRotation(const v3f& dir, const v3f& up = v3f(0.f, 1.f, 0.f))
		{
			v3f xaxis = up.cross(dir);
			xaxis.normalize2();

			v3f yaxis = dir.cross(xaxis);
			yaxis.normalize2();

			m_data[0].x = xaxis.x;
			m_data[0].y = yaxis.x;
			m_data[0].z = dir.x;

			m_data[1].x = xaxis.y;
			m_data[1].y = yaxis.y;
			m_data[1].z = dir.y;

			m_data[2].x = xaxis.z;
			m_data[2].y = yaxis.z;
			m_data[2].z = dir.z;
		}

		float* getPtr() { return reinterpret_cast<float*>(&m_data); }
		float* getPtrConst()const { return (float*)&m_data; }

		v4f& operator[](uint32_t i) { assert(i <= 3); return m_data[i]; }
		const v4f& operator[](uint32_t i) const { assert(i <= 3); return m_data[i]; }

		Mat4 operator+(const Mat4& m) const {
			Mat4 out = *this;

			out[0u] += m[0u];
			out[1u] += m[1u];
			out[2u] += m[2u];
			out[3u] += m[3u];

			return out;
		}

		Mat4 operator-(const Mat4& m) const {
			Mat4 out = *this;

			out[0u] -= m[0u];
			out[1u] -= m[1u];
			out[2u] -= m[2u];
			out[3u] -= m[3u];

			return out;
		}

		Mat4 operator*(const Mat4& m) const
		{
			return Mat4(
				m_data[0u] * m[0u].x + m_data[1u] * m[0u].y + m_data[2u] * m[0u].z + m_data[3u] * m[0u].w,
				m_data[0u] * m[1u].x + m_data[1u] * m[1u].y + m_data[2u] * m[1u].z + m_data[3u] * m[1u].w,
				m_data[0u] * m[2u].x + m_data[1u] * m[2u].y + m_data[2u] * m[2u].z + m_data[3u] * m[2u].w,
				m_data[0u] * m[3u].x + m_data[1u] * m[3u].y + m_data[2u] * m[3u].z + m_data[3u] * m[3u].w
			);
		}

		// возможно тут нужно по другому.
		v4f operator*(const v4f& v) const {
			return v4f
			(
				v.x * m_data[0].x + v.y * m_data[1].x + v.z * m_data[2].x + v.w * m_data[2].x,
				v.x * m_data[0].y + v.y * m_data[1].y + v.z * m_data[2].y + v.w * m_data[2].y,
				v.x * m_data[0].z + v.y * m_data[1].z + v.z * m_data[2].z + v.w * m_data[2].z,
				v.x * m_data[0].w + v.y * m_data[1].w + v.z * m_data[2].w + v.w * m_data[2].w
			);
		}

		Mat4 operator/(const Mat4& m) const {
			Mat4 out = *this;

			out[0u] /= m[0u];
			out[1u] /= m[1u];
			out[2u] /= m[2u];
			out[3u] /= m[3u];

			return out;
		}

		Mat4& operator+=(const Mat4& m) {
			m_data[0u] += m[0u];
			m_data[1u] += m[1u];
			m_data[2u] += m[2u];
			m_data[3u] += m[3u];
			return *this;
		}

		Mat4& operator-=(const Mat4& m) {
			m_data[0u] -= m[0u];
			m_data[1u] -= m[1u];
			m_data[2u] -= m[2u];
			m_data[3u] -= m[3u];
			return *this;
		}

		Mat4& operator*=(const Mat4& m) {
			(*this) = (*this) * m;
			return *this;
		}

		Mat4& operator/=(const Mat4& m) {
			m_data[0u] /= m[0u];
			m_data[1u] /= m[1u];
			m_data[2u] /= m[2u];
			m_data[3u] /= m[3u];
			return *this;
		}

		void transpose()
		{
			Mat4 tmp;
			tmp[0u].x = this->m_data[0u].x; //0
			tmp[0u].y = this->m_data[1u].x; //1
			tmp[0u].z = this->m_data[2u].x; //2
			tmp[0u].w = this->m_data[3u].x; //3

			tmp[1u].x = this->m_data[0u].y; //4
			tmp[1u].y = this->m_data[1u].y; //5
			tmp[1u].z = this->m_data[2u].y; //6
			tmp[1u].w = this->m_data[3u].y; //7

			tmp[2u].x = this->m_data[0u].z; //8
			tmp[2u].y = this->m_data[1u].z; //9
			tmp[2u].z = this->m_data[2u].z; //10
			tmp[2u].w = this->m_data[3u].z; //11

			tmp[3u].x = this->m_data[0u].w; //12
			tmp[3u].y = this->m_data[1u].w; //13
			tmp[3u].z = this->m_data[2u].w; //14
			tmp[3u].w = this->m_data[3u].w; //15
			this->m_data[0u] = tmp[0u];
			this->m_data[1u] = tmp[1u];
			this->m_data[2u] = tmp[2u];
			this->m_data[3u] = tmp[3u];
		}

		//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
		bool invert()
		{
			Mat4 mat;
			auto ptr = this->getPtr();
			for (unsigned column = 0; column < 4; ++column)
			{
				// Swap row in case our pivot point is not working
				auto column_data = m_data[column].data();
				if (column_data[column] == 0)
				{
					unsigned big = column;
					for (unsigned row = 0; row < 4; ++row)
					{
						auto row_data = m_data[row].data();
						auto big_data = m_data[big].data();
						if (fabs(row_data[column]) > fabs(big_data[column]))
							big = row;
					}
					// Print this is a singular matrix, return identity ?
					if (big == column)
						fprintf(stderr, "Singular matrix\n");
					// Swap rows                               
					else for (unsigned j = 0; j < 4; ++j)
					{
						auto big_data = m_data[big].data();
						std::swap(column_data[j], big_data[j]);

						auto other_column_data = mat.m_data[column].data();
						auto other_big_data = mat.m_data[big].data();
						std::swap(other_column_data[j], other_big_data[j]);
					}
				}

				// Set each row in the column to 0  
				for (unsigned row = 0; row < 4; ++row)
				{
					if (row != column)
					{
						auto row_data = m_data[row].data();
						float coeff = row_data[column] / column_data[column];
						if (coeff != 0)
						{
							for (unsigned j = 0; j < 4; ++j)
							{
								row_data[j] -= coeff * column_data[j];

								auto other_row_data = mat.m_data[row].data();
								auto other_column_data = mat.m_data[column].data();
								other_row_data[j] -= coeff * other_column_data[j];
							}
							// Set the element to 0 for safety
							row_data[column] = 0;
						}
					}
				}
			}

			// Set each element of the diagonal to 1
			for (unsigned row = 0; row < 4; ++row)
			{
				for (unsigned column = 0; column < 4; ++column)
				{
					auto other_row_data = mat.m_data[row].data();
					auto row_data = m_data[row].data();
					other_row_data[column] /= row_data[row];
				}
			}

			*this = mat;
			return true;
		}

		bool invert2() {
			Mat4 r0, r1, r2, r3;
			float a, det, invDet;
			float* mat = reinterpret_cast<float*>(this);

			det = mat[0 * 4 + 0] * mat[1 * 4 + 1] - mat[0 * 4 + 1] * mat[1 * 4 + 0];
			if (std::abs(det) < 1e-14) {
				return false;
			}
			v4f dd;

			invDet = 1.0f / det;

			r0[0][0] = mat[1 * 4 + 1] * invDet;
			r0[0][1] = -mat[0 * 4 + 1] * invDet;
			r0[1][0] = -mat[1 * 4 + 0] * invDet;
			r0[1][1] = mat[0 * 4 + 0] * invDet;
			r1[0][0] = r0[0][0] * mat[0 * 4 + 2] + r0[0][1] * mat[1 * 4 + 2];
			r1[0][1] = r0[0][0] * mat[0 * 4 + 3] + r0[0][1] * mat[1 * 4 + 3];
			r1[1][0] = r0[1][0] * mat[0 * 4 + 2] + r0[1][1] * mat[1 * 4 + 2];
			r1[1][1] = r0[1][0] * mat[0 * 4 + 3] + r0[1][1] * mat[1 * 4 + 3];
			r2[0][0] = mat[2 * 4 + 0] * r1[0][0] + mat[2 * 4 + 1] * r1[1][0];
			r2[0][1] = mat[2 * 4 + 0] * r1[0][1] + mat[2 * 4 + 1] * r1[1][1];
			r2[1][0] = mat[3 * 4 + 0] * r1[0][0] + mat[3 * 4 + 1] * r1[1][0];
			r2[1][1] = mat[3 * 4 + 0] * r1[0][1] + mat[3 * 4 + 1] * r1[1][1];
			r3[0][0] = r2[0][0] - mat[2 * 4 + 2];
			r3[0][1] = r2[0][1] - mat[2 * 4 + 3];
			r3[1][0] = r2[1][0] - mat[3 * 4 + 2];
			r3[1][1] = r2[1][1] - mat[3 * 4 + 3];

			det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];
			if (std::abs(det) < 1e-14) {
				return false;
			}

			invDet = 1.0f / det;

			a = r3[0][0];
			r3[0][0] = r3[1][1] * invDet;
			r3[0][1] = -r3[0][1] * invDet;
			r3[1][0] = -r3[1][0] * invDet;
			r3[1][1] = a * invDet;
			r2[0][0] = mat[2 * 4 + 0] * r0[0][0] + mat[2 * 4 + 1] * r0[1][0];
			r2[0][1] = mat[2 * 4 + 0] * r0[0][1] + mat[2 * 4 + 1] * r0[1][1];
			r2[1][0] = mat[3 * 4 + 0] * r0[0][0] + mat[3 * 4 + 1] * r0[1][0];
			r2[1][1] = mat[3 * 4 + 0] * r0[0][1] + mat[3 * 4 + 1] * r0[1][1];
			mat[2 * 4 + 0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
			mat[2 * 4 + 1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
			mat[3 * 4 + 0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
			mat[3 * 4 + 1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];
			mat[0 * 4 + 0] = r0[0][0] - r1[0][0] * mat[2 * 4 + 0] - r1[0][1] * mat[3 * 4 + 0];
			mat[0 * 4 + 1] = r0[0][1] - r1[0][0] * mat[2 * 4 + 1] - r1[0][1] * mat[3 * 4 + 1];
			mat[1 * 4 + 0] = r0[1][0] - r1[1][0] * mat[2 * 4 + 0] - r1[1][1] * mat[3 * 4 + 0];
			mat[1 * 4 + 1] = r0[1][1] - r1[1][0] * mat[2 * 4 + 1] - r1[1][1] * mat[3 * 4 + 1];
			mat[0 * 4 + 2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
			mat[0 * 4 + 3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
			mat[1 * 4 + 2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
			mat[1 * 4 + 3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];
			mat[2 * 4 + 2] = -r3[0][0];
			mat[2 * 4 + 3] = -r3[0][1];
			mat[3 * 4 + 2] = -r3[1][0];
			mat[3 * 4 + 3] = -r3[1][1];

			return true;
		}

		void setTranslation(const v3f& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }
		void setTranslation(const v4f& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }
		void setScale(const v3f& v)
		{
			m_data[0u].x = v.x;
			m_data[1u].y = v.y;
			m_data[2u].z = v.z;
		}

		void setScale(const v4f& v)
		{
			m_data[0u].x = v.x;
			m_data[1u].y = v.y;
			m_data[2u].z = v.z;
		}

		void setBasis(const Mat4& other)
		{
			m_data[0].x = other.m_data[0].x;
			m_data[0].y = other.m_data[0].y;
			m_data[0].z = other.m_data[0].z;
			m_data[1].x = other.m_data[1].x;
			m_data[1].y = other.m_data[1].y;
			m_data[1].z = other.m_data[1].z;
			m_data[2].x = other.m_data[2].x;
			m_data[2].y = other.m_data[2].y;
			m_data[2].z = other.m_data[2].z;
		}

		Mat4 getBasis() {
			Mat4 other;
			other.m_data[0].x = m_data[0].x;
			other.m_data[0].y = m_data[0].y;
			other.m_data[0].z = m_data[0].z;
			other.m_data[1].x = m_data[1].x;
			other.m_data[1].y = m_data[1].y;
			other.m_data[1].z = m_data[1].z;
			other.m_data[2].x = m_data[2].x;
			other.m_data[2].y = m_data[2].y;
			other.m_data[2].z = m_data[2].z;
			return other;
		}
	};

	class Mat3
	{
	public:
		v3f m_data[3];

		Mat3()
		{
			identity();
		}

		Mat3(const Mat3& m)
		{
			*this = m;
		}

		Mat3(const Quat& q)
		{
			setRotation(q);
		}

		Mat3(float v)
		{
			fill(v);
		}

		Mat3(const v3f& x, const v3f& y, const v3f& z) {
			m_data[0u] = x;
			m_data[1u] = y;
			m_data[2u] = z;
		}

		void		fill(float v) {
			m_data[0u].set(v);
			m_data[1u].set(v);
			m_data[2u].set(v);
		}

		void		zero() {
			fill(0.f);
		}


		void		identity() {
			auto* p = this->getPtr();
			p[0] = 1.f;
			p[1] = 0.f;
			p[2] = 0.f;

			p[3] = 0.f;
			p[4] = 1.f;
			p[5] = 0.f;

			p[6] = 0.f;
			p[7] = 0.f;
			p[8] = 1.f;
		}

		void setValue(float xx, float xy, float xz,
			float yx, float yy, float yz,
			float zx, float zy, float zz)
		{
			m_data[0].set(xx, xy, xz);
			m_data[1].set(yx, yy, yz);
			m_data[2].set(zx, zy, zz);
		}

		void setScale(const v3f& v)
		{
			m_data[0u].x = v.x;
			m_data[1u].y = v.y;
			m_data[2u].z = v.z;
		}

		void setScale(const v4f& v)
		{
			m_data[0u].x = v.x;
			m_data[1u].y = v.y;
			m_data[2u].z = v.z;
		}

		void setRotation(const Quat& q)
		{
			float d = q.length2();
			float s = 2.0f / d;
			float xs = q.x * s, ys = q.y * s, zs = q.z * s;
			float wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
			float xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
			float yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
			setValue(
				1.0f - (yy + zz), xy - wz, xz + wy,
				xy + wz, 1.0f - (xx + zz), yz - wx,
				xz - wy, yz + wx, 1.0f - (xx + yy));
		}

		float* getPtr() { return reinterpret_cast<float*>(&m_data); }
		float* getPtrConst()const { return (float*)&m_data; }

		v3f& operator[](uint32_t i) { assert(i <= 3); return m_data[i]; }
		const v3f& operator[](uint32_t i) const { assert(i <= 3); return m_data[i]; }
	};

	namespace math
	{

		//	create perspective matrix for left hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
		void  makePerspectiveLHMatrix(Mat4& out, float FOV, float aspect, float Near, float Far);

		//	create perspective matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
		void  makePerspectiveRHMatrix(Mat4& out, float FOV, float aspect, float Near, float Far);

		//	create orthogonal matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param width: width of viewport
		// \param height: height of viewport
		// \param Near: near clip plane
		// \param Far: far clip plane
		void  makeOrthoRHMatrix(Mat4& out, float width, float height, float Near, float Far);

		// based on XNA math library
		void makeMatrixOrthographicOffCenterRH(
			Mat4& out,
			float ViewLeft,
			float ViewRight,
			float ViewBottom,
			float ViewTop,
			float NearZ,
			float FarZ
		);

		//	create \a look \a at matrix for right hand coordinate system
		// \param in_out: view matrix
		// \param eye: camera position
		// \param center: camera target
		// \param up: up vector
		void makeLookAtRHMatrix(Mat4& out, const v4f& eye, const v4f& center, const v4f& up);

		void makeLookAtLHMatrix(const v4f& eye, const v4f& center, const v4f& up, Mat4& out);

		void makeTranslationMatrix(const v4f& position, Mat4& out);

		void makeScaleMatrix(const v4f& scale, Mat4& out);

		//Vector-matrix product 
		v4f mul(const v4f& vec, const Mat4& mat);
		v3f mul(const v3f& vec, const Mat4& mat);
		v3f mulBasis(const v3f& vec, const Mat4& mat);
		Quat matToQuat(const Mat4& mat);

	}//end math::

}


#endif