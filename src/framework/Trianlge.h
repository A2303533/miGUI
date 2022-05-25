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
#ifndef _MGF_TRIANGLE_H_
#define _MGF_TRIANGLE_H_

#include <stdint.h>
#include <float.h>

#include "Vec.h"
#include "Ray.h"

namespace mgf
{
	class Triangle
	{
	public:
		Triangle() {}
		Triangle(const v4f& _v1, const v4f& _v2, const v4f& _v3)
			:
			v1(_v1),
			v2(_v2),
			v3(_v3)
		{
			update();
		}

		Triangle(const v3f& _v1, const v3f& _v2, const v3f& _v3)
			:
			v1(_v1),
			v2(_v2),
			v3(_v3)
		{
			update();
		}

		v4f v1;
		v4f v2;
		v4f v3;
		//v4f faceNormal;
		v4f normal1;
		v4f normal2;
		v4f normal3;
		v4f e1;
		v4f e2;
		v4f t1;
		v4f t2;
		v4f t3;

		void update();
		void center(v4f& out);
		bool rayTest_MT(const Ray& ray, bool withBackFace, float& T, float& U, float& V, float& W);
		bool rayTest_Watertight(const Ray& ray, bool withBackFace, float& T, float& U, float& V, float& W);
	};
}
#endif
