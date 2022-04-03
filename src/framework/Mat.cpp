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


#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/Mat.h"

using namespace mgf;

void  math::makePerspectiveLHMatrix(Mat4& out, float FOV, float aspect, float Near, float Far)
{
	float S = std::sin(0.5f * FOV);
	float C = std::cos(0.5f * FOV);
	float H = C / S;
	float W = H / aspect;
	out[0] = v4f(W, 0.f, 0.f, 0.f);
	out[1] = v4f(0.f, H, 0.f, 0.f);
	out[2] = v4f(0.f, 0.f, Far / (Far - Near), 1.f);
	out[3] = v4f(0.f, 0.f, -out[2].z * Near, 0.f);
}

void  math::makePerspectiveRHMatrix(Mat4& out, float FOV, float aspect,
	float Near, float Far)
{
	float S = std::sin(0.5f * FOV);
	float C = std::cos(0.5f * FOV);
	float H = C / S;
	float W = H / aspect;
	out[0u] = v4f(W, 0.f, 0.f, 0.f);
	out[1u] = v4f(0.f, H, 0.f, 0.f);
	out[2u] = v4f(0.f, 0.f, Far / (Near - Far), -1.f);
	out[3u] = v4f(0.f, 0.f, out[2u].z * Near, 0.f);
}

void  math::makeOrthoRHMatrix(Mat4& out, float width, float height,	float Near, float Far)
{
	out[0u] = v4f(2.f / width, 0.f, 0.f, 0.f);
	out[1u] = v4f(0.f, 2.f / height, 0.f, 0.f);
	out[2u] = v4f(0.f, 0.f, 1.f / (Near - Far), 0.f);
	out[3u] = v4f(0.f, 0.f, out[2u].z * Near, 1.f);
}

void math::makeMatrixOrthographicOffCenterRH(
	Mat4& out,
	float ViewLeft,
	float ViewRight,
	float ViewBottom,
	float ViewTop,
	float NearZ,
	float FarZ
)
{
	float    ReciprocalWidth;
	float    ReciprocalHeight;

	ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
	ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

	out.m_data[0].set(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
	out.m_data[1].set(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
	out.m_data[2].set(0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f);
	out.m_data[3].set(-(ViewLeft + ViewRight) * ReciprocalWidth,
		-(ViewTop + ViewBottom) * ReciprocalHeight,
		out.m_data[2].z * NearZ,
		1.0f);
}

void  math::makeLookAtRHMatrix(Mat4& out, const v4f& eye, const v4f& center, const v4f& up)
{
	v4f f(center - eye);
	f.normalize2();

	v4f s;
	f.cross2(up, s);
	s.normalize2();

	v4f u;
	s.cross2(f, u);

	out.identity();

	out[0u].x = s.x;
	out[1u].x = s.y;
	out[2u].x = s.z;
	out[0u].y = u.x;
	out[1u].y = u.y;
	out[2u].y = u.z;
	out[0u].z = -f.x;
	out[1u].z = -f.y;
	out[2u].z = -f.z;
	out[3u].x = -s.dot(eye);
	out[3u].y = -u.dot(eye);
	out[3u].z = f.dot(eye);
}

void math::makeLookAtLHMatrix(const v4f& eye, const v4f& center, const v4f& up, Mat4& out)
{
	v4f f(center - eye);
	f.normalize2();

	v4f s;
	f.cross2(up, s);
	s.normalize2();

	v4f u;
	s.cross2(f, u);

	out.identity();

	out[0u].x = s.x;
	out[1u].x = s.y;
	out[2u].x = s.z;
	out[0u].y = u.x;
	out[1u].y = u.y;
	out[2u].y = u.z;
	out[0u].z = f.x;
	out[1u].z = f.y;
	out[2u].z = f.z;
	out[3u].x = -s.dot(eye);
	out[3u].y = -u.dot(eye);
	out[3u].z = -f.dot(eye);
}

void math::makeTranslationMatrix(const v4f& position, Mat4& out)
{
	out[3u].x = position.x;
	out[3u].y = position.y;
	out[3u].z = position.z;
}

void math::makeScaleMatrix(const v4f& scale, Mat4& out)
{
	out[0u].x = scale.x;
	out[1u].y = scale.y;
	out[2u].z = scale.z;
}

//Vector-matrix product 
v4f math::mul(const v4f& vec, const Mat4& mat)
{
	return v4f
	(
		mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z + mat[3u].x * vec.w,
		mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z + mat[3u].y * vec.w,
		mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z + mat[3u].z * vec.w,
		mat[0u].w * vec.x + mat[1u].w * vec.y + mat[2u].w * vec.z + mat[3u].w * vec.w
	);
}

v3f math::mul(const v3f& vec, const Mat4& mat)
{
	return v3f
	(
		mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z + mat[3u].x,
		mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z + mat[3u].y,
		mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z + mat[3u].z
	);
}

v3f math::mulBasis(const v3f& vec, const Mat4& mat)
{
	return v3f
	(
		mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z,
		mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z,
		mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z
	);
}

Quat math::matToQuat(const Mat4& mat)
{
	Quat q;
	float trace = mat[0].x + mat[1].y + mat[2].z;
	if (trace > 0.0f)
	{

		float t = trace + 1.0f;
		float s = std::sqrt(t) * 0.5f;

		q.w = s * t;
		q.x = (mat[2].y - mat[1].z) * s;
		q.y = (mat[0].z - mat[2].x) * s;
		q.z = (mat[1].x - mat[0].y) * s;

	}
	else
	{
		static int32_t	next[3] = { 1, 2, 0 };
		int32_t i = 0;
		if (mat[1][1] > mat[0][0])
			i = 1;
		if (mat[2][2] > mat[i][i])
			i = 2;
		int32_t j = next[i];
		int32_t k = next[j];

		float t = (mat[i][i] - (mat[j][j] + mat[k][k])) + 1.0f;
		float s = std::sqrt(t) * 0.5f;

		q[i] = s * t;
		q[3] = (mat[k][j] - mat[j][k]) * s;
		q[j] = (mat[j][i] + mat[i][j]) * s;
		q[k] = (mat[k][i] + mat[i][k]) * s;
	}
	return q;
}