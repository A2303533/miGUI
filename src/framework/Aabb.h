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
#ifndef __AABB_H_
#define __AABB_H_

#include "framework/Vec.h"
#include "framework/Mat.h"
#include "framework/Ray.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace mgf
{


// Oriented Bounding Box
class Obb
{
public:
	Obb() { }
	v4f v1, v2, v3, v4, v5, v6, v7, v8;
};
// Axis-Aligned Bounding Box
class Aabb
{
public:

	Aabb();
	Aabb(const v4f& min, const v4f& max);
	~Aabb();

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
	void transform(Aabb* original, Mat4* matrix, v4f* position);
	void add(const v4f& point);
	void add(const v3f& point);
	void add(const Aabb& box);
	bool rayTest(const Ray& r);
	void center(v4f& v) const ;
	float radius();
	void extent(v4f& v);
	bool isEmpty() const;
	void reset();

	v4f m_min;
	v4f m_max;
};

}

#endif
