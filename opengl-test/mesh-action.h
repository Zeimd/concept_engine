/*****************************************************************************
*
* mesh-command.h
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#ifndef CENGINE_MESH_ACTION_H
#define CENGINE_MESH_ACTION_H

#include <ceng.h>

#include <liblog.h>

#include "material.h"

namespace CEngine
{
	class Mesh;

	class MeshCommand
	{
	public:

	public:
		MeshCommand()
		{
		}

		virtual ~MeshCommand()
		{
		}

		virtual void Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform,Ceng::SamplerState *materialSampler) = 0;
	};

	class MaterialChange : public MeshCommand
	{
	public:
		std::shared_ptr<Material> material;

	public:
		MaterialChange();

		MaterialChange(std::shared_ptr<Material> &material);

		virtual ~MaterialChange();

		virtual void Execute(Ceng::RenderContext *context,Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler) override;
	};

	class VertexBufferChange : public MeshCommand
	{
	public:
		Ceng::VertexBuffer *vbuffer;
		Ceng::UINT32 stride;

	public:
		VertexBufferChange()
		{
		}

		VertexBufferChange(Ceng::VertexBuffer *vbuffer, Ceng::UINT32 stride)
			: vbuffer(vbuffer),stride(stride)
		{
		}

		virtual ~VertexBufferChange()
		{
		}

		virtual void Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler) override
		{
			Ceng::CRESULT cresult;
			cresult = context->SetVertexStream(0, vbuffer, stride, 0);
			if (cresult != Ceng::CE_OK)
			{
				return;
			}
		}
	};

	class IndexBufferChange : public MeshCommand
	{
	public:
		Ceng::IndexBuffer *ibuffer;

	public:
		IndexBufferChange()
		{
		}

		IndexBufferChange(Ceng::IndexBuffer *ibuffer) : ibuffer(ibuffer)
		{
		}

		virtual ~IndexBufferChange()
		{
		}

		virtual void Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler) override
		{
			context->SetIndexBuffer(ibuffer);
		}
	};

	class VertexFormatChange : public MeshCommand
	{
	public:
		Ceng::VertexFormat *format;

	public:
		VertexFormatChange()
		{
		}

		VertexFormatChange(Ceng::VertexFormat *format) : format(format)
		{
		}

		virtual ~VertexFormatChange()
		{
		}

		virtual void Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler) override
		{
			context->SetVertexFormat(format);
		}
	};

	class DrawCommand : public MeshCommand
	{
	public:
		Ceng::PRIMITIVE_TYPE::value type;
		Ceng::UINT32 first;
		Ceng::UINT32 primitiveCount;

	public:

		DrawCommand()
		{
		}

		DrawCommand(const Ceng::PRIMITIVE_TYPE::value type, const Ceng::UINT32 first, const Ceng::UINT32 primitiveCount)
			: type(type),first(first),primitiveCount(primitiveCount)
		{
		}

		virtual ~DrawCommand()
		{
		}

		virtual void Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
			Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler) override
		{
			context->DrawIndexed(type, first, primitiveCount);
		}

	};
}

#endif
