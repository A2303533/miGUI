#pragma once
#ifndef __MGF_GSMESH_H_
#define __MGF_GSMESH_H_

namespace mgf
{
	class Mesh;

	class GSMeshInfo
	{
	public:
		GSMeshInfo() 
			:
		m_meshPtr(0)
		{}
		~GSMeshInfo() {}

		Mesh* m_meshPtr;
	};

	class GSMesh : public BaseClass
	{
	protected:
		GSMeshInfo m_meshInfo;
	public:
		GSMesh() {}
		virtual ~GSMesh() {}

		GSMeshInfo* GetMeshInfo() { return &m_meshInfo; }

		virtual void MapModelForWriteVerts(uint8_t** v_ptr) = 0;
		virtual void UnmapModelForWriteVerts() = 0;
	};
}

#endif
