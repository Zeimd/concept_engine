/*****************************************************************************
*
* shadowmap-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_SHADOWMAP_COMPONENT_H
#define CENGINE_SHADOWMAP_COMPONENT_H

#include <ceng.h>

#include "component.h"

namespace CEngine
{
	class PointLightComponent;
	class RotationComponent;
	class PositionComponent;
	class SpotLightComponent;

	class ShadowmapComponent : public Component
	{
	public:

		Ceng::UINT32 textureSize;

		Ceng::Texture2D *depthTexture;
		Ceng::RenderTargetView *depthTarget;
		Ceng::ShaderResourceView *depthView;

		Ceng::Matrix4 projection;
		Ceng::Matrix4 lightTransform;

		Ceng::FLOAT32 zNear;
		Ceng::FLOAT32 depthBias;

		PositionComponent *position;
		PointLightComponent *light;
		RotationComponent *rotation;
		SpotLightComponent *spotData;

		Ceng::RenderDevice *renderDevice;

		enum Type
		{
			undefined = 0 ,
			point = 1,
			directional = 2,
			spot = 3,
		};

		Type type;

	protected:
		ShadowmapComponent();
	public:	

		ShadowmapComponent(Ceng::RenderDevice *renderDevice, const Ceng::UINT32 textureSize,
			const Ceng::FLOAT32 zNear,const Ceng::FLOAT32 depthBias);

		virtual ~ShadowmapComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;
		virtual void RemoveComponent(const Ceng::StringUtf8 &name) override;
	};
}

#endif