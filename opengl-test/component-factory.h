/*****************************************************************************
*
* component-factory.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_COMPONENT_FACTORY_H
#define CENGINE_COMPONENT_FACTORY_H

#include <ceng.h>
#include "json.h"

#include "engine-result.h"

namespace CEngine
{
	class Component;

	class ComponentFactory
	{
	public:
		Ceng::StringUtf8 name;

	public:
		ComponentFactory();
		virtual ~ComponentFactory();

		virtual EngineResult::value GetInstance(json& data, Component** output) = 0;
	};

}

#endif
