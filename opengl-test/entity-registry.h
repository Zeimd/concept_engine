/*****************************************************************************
*
* entity-registry.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENTITY_REGISTRY_H
#define CENGINE_ENTITY_REGISTRY_H

#include <unordered_map>
#include <memory>

#include <ceng.h>

#include "engine-result.h"

namespace CEngine
{
	class EntityType;
	class ComponentFactory;

	class EntityRegistry
	{
	public:
		std::unordered_map < Ceng::StringUtf8, std::shared_ptr<EntityType>> entityTypes;

		std::unordered_map<Ceng::StringUtf8, std::shared_ptr<ComponentFactory>> componentFactories;

	public:

		EntityRegistry();
		~EntityRegistry();

		EngineResult::value AddComponentFactory();

		EngineResult::value AddEntityType();

	};
}

#endif