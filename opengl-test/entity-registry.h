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

#include "json.h"

#include "engine-result.h"

namespace CEngine
{
	class Entity;
	class EntityType;
	class ComponentFactory;

	class EntityRegistry
	{
	public:
		std::unordered_map<Ceng::StringUtf8, std::shared_ptr<EntityType>> entityTypes;

		std::unordered_map<Ceng::StringUtf8, std::shared_ptr<ComponentFactory>> componentFactories;

	public:

		EntityRegistry();
		~EntityRegistry();

		EngineResult::value AddComponentFactory(const Ceng::StringUtf8& name, 
			std::shared_ptr<ComponentFactory>& factory);

		EngineResult::value RemoveComponentFactory(const Ceng::StringUtf8& name);

		EngineResult::value RemoveComponentFactory(std::shared_ptr<ComponentFactory>& factory);

		EngineResult::value ReplaceComponentFactory(const Ceng::StringUtf8& name,
			std::shared_ptr<ComponentFactory>& factory);

		EngineResult::value AddEntityType(const Ceng::StringUtf8& name, 
			std::shared_ptr<EntityType>& type);

		EngineResult::value RemoveEntityType(const Ceng::StringUtf8& name);

		EngineResult::value RemoveEntityType(std::shared_ptr<EntityType>& type);

		EngineResult::value ReplaceEntityType(const Ceng::StringUtf8& name, 
			std::shared_ptr<EntityType>& type);

		EngineResult::value GetEntityInstance(const Ceng::StringUtf8& name, 
			json& initialValues, std::shared_ptr<Entity>& output);

	};
}

#endif