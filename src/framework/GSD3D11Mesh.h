#pragma once
#ifndef __MGF_GSD3D11MESH_H_
#define __MGF_GSD3D11MESH_H_

#include "Mesh.h"
#include "GSMesh.h"

namespace mgf
{

	class GSD3D11Mesh : public GSMesh
	{
	public:
		GSD3D11Mesh(GSMeshInfo*, ID3D11Device*, ID3D11DeviceContext*);
		virtual ~GSD3D11Mesh();
		virtual void MapModelForWriteVerts(uint8_t** v_ptr) override;
		virtual void UnmapModelForWriteVerts() override;

		ID3D11DeviceContext* m_d3d11DevCon = 0;
		ID3D11Buffer* m_lockedResource = 0;
		ID3D11Buffer* m_vBuffer = 0;
		ID3D11Buffer* m_iBuffer = 0;
		uint32_t m_stride = 0;
		uint32_t m_iCount = 0;

		DXGI_FORMAT m_indexType = DXGI_FORMAT_R16_UINT;

		Mesh::MeshVertexType m_vertexType = Mesh::MeshVertexType_Triangle;
	};

}
#endif
