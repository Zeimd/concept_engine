/*****************************************************************************
*
* mesh-manager.h
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#ifndef CENGINE_MESH_MANAGER_H
#define CENGINE_MESH_MANAGER_H

#include <memory>

#include <ceng/datatypes/ce-string.h>

#include "manager.h"
#include "object-error.h"

namespace CEngine
{
	class Mesh;
	class MaterialManager;

	class MeshManager : public Manager
	{
	public:
		Ceng::UINT32 maxIndexBytes;
		Ceng::UINT32 maxIndexValue;
		
		std::vector<Ceng::VertexDeclData> vertexDecl;

		Ceng::VertexFormat *vertexFormat;

		Ceng::UINT32 iBufferMax;
		Ceng::UINT32 vBufferMax;

		MaterialManager *materialManager;
		Ceng::RenderDevice *renderDevice;

		MeshManager();
	public:
		virtual ~MeshManager();

		const Ceng::INT32 FindVertexVar(const Ceng::StringUtf8 &name);

		MeshManager(MaterialManager *materialManager,Ceng::RenderDevice *renderDevice,const Ceng::UINT32 maxIndexBytes,
			const std::vector<Ceng::VertexDeclData> &vertexFormat);

		const EngineResult::value LoadMesh(const Ceng::StringUtf8 &file,const Ceng::StringUtf8 &objectName,
			std::shared_ptr<Mesh> &out);

		const ObjectError::value LoadMesh_OBJ(const Ceng::StringUtf8 &filename, 
			const Ceng::StringUtf8 &materialFile,const Ceng::StringUtf8 &objectName,
			std::shared_ptr<Mesh> &out);

	};
}


#endif
