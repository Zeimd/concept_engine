/*****************************************************************************
*
* mesh.h
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#ifndef CENGINE_MESH_H
#define CENGINE_MESH_H

#include <ceng.h>

namespace CEngine
{
	class MeshCommand;

	class Mesh
	{
	public:

		std::vector<Ceng::IndexBuffer*> indexBuffers;
		std::vector<Ceng::VertexBuffer*> vertexBuffers;

		std::vector<std::unique_ptr<MeshCommand>> commands;

		/**
		 * Commands when rendering to a shadow map.
		 */
		std::vector<std::unique_ptr<MeshCommand>> shadowCommands;

	public:
		Mesh();
		virtual ~Mesh();

		void Render(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform,Ceng::SamplerState *materialSampler);

		void ShadowRender(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform);
	};

}

#endif
