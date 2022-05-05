#pragma once
#ifndef __MGF_GS_H_
#define __MGF_GS_H_

#include "framework/Vec.h"
#include "framework/Mat.h"

#include "framework/GSTexture.h"
#include "framework/GSMesh.h"

namespace mgf
{
	

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
		DriverType m_videoDriverType;
		SystemWindow* m_activeWindow;
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
		virtual void Draw() = 0;

		/// <summary>
		/// Copy GPU texture data to mgf::Image.
		/// </summary>
		/// <param name="t">- texture</param>
		/// <param name="i">- image. must be with same parameters with t</param>
		virtual void GetTextureCopyForImage(GSTexture* t, Image* i) = 0;
	};

}
#endif
