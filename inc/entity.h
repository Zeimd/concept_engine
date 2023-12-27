/*****************************************************************************
*
* entity.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_ENTITY_H
#define CENGINE_ENTITY_H

#include <memory>
#include <unordered_map>

#include <ceng/datatypes.h>

#include "engine-result.h"

namespace CEngine
{
	class Component;

	class Entity
	{
	public:
		std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Component>> components;

	public:
		Entity();
		virtual ~Entity();

		const EngineResult::value AddComponent(const Ceng::StringUtf8 &name, const std::shared_ptr<Component> &component);

		const EngineResult::value RemoveComponent(const Ceng::StringUtf8 &name);

		const std::shared_ptr<Component> operator [] (const Ceng::StringUtf8 &key);		
	};
};

#endif