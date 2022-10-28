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

		/*
		virtual Component* GetInstance();
		virtual Component* GetInstance(const Ceng::FLOAT32 x);

		virtual Component* GetInstance(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y);

		virtual Component* GetInstance(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, 
			const Ceng::FLOAT32 z);

		virtual Component* GetInstance(const Ceng::INT32 x);
		*/
	};

}

#endif
