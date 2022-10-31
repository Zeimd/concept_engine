/*****************************************************************************
*
* entity-type.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENTITY_TYPE_H
#define CENGINE_ENTITY_TYPE_H

#include <unordered_map>
#include <utility>
#include <memory>

#include <ceng.h>

namespace CEngine
{
	class Component;

	class EntityType
	{
	public:

		// Components that are shared between all instances
		// Example: mesh, animation definitions, scripts
		std::vector<std::pair<Ceng::StringUtf8, std::shared_ptr<Component>>> sharedComponents;

		// Default components that must be deep copied when creating new entity
		// Example: animation state, script state
		std::vector<std::pair<Ceng::StringUtf8, std::shared_ptr<Component>>> defaultComponents;

		// Components whose initialization must be provided when instance is created.
		// Example: position, 
		std::vector<Ceng::StringUtf8> requiredComponents;
	};
}

#endif