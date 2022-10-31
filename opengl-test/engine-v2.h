/*****************************************************************************
*
* eng-object.h
*
* By Jari Korkala 10/2011
*
*****************************************************************************/

#ifndef _ENG_OBJECT_H
#define _ENG_OBJECT_H

#include <ceng.h>

#include "render-params.h"

#include "env-probe-shader.h"
#include "env-map-manager.h"

#include "shader-manager.h"
#include "shader.h"
#include "shader-program.h"
#include "texture.h"
#include "texture-manager.h"

#include "material-manager.h"
#include "material.h"

#include "mesh.h"
#include "mesh-manager.h"
#include "mesh-action.h"

#include "component-factory.h"
#include "entity-type.h"
#include "entity-registry.h"

#include "entity.h"
#include "position-component.h"
#include "rotation-component.h"
#include "mesh-component.h"
#include "point-light-component.h"

#include "point-light-shader.h"
#include "direction-light-shader.h"
#include "depth-pass.h"
#include "shadowmap-component.h"

#include "spot-light-component.h"
#include "spot-light-shader.h"
#include "spot-shadow-shader.h"

#include "math-vector.h"

#include "env-map.h"

#include "basic-types.h"

#include "camera.h"
#include "engine-object.h"

namespace CEngine
{
	constexpr Ceng::UINT32 majorVersion = 0;
	constexpr Ceng::UINT32 minorVersion = 1;
	constexpr Ceng::UINT32 build = 113;
}


#include "math-vector.h"

struct ProgVertexData
{
	CEngine::Vec3 location;
	CEngine::Vec3 normal;
	CEngine::Vec3 tangent;
	CEngine::Vec3 binormal;

	CEngine::TexelF2 texCoord;
};



struct SurfaceData
{
	Ceng::INT32 vertexIndices[8];
};


class Actor : public CEngine::EngineObject
{
protected:

public:
	Actor();
	virtual ~Actor();

//	Ceng::CRESULT SetMesh(Mesh *meshPtr);
};

const Ceng::String ImageFormatToString(const Ceng::IMAGE_FORMAT::value format);

const Ceng::CRESULT CreateTexture2dFromFile(const Ceng::StringUtf8 &file, Ceng::Texture2dDesc &desc,
	Ceng::RenderDevice *renderDevice, Ceng::Texture2D **out_texture);

namespace TextureError
{
	enum value
	{
		ok,
		invalid_param,
		file_not_found,
		unexpected_eof,
		io_error,
		unknown_file_format,
		invalid_texture_type,
		texture_type_mismatch,
		failed,
		out_of_memory,
	};
}

#include "object-error.h"

#endif