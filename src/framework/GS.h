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
#ifndef __MGF_GS_H_
#define __MGF_GS_H_

#include "framework/Vec.h"
#include "framework/Mat.h"

#include "framework/GSTexture.h"
#include "framework/GSMesh.h"
#include "framework/GSShader.h"

namespace mgf
{
	struct GSData_D3D11
	{
		void* dev;
		void* con;
	};

	class GS : public BaseClass
	{
	public:
		enum DriverType
			{
				DriverType_Unknown,
		#ifdef MGF_GS_D3D11
				DriverType_Direct3D11
		#endif
			};

	protected:
		DriverType m_videoDriverType = DriverType_Unknown;
		SystemWindow* m_activeWindow = 0;
	public:
		GS();
		virtual ~GS();

		enum MatrixType
		{
			MatrixType_World,
			MatrixType_View,
			MatrixType_Projection,
			MatrixType_ViewProjection, //For 3d line
			MatrixType_WorldViewProjection,
			MatrixType_ViewInvert,
			MatrixType_GUIProjection,
			MatrixType__count
		};

		Mat4 m_matrices[MatrixType__count];

		DriverType GetVideoDriverType();
		SystemWindow* GetActiveWindow();

		virtual void UseVSync(bool) = 0;
		virtual bool UseDepth(bool) = 0; // return old
		virtual bool UseBlend(bool) = 0; // return old
		virtual void GetDepthRange(v2f*) = 0;
		virtual void SetClearColor(float* rgba) = 0;
		virtual void BeginDraw() = 0;
		virtual void ClearDepth() = 0;
		virtual void ClearColor() = 0;
		virtual void ClearAll() = 0;    // ClearDepth и ClearColor
		virtual void EndDraw() = 0;
		virtual void SwapBuffers() = 0;       // Present\swapbuffers
		virtual void UpdateMainRenderTarget(const v2f& bufferSize) = 0;
		virtual GSTexture* CreateTexture(GSTextureInfo*) = 0;
		virtual GSTexture* CreateRenderTargetTexture(uint32_t w, uint32_t h, GSTextureInfo* optional) = 0;
		virtual GSMesh* CreateMesh(GSMeshInfo*) = 0;
		virtual void SetRenderTarget(GSTexture*) = 0;
		virtual void SetViewport(float x, float y, float width, float height, SystemWindow* window, v4f* old) = 0;
		virtual void SetScissorRect(const v4f& rect, SystemWindow* window, v4f* old) = 0;
		virtual const wchar_t* GetVideoDriverName() = 0;

		virtual void SetTexture(uint32_t slot, GSTexture*) = 0;
		virtual void SetMesh(GSMesh*) = 0;

		enum
		{
			drawFlag_cullBackFace = 0x1,
			drawFlag_wireframe = 0x2,
		};

		virtual void Draw(uint32_t flags) = 0;

		/// <summary>
		/// Before using need to set matrix: MatrixType_GUIProjection
		/// </summary>
		virtual void DrawLine2D(const v3f& p1, const v3f& p2, const mgColor& c) = 0;

		/// <summary>
		/// Before using need to set matrix: MatrixType_ViewProjection
		/// </summary>
		virtual void DrawLine3D(const v4f& p1, const v4f& p2, const mgColor& c) = 0;

		/// <summary>
		/// Copy GPU texture data to mgf::Image.
		/// </summary>
		/// <param name="t">- texture</param>
		/// <param name="i">- image. must be with same parameters with t</param>
		virtual void GetTextureCopyForImage(GSTexture* t, Image* i) = 0;
	};

}
#endif
