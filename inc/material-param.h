/*****************************************************************************
*
* material-param.h
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#ifndef CENGINE_MATERIAL_PARAM_H
#define CENGINE_MATERIAL_PARAM_H

#include <ceng.h>
#include "texture.h"

namespace CEngine
{
	class MaterialParam
	{
	public:
		Ceng::ShaderConstant *uniform;

		MaterialParam() : uniform(nullptr)
		{
		}

	public:

		MaterialParam(Ceng::ShaderConstant * uniform) : uniform(uniform)
		{
		}

		virtual ~MaterialParam()
		{
			uniform->Release();
		}

		virtual void Apply(Ceng::RenderContext *context,Ceng::SamplerState *textureSampler) = 0;
	};

	class MaterialTexture : public MaterialParam
	{
	public:
		Ceng::UINT32 slot;
		std::shared_ptr<Texture> texture;

		Ceng::ShaderResourceView *texView;



	public:
		MaterialTexture() : slot(0),texture(nullptr)
		{
		}

		MaterialTexture(const Ceng::UINT32 slot, Ceng::ShaderConstant *uniform,const std::shared_ptr<Texture> &texture,
			Ceng::ShaderResourceView *texView)
			: MaterialParam(uniform),slot(slot),texture(texture),texView(texView)
		{
		}

		virtual ~MaterialTexture()
		{
			texView->Release();
		}

		virtual void Apply(Ceng::RenderContext *context,Ceng::SamplerState *textureSampler) override
		{
			uniform->SetInt(slot);

			context->SetPixelShaderResource(slot, texView);

			context->SetPixelShaderSamplerState(slot, textureSampler);

			// TODO: 
			// Find free texture slot
			// Assign texture
		}
	};

	class MaterialFloat : public MaterialParam
	{
	public:
		Ceng::FLOAT32 value;

	public:
		MaterialFloat() : value(0.0f)
		{
		}

		MaterialFloat(const Ceng::FLOAT32 value,Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),value(value)
		{
		}

		MaterialFloat(const Ceng::FLOAT32 *in, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),value(in[0])
		{
		}

		virtual ~MaterialFloat()
		{
		}

		virtual void Apply(Ceng::RenderContext *context, Ceng::SamplerState *textureSampler) override
		{
			uniform->SetFloat(value);
		}
	};

	class MaterialFloat2 : public MaterialParam
	{
	public:
		Ceng::FLOAT32 values[2];

	public:
		MaterialFloat2() : values{ 0.0f,0.0f }
		{
		}

		MaterialFloat2(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y,Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ x,y }
		{
		}

		MaterialFloat2(const Ceng::FLOAT32 *in, Ceng::ShaderConstant *uniform) : MaterialParam(uniform),values{ in[0],in[1] }
		{
		}

		virtual ~MaterialFloat2()
		{
		}

		virtual void Apply(Ceng::RenderContext *context, Ceng::SamplerState *textureSampler) override
		{
			uniform->SetFloat2(values);
		}
	};

	class MaterialFloat3 : public MaterialParam
	{
	public:
		Ceng::FLOAT32 values[3];

	public:
		MaterialFloat3() : values{ 0.0f,0.0f,0.0f }
		{
		}

		MaterialFloat3(const Ceng::FLOAT32 x,Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ x,0.0f,0.0f}
		{
		}

		MaterialFloat3(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ x,y,0.0f }
		{
		}

		MaterialFloat3(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, 
			const Ceng::FLOAT32 z, Ceng::ShaderConstant *uniform) : MaterialParam(uniform),values{ x,y,z }
		{
		}

		MaterialFloat3(const Ceng::FLOAT32 *in, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ in[0],in[1],in[2] }
		{
		}

		virtual ~MaterialFloat3()
		{
		}

		virtual void Apply(Ceng::RenderContext *context, Ceng::SamplerState *textureSampler) override
		{
			uniform->SetFloat3(values);
		}
	};

	class MaterialFloat4 : public MaterialParam
	{
	public:
		Ceng::FLOAT32 values[4];

	public:
		MaterialFloat4() : values{ 0.0f,0.0f,0.0f, 1.0f }
		{
		}

		MaterialFloat4(const Ceng::FLOAT32 x, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ x,0.0f,0.0f, 1.0f }
		{
		}

		MaterialFloat4(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, 
			Ceng::ShaderConstant *uniform) : MaterialParam(uniform),values{ x,y,0.0f, 1.0f }
		{
		}

		MaterialFloat4(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, 
			const Ceng::FLOAT32 z, Ceng::ShaderConstant *uniform) : MaterialParam(uniform),values{ x,y,z, 1.0f }
		{
		}

		MaterialFloat4(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, const Ceng::FLOAT32 z,
			const Ceng::FLOAT32 w, Ceng::ShaderConstant *uniform) : MaterialParam(uniform),values{ x,y,z, w }
		{
		}

		MaterialFloat4(const Ceng::FLOAT32 *in, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),values{ in[0],in[1],in[2],in[3] }
		{
		}

		virtual ~MaterialFloat4()
		{
		}

		virtual void Apply(Ceng::RenderContext *context, Ceng::SamplerState *textureSampler) override
		{
			uniform->SetFloat4(values);
		}
	};

	class MaterialInt : public MaterialParam
	{
	public:
		Ceng::INT32 value;

	public:
		MaterialInt() : value(0)
		{
		}

		MaterialInt(const Ceng::INT32 value, Ceng::ShaderConstant *uniform) : 
			MaterialParam(uniform),value(value)
		{
		}

		virtual ~MaterialInt()
		{
		}

		virtual void Apply(Ceng::RenderContext *context, Ceng::SamplerState *textureSampler) override
		{
			uniform->SetInt(value);
		}
	};
}

#endif
