/*****************************************************************************
*
* entity-registry.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "entity-registry.h"
#include "entity.h"
#include "entity-type.h"
#include "component.h"
#include "component-factory.h"

using namespace CEngine;

EntityRegistry::EntityRegistry()
{

}

EntityRegistry::~EntityRegistry()
{

}

EngineResult::value EntityRegistry::GetEntityInstanceCommon(const Ceng::StringUtf8& name,
	std::shared_ptr<Entity>& output)
{
	Ceng::Log::Print("EntityRegistry::GetEntityInstance");
	Ceng::Log::Print("Create instance of type: ");
	Ceng::Log::Print(name);

	output = nullptr;

	const auto typeIter = entityTypes.find(name);

	if (typeIter == entityTypes.end())
	{
		Ceng::Log::Print("Error: entity type not found.");
		return EngineResult::fail;
	}

	auto entity = std::make_shared<Entity>();

	for (auto& iter : typeIter->second->sharedComponents)
	{
		entity->AddComponent(iter.first, iter.second);
	}

	// Deep copy default components
	for (auto& iter : typeIter->second->defaultComponents)
	{
		entity->AddComponent(iter.first, iter.second->Clone());
	}

	output = entity;

	return EngineResult::ok;

}

EngineResult::value EntityRegistry::GetEntityInstance(const Ceng::StringUtf8& name,
	std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Component>>& initialValues,
	std::shared_ptr<Entity>& output)
{
	EngineResult::value eresult;

	std::shared_ptr<Entity> entity;

	eresult = GetEntityInstanceCommon(name, entity);

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	// Guaranteed to exist at this point
	const auto typeIter = entityTypes.find(name);

	for (auto& requiredCompName : typeIter->second->requiredComponents)
	{
		// NOTE: no need to check for factories since we get ready components from caller

		auto iter = initialValues.find(requiredCompName);

		if (iter == initialValues.end())
		{
			Ceng::Log::Print("Error: required component not in initializer list:");
			Ceng::Log::Print(requiredCompName);
			return EngineResult::fail;
		}

		eresult = entity->AddComponent(iter->first, iter->second);

		if (eresult != EngineResult::ok)
		{
			return eresult;
		}
	}

	output = entity;

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::GetEntityInstance(const Ceng::StringUtf8& name,
	json& initialValues, std::shared_ptr<Entity>& output)
{
	output = nullptr;

	EngineResult::value eresult;

	std::shared_ptr<Entity> entity;

	eresult = GetEntityInstanceCommon(name, entity);

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}
	
	// Guaranteed to exist at this point
	const auto typeIter = entityTypes.find(name);

	for (auto& requiredCompName : typeIter->second->requiredComponents)
	{
		auto factory = componentFactories.find(requiredCompName);

		if (factory == componentFactories.end())
		{
			Ceng::Log::Print("Error: factory for required component not found:");
			Ceng::Log::Print(requiredCompName);
			return EngineResult::fail;
		}

		bool found = false;

		// Add required components
		for (auto iter = initialValues.begin(); iter != initialValues.end(); ++iter)
		{
			if (iter.key() != requiredCompName.ToCString())
			{
				continue;
			}

			std::shared_ptr<Component> component;

			eresult = factory->second->GetInstance(iter.value(), component);

			if (eresult != EngineResult::ok)
			{
				return eresult;
			}

			eresult = entity->AddComponent(iter.key(), component);

			if (eresult != EngineResult::ok)
			{
				return eresult;
			}

			found = true;

			break;
		}

		if (found == false)
		{
			Ceng::Log::Print("Error: required component not in initializer list:");
			Ceng::Log::Print(requiredCompName);
			return EngineResult::fail;
		}
	}	

	output = entity;

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::AddComponentFactory(const Ceng::StringUtf8& name,
	std::shared_ptr<ComponentFactory>& factory)
{
	if (componentFactories.find(name) != componentFactories.end())
	{
		return EngineResult::fail;
	}

	componentFactories[name] = factory;

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::RemoveComponentFactory(const Ceng::StringUtf8& name)
{
	auto iter = componentFactories.find(name);

	if (iter == componentFactories.end())
	{
		return EngineResult::fail;
	}

	componentFactories.erase(iter);

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::RemoveComponentFactory(std::shared_ptr<ComponentFactory>& factory)
{
	/*
	for (auto& iter : componentFactories)
	{
		if (iter.second == factory)
		{
			componentFactories.erase(iter);

			return EngineResult::ok;
		}
	}
	*/

	return EngineResult::fail;
}

EngineResult::value EntityRegistry::ReplaceComponentFactory(const Ceng::StringUtf8& name,
	std::shared_ptr<ComponentFactory>& factory)
{
	auto iter = componentFactories.find(name);

	if (iter == componentFactories.end())
	{
		return EngineResult::fail;
	}

	componentFactories[name] = factory;

	return EngineResult::ok;	
}

EngineResult::value EntityRegistry::AddEntityType(const Ceng::StringUtf8& name, 
	std::shared_ptr<EntityType>& type)
{
	if (entityTypes.find(name) != entityTypes.end())
	{
		return EngineResult::fail;
	}

	entityTypes[name] = type;

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::RemoveEntityType(const Ceng::StringUtf8& name)
{
	auto iter = entityTypes.find(name);

	if (iter == entityTypes.end())
	{
		return EngineResult::fail;
	}

	entityTypes.erase(iter);

	return EngineResult::ok;
}

EngineResult::value EntityRegistry::RemoveEntityType(std::shared_ptr<EntityType>& type)
{
	/*
	for (auto& iter : entityTypes)
	{
		if (iter.second == type)
		{
			entityTypes.erase(iter);

			return EngineResult::ok;
		}
	}
	*/

	return EngineResult::fail;
}

EngineResult::value EntityRegistry::ReplaceEntityType(const Ceng::StringUtf8& name, 
	std::shared_ptr<EntityType>& type)
{
	auto iter = entityTypes.find(name);

	if (iter == entityTypes.end())
	{
		return EngineResult::fail;
	}

	entityTypes[name] = type;

	return EngineResult::ok;
}

