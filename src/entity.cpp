/*****************************************************************************
*
* entity.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "entity.h"

#include "component.h"

using namespace CEngine;

Entity::Entity()
{
}

Entity::~Entity()
{
}

const std::shared_ptr<Component> Entity::operator [] (const Ceng::StringUtf8 &key)
{
	return components[key];
}

const EngineResult::value Entity::AddComponent(const Ceng::StringUtf8 &name, const std::shared_ptr<Component> &component)
{
	// Link with existing components
	for (auto &x : components)
	{
		component->AddComponent(x.first, x.second.get());
		x.second->AddComponent(name, component.get());
	}

	components[name] = component;

	return EngineResult::ok;
}

const EngineResult::value Entity::RemoveComponent(const Ceng::StringUtf8 &name)
{
	components.erase(name);

	// Remove links
	for (auto &x : components)
	{
		x.second->RemoveComponent(name);
	}

	return EngineResult::ok;
}
